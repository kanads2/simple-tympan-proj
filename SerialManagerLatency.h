#ifndef _SerialManagerLat_h
#define _SerialManagerLat_h

#include <Tympan_Library.h>


extern Tympan myTympan;
extern AudioSDWriter_F32_UI audioSDWriter;


extern float incrementDigitalGain(float);
extern void printGainSettings(void);
extern bool enable_printCPUandMemory;
extern float incrementKnobGain(float);
extern float vol_knob_gain_dB;


class SerialManager : public SerialManagerBase  {  // see Tympan_Library SerialManagerBase for more functions!
  public:
    SerialManager(BLE *_ble) : SerialManagerBase(_ble) {};

    void printHelp(void);
    void createTympanRemoteLayout(void); 
    void printTympanRemoteLayout(void); 
    bool processCharacter(char c);  //this is called automatically by SerialManagerBase.respondToByte(char c)

    //method for updating the GUI on the App
    void setFullGUIState(bool activeButtonsOnly = false);
    

  private:
    TympanRemoteFormatter myGUI;  //Creates the GUI-writing class for interacting with TympanRemote App  
    
    
};


void SerialManager::printHelp(void) {  
  Serial.println();
  Serial.println("SerialManager Help: Available Commands:");
  Serial.println("   h: Print this help");
  
  //Add in the printHelp() that is built-into the other UI-enabled system components.
  //The function call below loops through all of the UI-enabled classes that were
  //attached to the serialManager in the setupSerialManager() function used back
  //in the main *.ino file.
  SerialManagerBase::printHelp();  ////in here, it automatically loops over the different UI elements issuing printHelp()
  
  Serial.println();
}


//switch yard to determine the desired action
bool SerialManager::processCharacter(char c) {
  bool ret_val = true;
  switch (c) {
    case 'h': case '?':
      printHelp(); break;
    case 'J': case 'j':           //The TympanRemote app sends a 'J' to the Tympan when it connects
      printTympanRemoteLayout();  //in resonse, the Tympan sends the definition of the GUI that we'd like
      break;   
    default:
      ret_val = SerialManagerBase::processCharacter(c);  //in here, it automatically loops over the different UI elements
      break; 
  }

  return ret_val;
}

// //////////////////////////////////  Methods for defining the GUI and transmitting it to the App

//define the GUI for the App
void SerialManager::createTympanRemoteLayout(void) {
  // Create some temporary variables
  TR_Page *page_h;  //dummy handle for a page
  TR_Card *card_h;  //dummy handle for a card

  //Add first page to GUI  (the indentation doesn't matter; it is only to help us see it better)
  page_h = myGUI.addPage("SD work");
      
      //Add a button group for SD recording...use a button set that is built into AudioSDWriter_F32_UI for you!
      card_h = audioSDWriter.addCard_sdRecord(page_h);

               
  //add some pre-defined pages to the GUI (pages that are built-into the App)
  //myGUI.addPredefinedPage("serialPlotter");  //if we send data in the right format, the App will plot the signal levels in real-time!
  myGUI.addPredefinedPage("serialMonitor");
}


// Print the layout for the Tympan Remote app, in a JSON-ish string
void SerialManager::printTympanRemoteLayout(void) {
    if (myGUI.get_nPages() < 1) createTympanRemoteLayout();  //create the GUI, if it hasn't already been created
    String s = myGUI.asString();
    Serial.println(s);
    ble->sendMessage(s); //ble is held by SerialManagerBase
    setFullGUIState();
}


// //////////////////////////////////  Methods for updating the display on the GUI

void SerialManager::setFullGUIState(bool activeButtonsOnly) {  //the "activeButtonsOnly" isn't used here, so don't worry about it

  //Then, let's have the system automatically update all of the individual UI elements that we attached
  //to the serialManager via the setupSerialManager() function used back in the main *.ino file.
  SerialManagerBase::setFullGUIState(activeButtonsOnly); //in here, it automatically loops over the different UI elements
}



#endif
