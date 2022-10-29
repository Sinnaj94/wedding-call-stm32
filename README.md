# Wedding Phone
The Wedding Phone is a device that plays an audio greeting as soon as a visitor takes the "phone call".
After that, the user can record an audio that the marriage couple can enjoy lateron.
## Install
1. Install platform.io.
2. Install libraries using `pio lib install`
3. Under [./.pio/libdeps/nanoatmega328/PCM/pcmConfig.h](./.pio/libdeps/nanoatmega328/PCM/pcmConfig.h), please comment out line `#define ENABLE_RECORDING`
4. Under [./.pio/libdeps/nanoatmega328/PCM/pcmConfig.h](./.pio/libdeps/nanoatmega328/PCM/pcmConfig.h), please comment out line `#define SD_FAT`
5. Get your wiring done (Microphone, SD Card Reader, Buttons, Speaker)
6. Run via `pio run` (maybe give additional settings)
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
## Connection
### SD-CARD
Connect the SD Card reader to the following ports:
```
CS      D10
MOSI    D11
MISO    D12
SCK     D13
```
### SOUND
```
LED     D4
BUTTON  D3
MIC     A0
SPEAKER D9
```
## Audio Format
You have to convert the greeting to an arduino compatible audio file. These are the settings:
1. Change Sampling Rate – 16000Hz or (16KHz)
2. Change audio Channel _ MONO
3. Change bit resolution - 8 Bit
4. Show advanced options - PCM Format- PCM Unsigned 8-bit
You can convert it on [https://audio.online-convert.com/convert-to-wav](https://audio.online-convert.com/convert-to-wav)
## Copyright
All rights reserved, Starklicht UG, Wolfenbüttel.
