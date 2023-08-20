#ifndef VS1053_H
#define VS1053_H

#include <Arduino.h>
#include <SPI.h>

// VS1053 Shield pin definitions
#define VS_XCS    6 // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   7 // Data Chip Select / BSYNC Pin
#define VS_DREQ   2 // Data Request Pin: Player asks for more data
#define VS_RESET  8 // Reset is active low

void sendMIDI(byte data);
void talkMIDI(byte cmd, byte data1, byte data2);
void initialiseVS1053 ();
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte);
void VSLoadUserCode(void);

#endif