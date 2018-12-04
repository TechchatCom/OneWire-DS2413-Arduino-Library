#include "DS2413.h"

#define DEBUG

//////////////////////////////////////
// initialises the onewire reference
DS2413::DS2413(OneWire* _onewire) {
  _ow = _onewire;
  _devices = 0;

};

//////////////////////////////////////

bool DS2413::begin(void) {
  // determine the number of DS2413 devices. Return error if none
  _error = true;
  _ow->reset();
  _ow->reset_search();
  _devices = 0; // Reset the number of devices when we enumerate wire devices
  while (_ow->search(_address))
  {
    if (validAddress(_address))
    {
      _devices++;
      _error = false;
    }
  }
  return _error;
}

//////////////////////////////////////

// finds an address at a given index on the bus
// returns true if the device was found
bool DS2413::getAddress(uint8_t* deviceaddress, uint8_t index)
{
  uint8_t depth = 1;
  _error = false;
  _ow->reset();
  _ow->reset_search();
  while (depth <= index && _ow->search(deviceaddress))
  {
    if (depth == index && validAddress(deviceaddress)) return true;
    if (validAddress(deviceaddress)) depth++;
  }
  return false;
}

//////////////////////////////////////

// returns true if DS2413 address is valid
bool DS2413::validAddress(uint8_t* deviceaddress)

{
  if ((deviceaddress[0] != DS2413_FAMILY_ID) &&
      (deviceaddress[0] != CLONE_FAMILY_ID)) {
    return false;
  }
  return (_ow->crc8(deviceaddress, 7) == deviceaddress[7]);
}

//////////////////////////////////////

// returns the number of devices found on the bus
int DS2413::getDeviceCount(void)
{
  return _devices;
}

//////////////////////////////////////
// return error state from the previous operation
boolean DS2413::isError() {
  return _error;
}

//////////////////////////////////////
//   Bits: 76543210
//         |||||||+- PIOA Pin state
//         ||||||+-- PIOA Out Latch state
//         |||||+--- PIOB Pin state
//         ||||+---- PIOB Out Latch state
//         |||+----- Bit 0 inverted
//         ||+------ Bit 1 inverted
//         |+------- Bit 2 inverted
//         +-------- Bit 3 inverted

// return state of the PIO pins (value = -1 if error)

byte DS2413::getPIO(uint8_t* deviceaddress)
{
  bool ok = false;
  uint8_t results;

  _ow->reset();
  _ow->select(deviceaddress);
  _ow->write(DS2413_ACCESS_READ);

  results = _ow->read();                    //Get the PIO status
  ok = (~results & 0x0F) == (results >> 4); // Compare nibbles as verification
  results &= 0x0F;                          // Clear inverted value

  _ow->reset();
  return ok ? results : -1;
}

//////////////////////////////////////
//   Bits: 76543210
//         |||||||+- PIOA Latch state, 0 = output switch on, 1 = input
//         ||||||+-- PIOB Latch state, 0 = output switch on, 1 = input
//         |||||+--- 1
//         ||||+---- 1
//         |||+----- 1
//         ||+------ 1
//         |+------- 1
//         +-------- 1

// write state value to the PIO pins and check if error

bool DS2413::writePIO(uint8_t* deviceaddress, int state)
{
  uint8_t ack = 0;

  // bits 2-7 must '1'
  state |= 0xFC;
  uint8_t results;
  bool ok = false;

  _ow->reset();
  _ow->select(deviceaddress);
  _ow->write(DS2413_ACCESS_WRITE);
  _ow->write(state);
  _ow->write(~state);                    // DS2413 checks inverted state matches
  ack = _ow->read();                     // 0xAA=success
  if (ack == DS2413_ACK_SUCCESS)
  {
    results = _ow->read();    // Read the status byte
    ok = (((~results & 0x0F) == (results >> 4))       // Compare nibbles as verification
          & (((results & 0x2) >> 1) == (state & 0x1)) // and PIO latch status agrees
          & (((results & 0x8) >> 2) == (state & 0x2)));
  }
  _ow->reset();

  return (ok ? true : false);
}
