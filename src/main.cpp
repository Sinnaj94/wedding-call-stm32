
/*
This sketch demonstrates recording of standard WAV files that can be played on any device that supports WAVs. The recording
uses a single ended input from any of the analog input pins. Uses AVCC (5V) reference currently.
Requirements:
Class 4 or 6 SD Card
Audio Input Device (Microphone, etc)
Arduino Uno,Nano, Mega, etc.
Steps:
1. Edit pcmConfig.h
    a: On Uno or non-mega boards, #define buffSize 128. May need to increase.
    b: Uncomment #define ENABLE_RECORDING and #define BLOCK_COUNT 10000UL
2. Usage is as below. See https://github.com/TMRh20/TMRpcm/wiki/Advanced-Features#wiki-recording-audio for
   additional informaiton.
Notes: Recording will not work in Multi Mode.
Performance is very dependant on SD write speed, and memory used.
Better performance may be seen using the SdFat library. See included example for usage.
Running the Arduino from a battery or filtered power supply will reduce noise.
*/

#include <SdFat.h>
#include <SPI.h>
#include <TMRpcm.h>
//Uncomment this, the include above and in pcmConfig.h if using SdFat library
SdFat SD;

#define SD_ChipSelectPin 10  //example uses hardware SS pin 53 on Mega2560
//#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
const String VERSION = "HOCHZEITSANRUF V0.1.0 ALPHA";
TMRpcm audio;
int file_number = 0;
char filePrefixname[50] = "hochzeit";
char exten[10] = ".wav";
const int recordLed = 2;
const int mic_pin = A0;
const int sample_rate = 16000;

void wait_min(int mins) {
  int count = 0;
  int secs = mins * 60;
  while (1) {
    Serial.print('.');
    delay(1000);
    count++;
    if (count == secs) {
      count = 0;
      break;
    }
  }
  Serial.println();
  return ;
}


void setup() {
  
  audio.speakerPin = 11; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  pinMode(12,OUTPUT);  //Pin pairs: 9,10 Mega: 5-2,6-7,11-12,46-45
  
  Serial.begin(9600);
  
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD FAILED");
    return;
  }else{
    Serial.println("SD OK"); 
  }
  Serial.println(("%s has started.", VERSION));
  // The audio library needs to know which CS pin to use for recording
  audio.CSPin = SD_ChipSelectPin;
}


void loop() {
    Serial.println("####################################################################################");
    char fileSlNum[20] = "";
    itoa(file_number, fileSlNum, 10);
    char file_name[50] = "";
    strcat(file_name, filePrefixname);
    strcat(file_name, fileSlNum);
    strcat(file_name, exten);
    Serial.print("New File Name: ");
    Serial.println(file_name);
    digitalWrite(recordLed, HIGH);
    audio.startRecording(file_name, sample_rate, mic_pin);
    Serial.println("startRecording ");
    // record audio for 2mins. means , in this loop process record 2mins of audio.
    // if you need more time duration recording audio then
    // pass higher value into the wait_min(int mins) function.
    delay(10000);
    digitalWrite(recordLed, LOW);
    audio.stopRecording(file_name);
    Serial.println("stopRecording");
    file_number++;
    Serial.println("####################################################################################");
}