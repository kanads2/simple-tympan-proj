#include <Tympan_Library.h>
#include "NLMS_F32.h"
#include "SerialManager.h"

//set the sample rate and block size
const float sample_rate_Hz = 24000.0f ; //24000 or 44117 (or other frequencies in the table in AudioOutputI2S_F32)
const int audio_block_samples = 128;     //do not make bigger than AUDIO_BLOCK_SAMPLES from AudioStream.h (which is 128)
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

// Create the audio library objects that we'll use
Tympan                    myTympan(TympanRev::E, audio_settings);   //do TympanRev::D or E or F
AudioInputI2S_F32           i2s_in(audio_settings);                  //Digital audio *from* the Tympan AIC
AudioSynthNoiseWhite_F32  noise(audio_settings);   //from the Tympan_Library
AudioOutputI2S_F32        audioOutput(audio_settings);//from the Tympan_Library
NLMS_F32                 NLMS(audio_settings); 






 
AudioConnection_F32 patchCord2(noise, 0, NLMS, 0); 

AudioConnection_F32 patchCord1(noise, 0, audioOutput, 0);
//AudioConnection_F32 patchCord0(noise, 0, audioOutput, 1);

AudioConnection_F32 patchCord10(i2s_in, 0, NLMS, 1); 

SerialManager serialManager;     //create the serial manager for real-time control (via USB or App)



bool enable_printCPUandMemory = true;
void setup() {
  // put your setup code here, to run once:
  //Open serial link for debugging
  myTympan.beginBothSerial(); delay(1000); //both Serial (USB) and Serial1 (BT) are started here
  myTympan.println("OutputTone: starting setup()...");

  //start the audio hardware
  AudioMemory_F32(20,audio_settings); //I can only seem to allocate 400 blocks
  
  myTympan.enable();

  //myTympan.inputSelect(TYMPAN_INPUT_ON_BOARD_MIC);     // use the on board microphones
  myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_MIC);
  myTympan.setInputGain_dB(10.0f);
  //allocate the audio memory
  

  //set the sine wave parameters
  noise.amplitude(0.0);
  myTympan.setAmberLED(LOW);
  
  //Set the baseline volume levels
  myTympan.volume_dB(0);                   // headphone amplifier.  -63.6 to +24 dB in 0.5dB steps.
  
  //prepare the SD writer for the format that we want and any error statements

  Serial.println("Setup complete.");

  
  serialManager.printHelp();
  //myTympan.printCPUandMemory(millis(),0);

}


void loop() {
  //handle any in-coming serial commands
  while (Serial.available()) serialManager.respondToByte((char)Serial.read());   //USB
  
  noise.amplitude(0.3);

  if (enable_printCPUandMemory) myTympan.printCPUandMemory(millis(),3000); //print every 3000 msec

  //Blink the LEDs!
  myTympan.serviceLEDs(millis());   //defaults to a slow toggle (see Tympan.h and Tympan.cpp)
  

}
