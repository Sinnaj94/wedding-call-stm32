
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
int filenameDigits = 4;
int file_number = 0;
char filePrefixname[8] = "antwort-";
char exten[4] = ".wav";
char versionString[4] = "0000";
char currentFilename[50] = "";
char welcome[20] = "greeting.wav";
const int recordLed = 2;
const int mic_pin = A0;
const int sample_rate = 16000;
int currentFile = 0;
int abortedRecording = 0;

const int RECORD_BUTTON = 2;
const int PHONE_SENSOR = 3;
int isRecording = 0;

void updateFileNumber () {
  // New Number
  // Let's start a record (new file, etc...)
  
  memcpy(currentFilename, filePrefixname, sizeof(filePrefixname));
  versionString[0] = '0' + (file_number/1000);   // thousand digit
  versionString[1] = '0' + ((file_number/100)%10);  // hundreds digit
  versionString[2] = '0' + ((file_number/10)%10);   // tens digit
  versionString[3] = '0' + (file_number%10);        // ones digit
  memcpy(currentFilename + sizeof(filePrefixname), versionString, filenameDigits);
  
  memcpy(currentFilename + sizeof(filePrefixname) + filenameDigits, ".wav", 4);
}

void userAcceptsPhoneCallback() {
  Serial.println("----------------\n\n");
  isRecording = 1;
  // Play audio
  Serial.println("HANDLER: User took the phone");
  
  Serial.print("Playing greeting file: ");
  Serial.println(welcome);
  audio.play(welcome);
  abortedRecording = 0;
  while(audio.isPlaying()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  if(abortedRecording == 1) {
    Serial.println("Aborted before recording. No recording file. Stopping playback");
    return;
  }
  Serial.println("Finished playing greeting File!\n");
  // Play beep?
  delay(100);


  file_number++;
  updateFileNumber();
  Serial.print("# REC File ");
  Serial.println(currentFilename);

  audio.startRecording(currentFilename, sample_rate, mic_pin);
}

void userHangsUpPhoneCallback() {
  Serial.println("----------------\n\n");
  isRecording = 0;
  Serial.println("HANDLER: User hung up");
  if(audio.isPlaying()) {
    audio.stopPlayback();
    abortedRecording = 1;
    return;
  }
  Serial.print("# STOP REC FILE ");
  Serial.println(currentFilename);
  audio.stopRecording(currentFilename);
}

void phoneChangeCallback() {
  int value = digitalRead(PHONE_SENSOR);
  if((value == HIGH) && (isRecording == 0)) {
    userAcceptsPhoneCallback();
  } else if((value == LOW) && (isRecording == 1)) {
    userHangsUpPhoneCallback();
  }
}


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


void displayDirectoryContent(File& aDirectory, byte tabulation) {
  File file;
  char fileName[20];

  if (!aDirectory.isDir()) return;
  aDirectory.rewind();

  while (file.openNext(&aDirectory, O_READ)) {
    if (!file.isHidden()) {
      file.getName(fileName, sizeof(fileName));
      for (uint8_t i = 0; i < tabulation; i++) Serial.write('\t');
      Serial.println(fileName);
      bool matchesString = true;
      for(int i = 0; i < sizeof(fileName); i++) {
          if(i < sizeof(filePrefixname)) {
            if(i < sizeof(filePrefixname) && fileName[i] != filePrefixname[i]) {
              matchesString = false;
            }
          }
      }
      // Get Version
      int number = 0;
      if(matchesString) {
        for(int i = sizeof(filePrefixname); i < sizeof(filePrefixname) + filenameDigits; i++) {
          int factorial = filenameDigits - (i - sizeof(filePrefixname) + 1);
          int factor = fileName[i] - '0';
          int add = factor * round(pow(10, factorial));
          number += add;
        }
      }

      if(number > file_number) {
        file_number = number;
      }

      if (file.isDir()) {
        Serial.println(F("/"));
        displayDirectoryContent(file, tabulation + 1);
      } else {
        Serial.write('\t'); Serial.print(file.fileSize()); Serial.println(F(" bytes"));
      }
    }
    file.close();
  }
  Serial.println("CURRENT NUMBER: ");
  Serial.println(file_number);
}

void recordNextFile() {
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
  // Now print files
  File dir;
  if(!dir.open("/")) {
    Serial.println("SD Card might be corrupted.");
  }
  displayDirectoryContent(dir, 0);
  // Connect all events...
  pinMode(PHONE_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PHONE_SENSOR), phoneChangeCallback, CHANGE);
  // TODO: Maybe make an own record file?
}


void loop() {
  // Empty, because we use handlers
}