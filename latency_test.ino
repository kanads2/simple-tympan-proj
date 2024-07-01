/*
SD input output latency test


*/




#include <Tympan_Library.h>

//set the sample rate and block size
const float sample_rate_Hz = 24000.0f ; //24000 or 44117 (or other frequencies in the table in AudioOutputI2S_F32)
const int audio_block_samples = 16;     //do not make bigger than AUDIO_BLOCK_SAMPLES from AudioStream.h (which is 128)
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);


// Create the audio library objects that we'll use
Tympan                    myTympan(TympanRev::E, audio_settings);   //do TympanRev::D or E or F
AudioSynthWaveform_F32    sineWave(audio_settings);   //from the Tympan_Library
AudioOutputI2S_F32        audioOutput(audio_settings);//from the Tympan_Library
AudioSDWriter_F32_UI         audioSDWriter(audio_settings); //this is stereo by default but can do 4 channels
AudioInputI2S_F32         i2s_in(audio_settings);        //Digital audio input from the ADC

// Create the audio connections from the sineWave object to the audio output object
AudioConnection_F32 patchCord10(sineWave, 0, audioOutput, 0);  //connect to left output

AudioConnection_F32 patchCord20(i2s_in, 0, audioSDWriter, 0);  //connect to left input to SD card
AudioConnection_F32 patchCord21(sineWave, 0, audioSDWriter, 1);  //connect to sine wave to SD card

#include "SerialManagerLatency.h"

BLE_UI&  ble = myTympan.getBLE_UI();  // reference to bluetooth object

SerialManager serialManager(&ble);


//set up the serial manager
void setupSerialManager(void) {
  //register all the UI elements here
  serialManager.add_UI_element(&ble);
  serialManager.add_UI_element(&audioSDWriter);
}


// Define the parameters of the tone
float tone_freq_Hz = 1000.0;   //frequency of the tone
float tone_amp = .01;     //here are the amplitudes (1.0 is the loudest before distorting...aka "full scale")
float tone_dur_msec = 1000.0;  // Length of time for the tone, milliseconds

// define setup()...this is run once when the hardware starts up
void setup(void)
{
  //Open serial link for debugging
  myTympan.beginBothSerial(); delay(1000); //both Serial (USB) and Serial1 (BT) are started here
  Serial.println("Latency test: starting setup()");
  Serial.println("Sample Rate (Hz): " + String(audio_settings.sample_rate_Hz));
  Serial.println("Audio Block Size (samples): " + String(audio_settings.audio_block_samples));

  //allocate the audio memory
  AudioMemory_F32(20,audio_settings); //I can only seem to allocate 400 blocks
  
  //set the sine wave parameters
  sineWave.frequency(tone_freq_Hz);
  sineWave.amplitude(0.0);
  myTympan.setAmberLED(LOW);
  
  //start the audio hardware
  myTympan.enable();

  //Set the baseline volume levels
  myTympan.volume_dB(0);                   // headphone amplifier.  -63.6 to +24 dB in 0.5dB steps.

  //setup BLE
  delay(500); myTympan.setupBLE(); delay(500); //Assumes the default Bluetooth firmware. You can override!
  
  myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_MIC);
  myTympan.setInputGain_dB(15.0f);


  //setup the serial manager
  setupSerialManager();

  //prepare the SD writer for the format that we want and any error statements
  audioSDWriter.setSerial(&myTympan);         //the library will print any error info to this serial stream (note that myTympan is also a serial stream)
  audioSDWriter.setNumWriteChannels(2);       //this is also the built-in defaullt, but you could change it to 4 (maybe?), if you wanted 4 channels.
  Serial.println("Setup: SD configured for " + String(audioSDWriter.getNumWriteChannels()) + " channels.");
  
  Serial.println(" This test is no filtering, pure input/output");
  
}

// define loop()...this is run over-and-over while the device is powered
#define START_SILENCE 0
#define PLAY_SILENCE 1
#define START_TONE 2
#define PLAY_TONE 3
#define END 4
int state = START_SILENCE;
unsigned long start_time_millis = 0;
float silence_dur_msec = 1000.f;
int iteration_count = 0;
void loop(void)
{   
    //service the SD recording
   while (Serial.available()) serialManager.respondToByte((char)Serial.read());   //USB
   //respond to BLE
  if (ble.available() > 0) {
    String msgFromBle; int msgLen = ble.recvBLE(&msgFromBle);
    for (int i=0; i < msgLen; i++) serialManager.respondToByte(msgFromBle[i]);
  }

  //service the BLE advertising state
  ble.updateAdvertising(millis(),5000); //check every 5000 msec to ensure it is advertising (if not connected)
  audioSDWriter.serviceSD_withWarnings(i2s_in); //For the warnings, it asks the i2s_in class for some info
  myTympan.serviceLEDs(millis(),audioSDWriter.getState() == AudioSDWriter::STATE::RECORDING); 


  switch (state) {
    case START_SILENCE:
      sineWave.amplitude(0.0f);
      start_time_millis = millis();
      myTympan.setAmberLED(LOW);
      state = PLAY_SILENCE;
      break;
    case PLAY_SILENCE:
      if ((millis() - start_time_millis) >= silence_dur_msec) {
        state = START_TONE;
      }
      break;
    case START_TONE:
      iteration_count++;
      sineWave.amplitude(tone_amp);
      myTympan.setAmberLED(HIGH);
      start_time_millis = millis();
      state = PLAY_TONE;
      break;
    case PLAY_TONE:
      if ((millis() - start_time_millis) >= tone_dur_msec) {
        state = START_SILENCE;
      }
      break;     
    case END:
        Serial.println("Done recording");
        sineWave.amplitude(0.0f);
        myTympan.setAmberLED(LOW);
        break;
  }
}
