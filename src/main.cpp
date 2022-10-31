
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
// Uncomment this, the include above and in pcmConfig.h if using SdFat library
SdFat SD;

//#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
TMRpcm audio;

// Settings
#define SD_ChipSelectPin 10
const String VERSION = "HOCHZEITSANRUF V0.1.0 ALPHA";
const int FILENAME_DIGITS = 4;
const char FILE_PREFIXNAME[7] = "answer-";
const char EXTENSION[4] = ".wav";
char GREETING_FILE[20] = "greeting.wav";
const int SPEED_DIAL_SENSOR = 2;
const int PHONE_SENSOR = 3;
const int RECORD_LED = 4;
const int SPEAKER_PIN = 9;
const int MIC_PIN = A0;
const bool INVERT_PHONE_SENSOR = true;
int file_number = -1;
char versionString[FILENAME_DIGITS] = "0000";
char currentFilename[50] = "";
const int sample_rate = 12000;
int currentFile = 0;
int abortedRecording = 0;
int isRecording = 0;
int willPlayLastMessage = 0;
int lastMessageTaken = 0;
int blinkState = 1;
void updateFileNumber()
{
  // New Number
  // Let's start a record (new file, etc...)

  memcpy(currentFilename, FILE_PREFIXNAME, sizeof(FILE_PREFIXNAME));
  // Building the version string now.
  for (int i = 0; i < FILENAME_DIGITS; i++)
  {
    int inverse = FILENAME_DIGITS - i - 1;
    int divisor = round(pow(10, inverse));
    if (i == 0)
    {
      versionString[i] = '0' + (file_number / divisor);
    }
    else
    {
      versionString[i] = '0' + ((file_number / divisor) % 10);
    }
  }

  memcpy(currentFilename + sizeof(FILE_PREFIXNAME), versionString, FILENAME_DIGITS);

  memcpy(currentFilename + sizeof(FILE_PREFIXNAME) + FILENAME_DIGITS, ".wav", 4);
}

