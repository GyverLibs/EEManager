This is an automatic translation, may be incorrect in some places. See sources and examples!

# Eemanager
Eeprom manager - Library to reduce memory wear
- postponed entry (update) by timer
- works on the basis of standard eeprom.h
- Built -in mechanism of the first record key

## compatibility
Compatible with all arduino platforms (used arduino functions)
- For ESP8266, do not forget to call EEPROM.BEGIN (size)!

## Content
- [installation] (# Install)
- [initialization] (#init)
- [use] (#usage)
- [Example] (# Example)
- [versions] (#varsions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** eemanager ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/eemanager/archive/refs/heads/main.zip) .Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!


<a id="init"> </a>
## initialization
`` `CPP
// We transfer our variable of any type (in fact its address) - structure, array, anything
Eemanager Memory (Data);

// You can set the update timout, by the silence.5 seconds (5000ms)
Eemanager Memory (Data, 1000);
`` `

<a id="usage"> </a>
## Documentation
`` `CPP
// Start work, read data in a variable.
// accepts the address of the start of storage of the date and key
// returns:
// 0 - the key coincided, the data is read from EPROM
// 1 - the key did not coincide (the first launch), the data are recorded in EPROM
// 2 - error, there is not enough space in Eprom
Uint8_t Begin (Uint16_T Addr, Uint8_t Key);

VOID settimeout (uint16_t tout = 5000);// change the timaut
VOID updatenow ();// update data to her industry now
VOID update ();// postpone the update and reset the timer
Bool Tick ();// Tiker of updates
VOID Reset ();// Reset the launch key.When rebooting (or calls is the Begin), standard data will be recorded

uint16_t datasize ();// get data size
Uint16_T BLOCKSIZE ();// Get the size of the entire block (data + key)

uint16_t keyaddr ();// get the key address (coincides with the transferred to Begin ())
uint16_t Startaddr ();// get the address of the first byte of data in the block (equal to address in Begin () + 1)
Uint16_T Endaddr ();// get the address of the last byte in the block
uint16_t nEXTADDDR ();// Get the first free address for the next block
`` `

General functions/macros
`` `CPP
EEBLOCK (DATA);// returns the size of the specified data Data (any type) +1 to the key (for calculating the addresses of the blocks in memory)
`` `

### Usage
1. declare a variable in which data is stored (integer, array, structure ...)
2. Pass it to Eemanager: `Eemanager Memory (Data);`
2.1 * [only for ESP] * Launch EEPROM indicating the size: `eeprom.begin (size);`.If only the specified data is stored, you can do it: `eeprom.begin (memory.blocksize ());`
3. Run the manager `Memory.Begin (address, key);`
- Address - starting from this address, the block will be stored in memory (first the key, then data)
- Key - the "first entry" tag type `byte` (you can use the symbol).If you change the key, the data will be rewritten from the variable, i.e.They will drop to default
4. Call `memory.tick ()` inside `loop ()`
5 and
6. The “first record” key is stored at the very beginning of the block (at the address indicated in the address), therefore, when the amount of the recorded data changes, it will not suffer

The library allows you to postpone the data recording on a given timout, so when setting up the devices settings (buttons/twists), you can safely call update () after each change, since the data will be recorded after the timut after the change is stopped.

<a id="EXAMPLE"> </a>
## Example
`` `CPP
// Data storage structure
Struct Data {
  Char Val = 'H';
  Chard [15] = "Ello Kitty!";
};
Data Data;// variable with which we work in the program

#include <eemanager.h> // Connect Liba
Eemanager Memory (Data);// We transmit our variable (in fact its address)

// You can set the update timout, by the silence.5 seconds (5000ms)
// Eemanager Memory (Data, 1000);

VOID setup () {
  Serial.Begin (9600);
  // For ESP8266 Do not forget to call eEPROM.BEGIN (size)!
  /*
    We launch the manager, transmit:
    - starting address in memory for recording date
    - storage key (0 .. 255) or symbol
  */
  byte stat = memory.begin (0, 'b');

  /*
    Return codes:
    0 - the key coincided, the data is read from EPROM
    1 - the key did not coincide (the first launch), the data is recorded in EPROM
    2 - error, in Eprom there is not enough space
  */
  Serial.println (stat);

  // Determine the entire contents of the memory unit, including the key
  for (uint16_t I = Memory.startaddr (); i <memory.ndadddr ()+1; i ++)
    Serial.write (eeprom.read (i));
  Serial.println ();
  
  Data.val = 'X';// Change data in the variable
  Memory.update ();// postpone the update

  // Note: if you need to create another block next in memory -
  // Launch it at the address Memory.ndadddr () + 1, this is the first
  // Free byte after the first block
}

VOID loop () {
  // In a lup we call tick (), in it the timer will update
  // The function will return True when it happens
  if (memory.tick ()) serial.println ("updated!");

  // Determine the entire contents of the memory unit, including the key
  for (uint16_t I = Memory.startaddr (); i <memory.ndadddr ()+1; i ++)
    Serial.write (eeprom.read (i));
  Serial.println ();

  // Timer of reset for 10 seconds: after 10 seconds
  // Data key will be discharged, and at the next launch
  // In the EPROM, the default data will be recorded (what is in the structure of the structure)
  Static uint32_t tmr;
  if (millis () - tmr> = 10000) {
    Memory.Reset ();
    Serial.println ("reset");
    for (;;);
  }
  
  DELAY (1000);// Chilim
}
`` `

<a id="versions"> </a>
## versions
- V1.0
- V1.1 - Code of Return Begin
- v1.2 - added Nextadddr ()
- V1.2.1 - ESP32 support
- v1.2.2 - fixed Varning
- V1.3 - A critical bug with addressing was fixed, EEBLOCK macro has been added
- V1.4 - even more support ESP8266/32
- V2.0 - the launch key is transferred to the beginning of the block for convenience.The library update will drop data in Eeprom!

<a id="feedback"> </a>
## bugs and feedback
When the bugs are created** Issue **, or better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!


When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code