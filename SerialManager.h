#ifndef _SerialManager_h
#define _SerialManager_h

#include <Tympan_Library.h>
extern Tympan myTympan;               
extern NLMS_F32 NLMS;


extern bool enable_printCPUandMemory;

class SerialManager : public SerialManagerBase  {  // see Tympan_Library SerialManagerBase for more functions!
  public:
    void printHelp(void);
    bool processCharacter(char c);  //this is called by SerialManagerBase.respondToByte(char c)
};


void SerialManager::printHelp(void) {  
  Serial.println();
  Serial.println("SerialManager Help: Available Commands:");
  Serial.println("   h: Print this help");
  Serial.println("   p: Print the RTF coef");
  Serial.println("   e: print error");
  Serial.println("   c: Disable printing of CPU memory");
  Serial.println("   C: Enable printing of CPU memory");
  Serial.println();
}


//switch yard to determine the desired action
bool SerialManager::processCharacter(char c) {
  bool ret_val = true;
  switch (c) {
    case 'h': case '?':
      printHelp(); break;
    case 'p': case 'P':
      Serial.println();
      NLMS.printFilter();
      Serial.println();
      break;
    case 'e':
        Serial.println();
      NLMS.printError();
      Serial.println();
      break;
    case 'c': 
        enable_printCPUandMemory = false;
        break;
    case 'C':
        enable_printCPUandMemory = true;
        break;
    default:
      ret_val = SerialManagerBase::processCharacter(c);  //in here, it automatically loops over the different UI elements
      break; 
  }

  return ret_val;
}
#endif
