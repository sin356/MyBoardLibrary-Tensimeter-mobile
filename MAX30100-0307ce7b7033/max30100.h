#ifndef MAX30100_H
#define MAX30100_H

#include "max30100_reg.h"
#include "mbed.h"

typedef enum{ // This is the same for both LEDs
  pw200,    // 200us pulse
  pw400,    // 400us pulse
  pw800,    // 800us pulse
  pw1600    // 1600us pulse
}pulseWidth;

typedef enum{
  sr50,    // 50 samples per second
  sr100,   // 100 samples per second
  sr167,   // 167 samples per second
  sr200,   // 200 samples per second
  sr400,   // 400 samples per second
  sr600,   // 600 samples per second
  sr800,   // 800 samples per second
  sr1000   // 1000 samples per second
}sampleRate;

typedef enum{
  i0,    // No current
  i4,    // 4.4mA
  i8,    // 7.6mA
  i11,   // 11.0mA
  i14,   // 14.2mA
  i17,   // 17.4mA
  i21,   // 20.8mA
  i27,   // 27.1mA
  i31,   // 30.6mA
  i34,   // 33.8mA
  i37,   // 37.0mA
  i40,   // 40.2mA
  i44,   // 43.6mA
  i47,   // 46.8mA
  i50    // 50.0mA
}ledCurrent;

class max30100 {
 
    protected:
 
    public:

    //
    int i2c_write (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte );
    int i2c_read (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte );
    //
   
    void setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir);
    void setSPO2(sampleRate sr);
    int getNumSamp(void);
    void readSensor(void);
    void shutdown(void);
    void reset(void);
    void startup(void);
    uint8_t getRevID(void);
    uint8_t getPartID(void);
    void begin(pulseWidth pw, ledCurrent ir, sampleRate sr);
    void printRegisters(void);
};
#endif