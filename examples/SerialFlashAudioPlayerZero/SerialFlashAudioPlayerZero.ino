/*
  Simple Audio Player for Arduino Zero

 Demonstrates the use of the Audio library for the Arduino Zero

 Hardware required :
 * Arduino shield with a SD card on CS4
 * A sound file named "test.wav" in the root directory of the SD card
 * An audio amplifier to connect to the DAC0 and ground
 * A speaker to connect to the audio amplifier

 
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc>

 This example code is in the public domain

 http://arduino.cc/en/Tutorial/SimpleAudioPlayerZero


 This sample demonstrates playing audio from a serial flash chip
 for the version of AudioZero that has been modified for the Teensy LC
*/

#include <AudioZero.h>

#define FLASH_CS_PIN    6
#define AMP_ENABLE_PIN  5

void setup()
{
  // debug output at 115200 baud
  Serial.begin(115200);
  while (!Serial) ;

  // setup serial flash chip
  Serial.print("Initializing serial flash... ");
  if (!SerialFlash.begin(FLASH_CS_PIN)) {
    Serial.println("failed!");
    while(true);
  }
  Serial.println("done.");

  // turn on the amplifier if you are using Prop Shield
  pinMode(AMP_ENABLE_PIN, OUTPUT);
  digitalWrite(AMP_ENABLE_PIN, HIGH);
  delay(10);
}

void loop()
{
  Serial.println("8-bit unsigned 44.1kHz Mono File");

  SerialFlashFile myFile = SerialFlash.open("8BU44K1M.WAV");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening file");
    while (true);
  }
  myFile.seek(44); // skip WAV header
  
  Serial.println("Playing");
 
  AudioZero.begin(44100);
  AudioZero.play(myFile);
  AudioZero.end();

  Serial.println("Done.");

  delay(1000);

  Serial.println("8-bit unsigned 44.1kHz Stereo File");

  myFile = SerialFlash.open("8BU44K1S.WAV");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening file");
    while (true);
  }
  myFile.seek(44); // skip WAV header
  
  Serial.println("Playing");

  // double the playback rate because a stereo file has twice as many samples
  AudioZero.begin(2*44100);
  AudioZero.play(myFile);
  AudioZero.end();
 
  Serial.println("Done.");

  while (true) ;
}
