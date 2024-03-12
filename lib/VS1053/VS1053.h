#ifndef VS1053_H
#define VS1053_H

#include <Arduino.h>
#include <SPI.h>
#include <MIDI.h>

// VS1053 Shield pin definitions
#define VS_XCS    6 // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   7 // Data Chip Select / BSYNC Pin
#define VS_DREQ   2 // Data Request Pin: Player asks for more data
#define VS_RESET  9 // Reset is active low


#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x7

#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0



// There are three selectable sound banks on the VS1053
// These can be selected using the MIDI command 0xBn 0x00 bank
#define DEFAULT_SOUND_BANK 0x00 // General MIDI 1 sound bank
#define DRUM_SOUND_BANK 0x78    // Drums
#define ISNTR_SOUND_BANK 0x79   // General MIDI 2 sound bank


void sendMIDI(byte data);
void talkMIDI(byte cmd, byte data1, byte data2);
void initialiseVS1053 ();
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte);
void VSLoadUserCode(void);
void midiSetChannelVolume(uint8_t chan, uint8_t vol);
void midiSetChannelBank(uint8_t chan, uint8_t bank);
void midiSetInstrument(uint8_t chan, uint8_t inst);
void toca_midi(byte n);
void para_midi(byte n);
byte traduzNota(char t);



#endif