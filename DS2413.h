#ifndef DS2413_h
#define DS2413_h

#include <Arduino.h>
#include <OneWire.h>

#define DS2413_FAMILY_ID    0x3A // Maxim DS2413 device code
#define CLONE_FAMILY_ID     0x85 // clone device code
#define DS2413_ACCESS_READ  0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS  0xAA


typedef uint8_t DeviceAddress[8];

class DS2413 {

  public:
    DS2413(OneWire*);
    // start the library and determine number of DS2413 devices
    bool begin();
    // returns true if DS2413 address is valid
    bool validAddress(uint8_t* deviceaddress);
    // finds a DS2413 address at a given index on the bus
    bool getAddress(uint8_t* deviceaddress, uint8_t index);
    // returns number of DS2413 devices found (call begin first)
    int getDeviceCount();
    //check error from last operation
    boolean isError();
    // reads the PIO port pin state
    uint8_t getPIO(uint8_t *deviceaddress);
    // write to PIO port
    bool writePIO(uint8_t *deviceaddress, int state);

  private:
    OneWire *_ow;
    DeviceAddress _address;
    uint8_t _devices = 0;
    uint8_t _piodata;
    boolean _error = false;
};
#endif
