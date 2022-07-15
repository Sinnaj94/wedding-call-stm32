# Wedding Phone
The Wedding Phone is a device that plays an audio greeting as soon as a visitor takes the "phone call".
After that, the user can record an audio that the marriage couple can enjoy lateron.
## Install
1. Install platform.io.
2. Install libraries using `pio lib install`
3. Get your wiring done (Microphone, SD Card Reader, Buttons, Speaker)
4. Run via `pio run` (maybe give additional settings)

## Settings
These are the settings (top of main.cpp):
```
// Settings
#define SD_ChipSelectPin 10 // SD PIN "CS"
const String VERSION = "HOCHZEITSANRUF V0.1.0 ALPHA"; -> 
const int FILENAME_DIGITS = 4; // How many digits will there be in filenames (file 1 -> 0001)
const char FILE_PREFIXNAME[7] = "answer-"; // prefix for answers
const char EXTENSION[4] = ".wav"; // default extension
char GREETING_FILE[20] = "greeting.wav"; // default greeting file
const int RECORD_BUTTON = 2; // record button
const int PHONE_SENSOR = 3; // phone sensor (when user lifts phone)
const int RECORD_LED = 4; // record led (goes on when recording)
const int SPEAKER_PIN = 9; // speaker pin 
const int MIC_PIN = A0; // microphone pin
``` 

## Copyright
All rights reserved, Starklicht UG, Wolfenbüttel.