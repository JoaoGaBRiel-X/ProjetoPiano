#include "VS1053.h"

extern uint8_t volume;

byte bank = 0;
// List of instruments to send to any configured MIDI channels.
byte preset_instruments[16] = {
    /* 01 */ 0,
    /* 02 */ 0,
    /* 03 */ 10,
    /* 04 */ 10,
    /* 05 */ 10,
    /* 06 */ 0,
    /* 07 */ 0,
    /* 08 */ 0,
    /* 09 */ 0,
    /* 10 */ 0, // Channel 10 will be ignored later as that is percussion anyway.
    /* 11 */ 0,
    /* 12 */ 0,
    /* 13 */ 0,
    /* 14 */ 0,
    /* 15 */ 0,
    /* 16 */ 0};

byte instrument;

// Use binary to say which MIDI channels this should respond to.
// Every "1" here enables that channel. Set all bits for all channels.
// Make sure the bit for channel 10 is set if you want drums.
//
//                               16  12  8   4  1
//                               |   |   |   |  |
uint16_t MIDI_CHANNEL_FILTER = 0b1111111111111111;


byte altura = 12;


//
// Plugin to put VS10XX into realtime MIDI mode
// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
// Permission to reproduce here granted by VLSI solution.
//
const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] =
    {
        /* Compressed plugin */
        0x0007,
        0x0001,
        0x8050,
        0x0006,
        0x0014,
        0x0030,
        0x0715,
        0xb080, /*    0 */
        0x3400,
        0x0007,
        0x9255,
        0x3d00,
        0x0024,
        0x0030,
        0x0295,
        0x6890, /*    8 */
        0x3400,
        0x0030,
        0x0495,
        0x3d00,
        0x0024,
        0x2908,
        0x4d40,
        0x0030, /*   10 */
        0x0200,
        0x000a,
        0x0001,
        0x0050,
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
    while (!digitalRead(VS_DREQ))
        ;

    digitalWrite(VS_XDCS, LOW);

    sendMIDI(cmd);

    // Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
    //(sort of: http://253.ccarh.org/handout/midiprotocol/)
    if ((cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0)
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

void initialiseVS1053()
{


    // Set up the pins controller the SPI link to the VS1053
    pinMode(VS_DREQ, INPUT_PULLUP);
    pinMode(VS_XCS, OUTPUT);
    pinMode(VS_XDCS, OUTPUT);
    digitalWrite(VS_XCS, HIGH);  // Deselect Control
    digitalWrite(VS_XDCS, HIGH); // Deselect Data
    pinMode(VS_RESET, OUTPUT);

    // Setup SPI for VS1053
    pinMode(10, OUTPUT); // Pin 10 must be set as an output for the SPI communication to work
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    // From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz.
    // Internal clock multiplier is 1.0x after power up.
    // Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
    SPI.setClockDivider(SPI_CLOCK_DIV16); // Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
    SPI.transfer(0xFF);                   // Throw a dummy byte at the bus

    delayMicroseconds(1);

    // delay(500);

    digitalWrite(VS_RESET, HIGH); // Bring up VS1053

    // delay(500);
    //  Enable real-time MIDI mode
    VSLoadUserCode();
}

// Write to VS10xx register
// SCI: Data transfers are always 16bit. When a new SCI operation comes in
// DREQ goes low. We then have to wait for DREQ to go high again.
// XCS should be low for the full duration of operation.
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte)
{
    while (!digitalRead(VS_DREQ))
        ;                      // Wait for DREQ to go high indicating IC is available
    digitalWrite(VS_XCS, LOW); // Select control

    // SCI consists of instruction byte, address byte, and 16-bit data word.
    SPI.transfer(0x02); // Write instruction
    SPI.transfer(addressbyte);
    SPI.transfer(highbyte);
    SPI.transfer(lowbyte);
    while (!digitalRead(VS_DREQ))
        ;                       // Wait for DREQ to go high indicating command is complete
    digitalWrite(VS_XCS, HIGH); // Deselect Control
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

void midiSetChannelVolume(uint8_t chan, uint8_t vol)
{
    if (chan > 15)
        return;
    if (vol > 127)
        return;

    talkMIDI(MIDI_CHAN_MSG | chan, MIDI_CHAN_VOLUME, vol);
}

void midiSetInstrument(uint8_t chan, uint8_t inst)
{
    if (chan > 15)
        return;
    inst--; // page 32 has instruments starting with 1 not 0 :(
    if (inst > 127)
        return;

    talkMIDI(MIDI_CHAN_PROGRAM | chan, inst, 127);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank)
{
    if (chan > 15)
        return;
    if (bank > 127)
        return;

    talkMIDI(MIDI_CHAN_MSG | chan, (uint8_t)MIDI_CHAN_BANK, bank);
}

byte traduzNota(char t)
{
    byte n = 0;
    if (t == 'z')
        n = 36;
    else if (t == '1')
        n = 37;
    else if (t == 'x')
        n = 38;
    else if (t == '2')
        n = 39;
    else if (t == 'c')
        n = 40;
    else if (t == 'v')
        n = 41;
    else if (t == '3')
        n = 42;
    else if (t == 'b')
        n = 43;
    else if (t == '4')
        n = 44;
    else if (t == 'n')
        n = 45;
    else if (t == '5')
        n = 46;
    else if (t == 'm')
        n = 47;

    else if (t == 'a')
        n = 48;
    else if (t == '6')
        n = 49;
    else if (t == 's')
        n = 50;
    else if (t == '7')
        n = 51;
    else if (t == 'd')
        n = 52;
    else if (t == 'f')
        n = 53;
    else if (t == '8')
        n = 54;
    else if (t == 'g')
        n = 55;
    else if (t == '9')
        n = 56;
    else if (t == 'h')
        n = 57;
    else if (t == '0')
        n = 58;
    else if (t == 'j')
        n = 59;

    else if (t == 'q')
        n = 60;
    else if (t == 'i')
        n = 61;
    else if (t == 'w')
        n = 62;
    else if (t == 'o')
        n = 63;
    else if (t == 'e')
        n = 64;
    else if (t == 'r')
        n = 65;
    else if (t == 'p')
        n = 66;
    else if (t == 't')
        n = 67;
    else if (t == 'k')
        n = 68;
    else if (t == 'y')
        n = 69;
    else if (t == 'l')
        n = 70;
    else if (t == 'u')
        n = 71;

    return n + altura;
}

void toca_midi(byte n)
{
    talkMIDI(0x90, n, volume);
    delay(10);
}

void para_midi(byte n)
{
    delay(5);
    talkMIDI(0x80, n, volume);
}