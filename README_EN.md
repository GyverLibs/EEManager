This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/EEManager.svg?color=brightgreen)](https://github.com/GyverLibs/EEManager/releases/latest/download/EEManager.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/EEManager.svg)](https://registry.platformio.org/libraries/gyverlibs/EEManager)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/EEManager?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# EEManager
EEPROM Manager - a library to reduce memory wear
- Postponed recording (update) by timer
- It works on the basis of standard EEPROM. h h
- Built-in mechanism of the first record key

> For esp8266 and esp32 it is much better to store data in the file system, use the library.[FileData](https://github.com/GyverLibs/FileData)(binary data in the file) or[Pairs](https://github.com/GyverLibs/Pairs)(human-readable files)

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)
- For esp8266, remember to call EEPROM.begin(size)!

## Contents
- [Initialization](#init)
- [Use of use](#usage)
- [Example](#example)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="init"></a>

## Initialization
```cpp
EEManager(T& data, uint16_t tout = 5000);
EEManager(void* data, uint16_t size, uint16_t tout = 5000);
```

<a id="usage"></a>

## Documentation.
```cpp
// Start working, read the data into a variable.
// Accepts date start address and key
// Returns:
// 0 - the key matched, the data read from Eprom
// 1 - the key did not match (first run), the data is recorded in eprom
// 2 - a mistake, there is not enough space in the eprom
uint8_t begin(uint16_t addr, uint8_t key);

void setTimeout(uint16_t tout = 5000);      // timeout
void updateNow();       // Update the data in her industry now
void update();          // postpone the update and reset the timer
void stop();            // cancel the delayed update
bool tick();            // ticker
void reset();           // reset the launch key. When restarting (or calling start), standard data will be written.

uint16_t dataSize();    // size
uint16_t blockSize();   // Get the size of the entire block (data + key)

uint16_t keyAddr();     // receive the address of the key (coincides with the transferred to start())
uint16_t startAddr();   // get the address of the first data byte in the block (equal to the address in start() + 1)
uint16_t endAddr();     // Get the address of the last byte in the block
uint16_t nextAddr();    // Get the first free address for the next block
```

General functions/macros
```cpp
EEBlock(data);          // returns the size of the specified data (any type) +1 to the key (to calculate the addresses of blocks in memory)
```

### Use of use
1. Declare the variable in which the data is stored (integer, array, structure...)
2. Send it to EEManager:`EEManager memory(data);`  
2.1 *[For ESP only]* Start EEPROM with the size:`EEPROM.begin(размер);`. If only these data will be stored, you can do this:`EEPROM.begin(memory.blockSize());`
3. Run the manager.`memory.begin(адрес, ключ);`
- Address – starting from this address, the block will be stored in memory (key first, then data)
- The key is a "first entry" type label.`byte`(Symbols may be used). If you change the key, the data will be overwritten from the variable, i.e. reset to silence.
4. Call in`memory.tick()`inside`loop()`
5. After changing the data, call`memory.update()`EEPROM data will be updated automatically after a timeout within the function`tick()`
6. The key of the "first record" is stored at the very beginning of the block (at the address specified in the start() address), so if you change the size of the recorded data will not suffer.

The library allows you to postpone the recording of data for a given timeout, so when setting the device parameters (buttons / twisters), you can safely call update() after each change, since the data will be written after a timeout after the changes stop.

<a id="example"></a>

## Example
```cpp
// storage
struct Data {
  char val = 'H';
  char str[15] = "ello kitty!";
};
Data data;  // The variable we work with in the program

#include <EEManager.h>  // plug-in
EEManager memory(data); // Transmit our variable (actually its address)

// You can set a timeout update, silently. 5 seconds (5000ms)
//EEManager memory(data, 1000);

void setup() {
  Serial.begin(9600);
  // For esp8266, remember to call EEPROM.begin(size)!
  /*
    Запускаем менеджер, передаём:
    - Стартовый адрес в памяти для записи даты
    - Ключ хранения (0.. 255) или символ
  */
  byte stat = memory.begin(0, 'b');

  /*
    Коды возврата:
    0 - ключ совпал, данные прочитаны из епром
    1 - ключ не совпал (первый запуск), данные записаны в епром
    2 - ошибка, в епроме не хватает места
  */
  Serial.println(stat);

  // port all the contents of the memory block, including the key
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();
  
  data.val = 'X';     // change the data in the variable
  memory.update();    // postpone the update

  // Note: if you want to create another block next in memory,
  // Run it at memory.endAddr() + 1, this is the first
  // free byte after the first block
}

void loop() {
  // in the magnifier call tick(), it will update the timer
  // The function will return true when it happens.
  if (memory.tick()) Serial.println("Updated!");

  // port all the contents of the memory block, including the key
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();

  // Reset timer for 10 seconds: after 10 seconds
  // The data key will be reset, and at the next launch
  // In Eprom, the data will be recorded by default (what is in the structure shortcut)
  static uint32_t tmr;
  if (millis() - tmr >= 10000) {
    memory.reset();
    Serial.println("reset");
    for (;;);
  }
  
  delay(1000);    // chilim
}
```

<a id="versions"></a>

## Versions
- v1.0
- v1.1 - Changed start return codes
- v1.2 - added nextAddr()
- v1.2.1. support for esp32
- v1.2.2 - Fixed warning
- v1.3 - Fixed critical addressing bug, added macro EEBlock
- v1.4 - more support for esp8266/32
- v2.0 - The start key is moved to the beginning of the block for convenience. The library update will reset data to EEPROM!

<a id="install"></a>

## Installation
- The library can be found under the name **EEManager** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/EEManager/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
