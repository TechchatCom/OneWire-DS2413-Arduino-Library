/*
     DS2413

     This example demonstrates the use of the DS2413 Library to read and write the PIO pins of the Maxim DS2413.

     by Ian Fleet

     (C) 2018, W3.TechChat.com.au

     All files, software, schematics and designs are provided as-is with no warranty.
     All files, software, schematics and designs are for experimental/hobby use.
     Under no circumstances should any part be used for critical systems where safety,
     life or property depends upon it. You are responsible for all use.
     You are free to use, modify, derive or otherwise extend for your own purposes provided credit is
     given to the author (i.e. portions © Ian Fleet, W3.TechChat.com.au) and provide a link to the original source.

*/


#include <Arduino.h>
#include <OneWire.h>
#include <DS2413.h>

// define the Arduino digital I/O pin to be used for the 1-Wire network
const uint8_t ONE_WIRE_PIN = 4;

// define the 1-Wire address of the DS2413
uint8_t DS2413_address[8];

OneWire ow(ONE_WIRE_PIN);
DS2413 ds2413(&ow);

int DS2413devices = 0;
int PIO_data = 0;
int PIO_Pins = 0x0;

/////////////////////////////////////

void printAddress(uint8_t* addr, uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
  {
    Serial.print(addr[i] >> 4, HEX);
    Serial.print(addr[i] & 0x0F, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

/////////////////////////////////////

void setup() {
  Serial.begin(9600);

  if (ds2413.begin() == 0) {
    Serial.println("DS2413 Begin OK");
  }
  else {
    Serial.println("DS2413 Begin Error");
  }
  DS2413devices = ds2413.getDeviceCount();

  Serial.print("Found ");
  Serial.print(DS2413devices);
  Serial.println(" Devices");

  // print addresses of the devices found
  int device = 1;
  while (device <= DS2413devices) {
    ds2413.getAddress(DS2413_address, device);
    Serial.print("Device ");
    Serial.print(device);
    Serial.print(" : ");
    printAddress(DS2413_address, 8);
    device++;
  }
}

/////////////////////////////////////

void loop() {

  if (ds2413.getAddress(DS2413_address, 1)) {
    if (!ds2413.validAddress(DS2413_address)) {
      Serial.println("validAddress Error");
    } else {
      if (!ds2413.writePIO(DS2413_address, PIO_Pins)) {
        Serial.println("Error writing DS2413 device PIO");
      }
      else {
        PIO_data = ds2413.getPIO(DS2413_address);
        if (ds2413.isError()) {
          Serial.println("Error reading DS2413 device PIO");
        } else {
          Serial.print("DS2413 PIO :");
          Serial.print(PIO_data, HEX);
          Serial.println(" ");
        }
      }
    }
  } else Serial.println("getAddress Error");
  delay(2000);
  PIO_Pins = (PIO_Pins + 1); // cycle though all output latch combinations
}