void userAcceptsPhoneCallback()
{
  isRecording = 1;
  // Play audio
  Serial.println("HANDLER: User took the phone");
  if (willPlayLastMessage == 1)
  {
    lastMessageTaken = 1;
    Serial.println("Play last message now, because it was requested");
    audio.play(currentFilename);
    return;
  }
  Serial.print("Playing greeting file: ");
  Serial.println(GREETING_FILE);
  audio.play(GREETING_FILE);
  abortedRecording = 0;
  while (audio.isPlaying())
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  if (abortedRecording == 1)
  {
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
  audio.startRecording(currentFilename, sample_rate, MIC_PIN);
}

void userHangsUpPhoneCallback()
{
  Serial.println("----------------\n\n");
  isRecording = 0;
  Serial.println("HANDLER: User hung up");
  if (willPlayLastMessage == 1)
  {
    lastMessageTaken = 0;
    Serial.println("Was playing last message. Stopping.");
    willPlayLastMessage = 0;
    if (audio.isPlaying())
    {
      audio.stopPlayback();
    }
    return;
  }
  if (audio.isPlaying())
  {
    audio.stopPlayback();
    abortedRecording = 1;
    return;
  }
  Serial.print("# STOP REC FILE ");
  Serial.println(currentFilename);
  audio.stopRecording(currentFilename);
}

void phoneChangeCallback()
{
  int value = digitalRead(PHONE_SENSOR);
  if (INVERT_PHONE_SENSOR)
  {
    if (value == HIGH)
    {
      value = LOW;
    }
    else if (value == LOW)
    {
      value = HIGH;
    }
  }
  if ((value == HIGH) && (isRecording == 0))
  {
    userAcceptsPhoneCallback();
  }
  else if ((value == LOW) && (isRecording == 1))
  {
    userHangsUpPhoneCallback();
  }
}

void speedDialCallback()
{
  Serial.println("Speedial callback is called");
  willPlayLastMessage = 1;
}

void wait_min(int mins)
{
  int count = 0;
  int secs = mins * 60;
  while (1)
  {
    Serial.print('.');
    delay(1000);
    count++;
    if (count == secs)
    {
      count = 0;
      break;
    }
  }
  Serial.println();
  return;
}

void displayDirectoryContent(File &aDirectory, byte tabulation)
{
  File file;
  char fileName[20];

  if (!aDirectory.isDir())
    return;
  aDirectory.rewind();

  while (file.openNext(&aDirectory, O_READ))
  {
    if (!file.isHidden())
    {
      file.getName(fileName, sizeof(fileName));
      for (uint8_t i = 0; i < tabulation; i++)
        Serial.write('\t');
      Serial.println(fileName);
      bool matchesString = true;
      for (int i = 0; i < sizeof(fileName); i++)
      {
        if (i < sizeof(FILE_PREFIXNAME))
        {
          if (i < sizeof(FILE_PREFIXNAME) && fileName[i] != FILE_PREFIXNAME[i])
          {
            matchesString = false;
          }
        }
      }
      // Get Version
      int number = 0;
      if (matchesString)
      {
        for (int i = sizeof(FILE_PREFIXNAME); i < sizeof(FILE_PREFIXNAME) + FILENAME_DIGITS; i++)
        {
          int factorial = FILENAME_DIGITS - (i - sizeof(FILE_PREFIXNAME) + 1);
          int factor = fileName[i] - '0';
          int add = factor * round(pow(10, factorial));
          number += add;
        }
      }

      if (number > file_number)
      {
        file_number = number;
      }

      if (file.isDir())
      {
        Serial.println(F("/"));
        displayDirectoryContent(file, tabulation + 1);
      }
      else
      {
        Serial.write('\t');
        Serial.print(file.fileSize());
        Serial.println(F(" bytes"));
      }
    }
    file.close();
  }
  Serial.println("CURRENT NUMBER: ");
  Serial.println(file_number);
}

void setup()
{
  // Short blink message (3 times on, 3 times off)
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(LED_BUILTIN, i % 2 == 0 ? HIGH : LOW);
    delay(100);
  }
  audio.speakerPin = SPEAKER_PIN; // 5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  audio.quality(1);
  pinMode(12, OUTPUT); // Pin pairs: 9,10 Mega: 5-2,6-7,11-12,46-45
  pinMode(RECORD_LED, OUTPUT);

  Serial.begin(9600);

  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD FAILED");
    Serial.println("Please insert an SD and try again");
    return;
  }
  else
  {
    Serial.println("SD OK");
  }
  Serial.println(("%s has started.", VERSION));
  // The audio library needs to know which CS pin to use for recording
  audio.CSPin = SD_ChipSelectPin;
  // Now print files
  File dir;
  if (!dir.open("/"))
  {
    Serial.println("SD Card might be corrupted.");
  }
  displayDirectoryContent(dir, 0);
  updateFileNumber();
  // Connect all events...
  pinMode(SPEED_DIAL_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_DIAL_SENSOR), speedDialCallback, FALLING);
  pinMode(PHONE_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PHONE_SENSOR), phoneChangeCallback, CHANGE);
  // TODO: Maybe make an own record file?
}

void loop()
{
  if (willPlayLastMessage == 1)
  {
    int delay = 1000;
    if (lastMessageTaken == 1)
    {
      delay = 500;
      if (!audio.isPlaying())
      {
        digitalWrite(RECORD_LED, LOW);
        return;
      }
    }
    if (millis() % delay > delay / 2)
    {
      digitalWrite(RECORD_LED, HIGH);
    }
    else
    {
      digitalWrite(RECORD_LED, LOW);
    }
  }
  else if (isRecording == 1)
  {
    digitalWrite(RECORD_LED, HIGH);
  }
  else
  {
    digitalWrite(RECORD_LED, LOW);
  }
}