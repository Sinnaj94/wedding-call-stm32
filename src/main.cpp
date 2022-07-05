#include <Arduino.h>
#include <TMRpcm.h>
#include <OneButton.h>
String VERSION = "V0.1.0 ALPHA";
TMRpcm audio;
int fileNumber = 0;
String filePrefixname = "antwort";
int index = 0;
String currentFileName = "";
String extension = ".wav";
const int recordLed = 2;
const int sample_rate = 16000;
const int mic_pin = A0;
#define SD_CSPin 5
// const int PHONE_BUTTON = A1;
const int SPEAKER_PIN = 9;
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
  audio.play("welcome.wav", 0);
}

void endRecord() {
  audio.stopRecording(toCharArray(currentFileName));
  Serial.println("RECORD FOR FILE " + currentFileName + " FINISHED");
  isRecording = false;
}


void phoneBeep() {
  // vol.tone(SPEAKER_PIN ,425, 40);
  audio.stopPlayback();
  audio.play("rufzeichen.wav");
  delay(1000);
  audio.stopPlayback();
}

void sdCardConnectFeedback() {
  analogWrite(LED_BUILTIN, HIGH);
  delay(100);
  analogWrite(LED_BUILTIN, LOW);
  delay(100);
}


void sdCardConnectedFeedback() {
  for(int i = 0; i < 3; i++) {
    analogWrite(LED_BUILTIN, HIGH);
    delay(500);
    analogWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Hochzeitsanruf " + VERSION + " has started.");
  pinMode(10, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  digitalWrite(10, HIGH); // Add this line
  // Set pin Modes
  // pinMode(mic_pin, INPUT);
  // pinMode(recordLed, OUTPUT);
  // Connect to sd card
  Serial.println("loading... SD card");
  if (!SD.begin(10)) {
    Serial.println("An Error has occurred while mounting SD");
    sdCardConnectFeedback();
  }
  // Wait to connect to SD Card...
  while (!SD.begin(10)) {
    Serial.print(".");
    delay(500);
    sdCardConnectFeedback();
  }
  sdCardConnectedFeedback();
  Serial.println("Successfully connected to SD Card!");
  audio.speakerPin = SPEAKER_PIN;
}



void loop() {
  tone(SPEAKER_PIN, 400);
  delay(1000);
  noTone(SPEAKER_PIN);

  Serial.println("RECORDING A FILE");
  audio.startRecording("glockenspiel.wav", 16000, A0);
  delay(4000);
  audio.stopRecording("glockenspiel.wav");
  while(true) {
    //
    sdCardConnectedFeedback();
  }
  /* phoneBeep();
  audio.stopPlayback();
  Serial.println("Playing a Sound! Test...");

  audio.play("welcome.wav");
  delay(1000);
  audio.stopPlayback();
  Serial.println("Sound has played now");
  delay(500);
  Serial.println("RECORDING A FILE");
  audio.startRecording("glockenspiel.wav", 16000, A0);

  delay(2000);
  audio.stopRecording("glockenspiel.wav");
  Serial.println("RECORD FINISHED!");
  delay(100);
  audio.play("glockenspiel.wav");
  delay(2000); */

  // We should test this logic at STARKLICHT HQ tomorrow
  // Tick button 
  // Step 1: Wait, until long press has started (user takes phone)
  /* if(analogRead(PHONE_BUTTON) == HIGH) {
      // Step 2: First, play greeting
      Serial.println("Playing a Sound! Test...");
      audio.play(defaultFile);
      delay(2000);
      Serial.println("Sound has played now for 2 Seconds.");
      // Step 3: After this, record new file
      //startRecord();
      //delay(200);
  } else if(isRecording) {
    endRecord();
    delay(200);
  } */
}
