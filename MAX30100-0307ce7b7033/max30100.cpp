#include "max30100.h"

//Set up I2C, (SDA,SCL)
static I2C i2c(I2C_SDA, I2C_SCL);
static Serial pc(USBTX, USBRX); // tx, rx

uint16_t IR = 0;      // Last IR reflectance datapoint
uint16_t RED = 0;     // Last Red reflectance datapoint

//Wire read and write protocols
int max30100::i2c_write (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte )
{
    int ret;
    char *tmp_buffer;

    tmp_buffer = (char*)malloc(sizeof(char)*(Nbyte+1));

    /* First, send device address. Then, send data and STOP condition */
    tmp_buffer[0] = register_addr;
    memcpy(tmp_buffer+1, buffer, Nbyte);

    ret = i2c.write(i2c_addr, tmp_buffer, Nbyte+1, false);

    return ret;
}

int max30100::i2c_read (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte )
{
    int ret;

    /* Send device address, with no STOP condition */
    ret = i2c.write(i2c_addr, (const char*)&register_addr, 1, true);
    if(!ret) {
        /* Read data, with STOP condition  */
        ret = i2c.read((i2c_addr|0x01), buffer, Nbyte, false);
    }

    return ret;
}
//
   
void max30100::setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xFC; // Set LED_PW to 00
  reg[0] = reg[0] | pw;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);     // Mask LED_PW
  reg[0] = (red<<4) | ir;
  i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, &reg[0], 1); // write LED configs
}

void max30100::setSPO2(sampleRate sr){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xE3; // Set SPO2_SR to 000
  reg[0] = reg[0] | (sr<<2);
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1); // Mask SPO2_SR
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);
  reg[0] = reg[0] & 0xf8; // Set Mode to 000
  reg[0] = reg[0] | 0x03;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1); // Mask MODE
}

int max30100::getNumSamp(void){
    char wrPtr[1];
    char rdPtr[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR, &wrPtr[0], 1);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR, &rdPtr[0], 1);
    return (abs( 16 + wrPtr[0] - rdPtr[0] ) % 16);
}

void max30100::readSensor(void){
  char temp[4] = {0};  // Temporary buffer for read values
  i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_DATA, &temp[0], 4);  // Read four times from the FIFO
  IR = (temp[0]<<8) | temp[1];    // Combine values to get the actual number
  RED = (temp[2]<<8) | temp[3];   // Combine values to get the actual number
}

void max30100::shutdown(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] | 0x80;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the SHDN bit
}

void max30100::reset(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] | 0x40;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the RESET bit
}

void max30100::startup(void){
  char reg[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);  // Get the current register
  reg[0] = reg[0] & 0x7F;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);   // mask the SHDN bit
}

uint8_t max30100::getRevID(void){
  char buffer[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_REV_ID, &buffer[0], 1);
  return buffer[0];
}

uint8_t max30100::getPartID(void){
  char buffer[1];
  i2c_read(MAX30100_ADDRESS, MAX30100_PART_ID, &buffer[0], 1);
  return buffer[0];
}

void max30100::begin(pulseWidth pw, ledCurrent ir, sampleRate sr){
  char buffer[1];
  buffer[0] = 0x02;
  i2c_write(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &buffer[0], 1); // Heart rate only
  buffer[0] = ir;
  i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, &buffer[0], 1);
  buffer[0] = (sr<<2)|pw;
  i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &buffer[0], 1);
}

void max30100::printRegisters(void){
    char reg[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_INT_STATUS, &reg[0], 1);
    pc.printf("MAX30100_INT_STATUS: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_INT_ENABLE, &reg[0], 1);
    pc.printf("MAX30100_INT_ENABLE: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR, &reg[0], 1);
    pc.printf("MAX30100_FIFO_WR_PTR: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_OVRFLOW_CTR, &reg[0], 1);
    pc.printf("MAX30100_OVRFLOW_CTR: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR, &reg[0], 1);
    pc.printf("MAX30100_FIFO_RD_PTR: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_DATA, &reg[0], 1);
    pc.printf("MAX30100_FIFO_DATA: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, &reg[0], 1);
    pc.printf("MAX30100_MODE_CONFIG: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, &reg[0], 1);
    pc.printf("MAX30100_SPO2_CONFIG: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_LED_CONFIG, &reg[0], 1);
    pc.printf("MAX30100_LED_CONFIG: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_INTG, &reg[0], 1);
    pc.printf("MAX30100_TEMP_INTG: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_FRAC, &reg[0], 1);
    pc.printf("MAX30100_TEMP_FRAC: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_REV_ID, &reg[0], 1);
    pc.printf("MAX30100_REV_ID: %d\r\n",reg[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_PART_ID, &reg[0], 1);
    pc.printf("MAX30100_PART_ID: %d\r\n",reg[0]);
}