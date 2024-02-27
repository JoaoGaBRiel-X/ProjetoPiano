#include "VS1053.h"

//
// Plugin to put VS10XX into realtime MIDI mode
// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
// Permission to reproduce here granted by VLSI solution.
//
const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = 
{ /* Compressed plugin */
    0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
    0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
    0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
    0x0200, 0x000a, 0x0001, 0x0050,
};


void sendMIDI(byte data) 
{
    SPI.transfer(0);
    SPI.transfer(data);
}

void talkMIDI(byte cmd, byte data1, byte data2) 
{
    //
    // Wait for chip to be ready (Unlikely to be an issue with real time MIDI)
    //
    while (!digitalRead(VS_DREQ));
    
    digitalWrite(VS_XDCS, LOW);

    sendMIDI(cmd);

    //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
    //(sort of: http://253.ccarh.org/handout/midiprotocol/)
    if ( (cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0) 
    {
        sendMIDI(data1);
        sendMIDI(data2);
    } 
    else 
    {
        sendMIDI(data1);
    }

    digitalWrite(VS_XDCS, HIGH);
}

void initialiseVS1053 () 
{
    // Set up the pins controller the SPI link to the VS1053
    pinMode(VS_DREQ, INPUT_PULLUP);
    pinMode(VS_XCS, OUTPUT);
    pinMode(VS_XDCS, OUTPUT);
    digitalWrite(VS_XCS, HIGH);  //Deselect Control
    digitalWrite(VS_XDCS, HIGH); //Deselect Data
    pinMode(VS_RESET, OUTPUT);

    //Setup SPI for VS1053
    pinMode(10, OUTPUT); // Pin 10 must be set as an output for the SPI communication to work
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz.
    //Internal clock multiplier is 1.0x after power up.
    //Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
    SPI.setClockDivider(SPI_CLOCK_DIV16); //Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
    SPI.transfer(0xFF); //Throw a dummy byte at the bus

    delayMicroseconds(1);

    //delay(500);

    digitalWrite(VS_RESET, HIGH); //Bring up VS1053


    //delay(500);
    // Enable real-time MIDI mode
    VSLoadUserCode();
}

//Write to VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte) 
{
    while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating IC is available
    digitalWrite(VS_XCS, LOW); //Select control

    //SCI consists of instruction byte, address byte, and 16-bit data word.
    SPI.transfer(0x02); //Write instruction
    SPI.transfer(addressbyte);
    SPI.transfer(highbyte);
    SPI.transfer(lowbyte);
    while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating command is complete
    digitalWrite(VS_XCS, HIGH); //Deselect Control
}


void VSLoadUserCode(void) 
{
    unsigned int i = 0;

    while (i < sizeof(sVS1053b_Realtime_MIDI_Plugin) / sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) 
    {
        unsigned short addr, n, val;
        addr = sVS1053b_Realtime_MIDI_Plugin[i++];
        n = sVS1053b_Realtime_MIDI_Plugin[i++];
        while (n--) 
        {
            val = sVS1053b_Realtime_MIDI_Plugin[i++];
            VSWriteRegister(addr, val >> 8, val & 0xFF);
        }
    }
}