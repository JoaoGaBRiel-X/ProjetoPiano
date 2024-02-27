#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MIDI.h>
#include <SoftwareSerial.h>
#include <Keyboard.h>
#include <Keypad.h>
#include <Adafruit_VS1053.h>
#include "VS1053.h"


//
// Assinaturas de Funções
//
void toca_midi(byte n);
void para_midi(byte n);
byte traduzNota(char t);
byte c = 0;
char leitura_teclas;

// Use binary to say which MIDI channels this should respond to.
// Every "1" here enables that channel. Set all bits for all channels.
// Make sure the bit for channel 10 is set if you want drums.
//
//                               16  12  8   4  1
//                               |   |   |   |  |
uint16_t MIDI_CHANNEL_FILTER = 0b1111111111111111;

// There are three selectable sound banks on the VS1053
// These can be selected using the MIDI command 0xBn 0x00 bank
#define DEFAULT_SOUND_BANK 0x00  // General MIDI 1 sound bank
#define DRUM_SOUND_BANK    0x78  // Drums
#define ISNTR_SOUND_BANK   0x79  // General MIDI 2 sound bank

char tecla = 0;
byte altura = 12;
byte nota = 0;
const byte LINHAS = 6; // Linhas do teclado
const byte COLUNAS = 6; // Colunas do teclado
const byte PINOS_LINHAS[LINHAS] = {3, 30, 5, 8, 10, 13 }; // l1=pd0,l2=pd4,l3=pc6,l4=pb4,l5=pb6,l6=pc7
const byte PINOS_COLUNAS[COLUNAS] = {A0, A1, A2, A3, A4, A5}; // r0,r1,r2,r3,r4,r5,r6,r7

char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado TOMPLAY)
  {'c','m','8','j','r','u'},
  {'x','n','d','h','e','l'},
  {'2','5','s','g','o','y'},
  {'z','b','7','0','w','k'},
  {'1','4','a','f','i','t'},
  {'3','v','6','9','q','p'}
};

Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS); // Inicia teclado

byte bank = 0;
// List of instruments to send to any configured MIDI channels.
byte preset_instruments[16] = {
  /* 01 */  0,
  /* 02 */  0,
  /* 03 */  10,
  /* 04 */  10,
  /* 05 */  10,
  /* 06 */  0,
  /* 07 */  0,
  /* 08 */  0,
  /* 09 */  0,
  /* 10 */  0,  // Channel 10 will be ignored later as that is percussion anyway.
  /* 11 */  0,
  /* 12 */  0,
  /* 13 */  0,
  /* 14 */  0,
  /* 15 */  0,
  /* 16 */  0
};

// This is required to set up the MIDI library.
// The default MIDI setup uses the built-in serial port.
MIDI_CREATE_DEFAULT_INSTANCE();

byte instrument;

void setup() {
  //Interrupcao
  // Configuração do timer1 
  TCCR1A = 0;                        //confira timer para operação normal pinos OC1A e OC1B desconectados
  TCCR1B = 0;                        //limpa registrador
  TCCR1B |= (1<<CS10)|(1 << CS12);   // configura prescaler para 1024: CS12 = 1 e CS10 = 1
 
  TCNT1 = 0xFF9F;                    // incia timer com valor para que estouro ocorra em 1 segundo
                                     // 65536-(16MHz/1024/1Hz) = 65.146 = 0xFF9F
  
  TIMSK1 |= (1 << TOIE1);           // habilita a interrupção do TIMER1

  Keyboard.begin();

  initialiseVS1053();

  MIDI.begin(MIDI_CHANNEL_OMNI);
  talkMIDI(0xb0, 0x0b, 127);

  delay(100);

  toca_midi((byte) 72);
  delay(300);
  toca_midi((byte) 76);
  delay(300);
  toca_midi((byte) 79);
}
void loop() 
{
  if (leitura_teclas) 
  {
    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
     if ( teclado_personalizado.key[i].stateChanged )   // Only find keys that have changed state.
      {
        if (teclado_personalizado.key[i].kstate == PRESSED) {
            Keyboard.print(teclado_personalizado.key[i].kchar);
            tecla = teclado_personalizado.key[i].kchar;
            nota = traduzNota(tecla);
            toca_midi(nota);
        }
        if (teclado_personalizado.key[i].kstate == RELEASED) {
            para_midi(nota);
        }
      }
    }
  }
}


byte traduzNota(char t) {
    byte n = 0;
         if (t == 'z') n = 36;
    else if (t == '1') n = 37;
    else if (t == 'x') n = 38;
    else if (t == '2') n = 39;
    else if (t == 'c') n = 40;
    else if (t == 'v') n = 41;
    else if (t == '3') n = 42;
    else if (t == 'b') n = 43;
    else if (t == '4') n = 44;
    else if (t == 'n') n = 45;
    else if (t == '5') n = 46;
    else if (t == 'm') n = 47;

    else if (t == 'a') n = 48;
    else if (t == '6') n = 49;
    else if (t == 's') n = 50;
    else if (t == '7') n = 51;
    else if (t == 'd') n = 52;
    else if (t == 'f') n = 53;
    else if (t == '8') n = 54;
    else if (t == 'g') n = 55;
    else if (t == '9') n = 56;
    else if (t == 'h') n = 57;
    else if (t == '0') n = 58;
    else if (t == 'j') n = 59;

    else if (t == 'q') n = 60;
    else if (t == 'i') n = 61;
    else if (t == 'w') n = 62;
    else if (t == 'o') n = 63;
    else if (t == 'e') n = 64;
    else if (t == 'r') n = 65;
    else if (t == 'p') n = 66;
    else if (t == 't') n = 67;
    else if (t == 'k') n = 68;
    else if (t == 'y') n = 69;
    else if (t == 'l') n = 70;
    else if (t == 'u') n = 71;

    return n + altura;
}

void toca_midi(byte n) {
  talkMIDI (0x90, n, 127);
  delay (10);
}

void para_midi(byte n){
  delay (10);
  talkMIDI (0x80, n, 0);
}

ISR(TIMER1_OVF_vect)                              //interrupção do TIMER1 
{
  TCNT1 = 0xFF9F;                                 // Renicia TIMER
  leitura_teclas = teclado_personalizado.getKeys();
}