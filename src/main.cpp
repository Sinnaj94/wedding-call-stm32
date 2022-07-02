#include <Arduino.h>
#include <TMRpcm.h>
#include <OneButton.h>
TMRpcm audio;
int fileNumber = 0;
char defaultFile[20] = "welcome.wav";
String filePrefixname = "antwort";
int index = 0;
String currentFileName = "";
String extension = ".wav";
const int recordLed = 2;
const int sample_rate = 16000;
const int mic_pin = A0;
#define SD_CSPin 10
const int PHONE_BUTTON = A1;
#define SPEAKER_PIN A3
bool isRecording = false;
int getCurrentIndex() {
  return -1;
}

char* toCharArray(String inputString) {
    // Length (with one extra character for the null terminator)
  int str_len = inputString.length() + 1; 
  // Prepare the character Array (the buffer) 
  char char_array[str_len];
  inputString.toCharArray(char_array, str_len);
  return char_array;
}

void startRecord() {
  fileNumber = getCurrentIndex() + 1;
  // TODO: Change index based on max file number (using regex)
  currentFileName = ("%s%d%s", filePrefixname, fileNumber, extension);
  // Start recording audio
  audio.startRecording(toCharArray(currentFileName), sample_rate, mic_pin);
  isRecording = true;
  Serial.println("RECORD STARTING FOR FILE " + currentFileName);
}


void play() {
  audio.play(defaultFile, 0);
}

void endRecord() {
  audio.stopRecording(toCharArray(currentFileName));
  Serial.println("RECORD FOR FILE " + currentFileName + " FINISHED");
  isRecording = false;
}

void setup() {
  Serial.begin(9600);
  // Set pin Modes
  pinMode(mic_pin, INPUT);
  pinMode(recordLed, OUTPUT);
  // Connect to sd card
  Serial.println("loading... SD card");
  if (!SD.begin(SD_CSPin)) {
    Serial.println("An Error has occurred while mounting SD");
  }
  // Wait to connect to SD Card...
  while (!SD.begin(SD_CSPin)) {
    Serial.print(".");
    delay(500);
  }
  audio.CSPin = SD_CSPin;
}



void loop() {
  // We should test this logic at STARKLICHT HQ tomorrow
  // Tick button 
  // Step 1: Wait, until long press has started (user takes phone)
  if(analogRead(PHONE_BUTTON) == HIGH) {
      // Step 2: First, play greeting
      audio.play(defaultFile);
      delay(200);
      // Step 3: After this, record new file
      startRecord();
      delay(200);
  } else if(isRecording) {
    endRecord();
    delay(200);
  }
}
