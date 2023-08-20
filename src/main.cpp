#include <Arduino.h>
#include <SPI.h>
#include <MIDI.h>
#include <SoftwareSerial.h>
#include <Keyboard.h>
#include <Keypad.h>
#include "VS1053.h"

//
// Assinaturas de Funções
//
void toca_midi(byte n);
byte traduzNota(char t);

// Use binary to say which MIDI channels this should respond to.
// Every "1" here enables that channel. Set all bits for all channels.
// Make sure the bit for channel 10 is set if you want drums.
//
//                               16  12  8   4  1
//                               |   |   |   |  |
uint16_t MIDI_CHANNEL_FILTER = 0b1111111111111111;

// Channel to link to the potentiometer
#define POT_MIDI_CHANNEL 1



// There are three selectable sound banks on the VS1053
// These can be selected using the MIDI command 0xBn 0x00 bank
#define DEFAULT_SOUND_BANK 0x00  // General MIDI 1 sound bank
#define DRUM_SOUND_BANK    0x78  // Drums
#define ISNTR_SOUND_BANK   0x79  // General MIDI 2 sound bank

int counter = 0;                 // button push counter
int buttonState = 0;
char inicio = 0;
char tecla = 0;

char tecla_z = 0;
char tecla_x = 0;
char tecla_c = 0;
char tecla_u = 0;
char tecla_y = 0;
char tecla_t = 0;
char tecla_r = 0;
char tecla_e = 0;
char tecla_w = 0;
char tecla_q = 0;
char tecla_j = 0;
char tecla_h = 0;
char tecla_g = 0;
char tecla_v = 0;
char tecla_b = 0;
char tecla_n = 0;
char tecla_m = 0;

char tocando = 0;
char toca = 0;

byte altura = 0;
byte nota = 0;
const byte LINHAS = 8; // Linhas do teclado
const byte COLUNAS = 8; // Colunas do teclado
byte PINOS_LINHAS[LINHAS] = {A0, A1, A2, A3, A4, A5, 0, 30}; // C1,c2,c3,c4,c5,c6,c7,c11
byte PINOS_COLUNAS[COLUNAS] = {3, 4, 5, 8, 10, 11, 12, 13}; // r0,r1,r2,r3,r4,r5,r6,r7

/*  const char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres NUMERADO
  {'O', ' ', 'C',' ', '1', ' ', ' ', '2'},
  {'Q', ' ', 'E',' ', '3', ' ', ' ', '4'},
  {'S', ' ', 'G',' ', '5', ' ', ' ', '7'},
  {'T', 'V', 'H','J', '6', '8', 'A', '9'},
  {'Z', 'X', 'N','L', 'B', '0', 'D', 'F'},
  {'P', ' ', 'W',' ', ' ', ' ', ' ', 'I'},
  {'R', ' ', 'Y',' ', ' ', ' ', ' ', 'K'},
  {'U', ' ', ' ',' ', ' ', ' ', ' ', 'M'}
  };
*/

/*
  //  Tecaldo Piano
  const char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres simulador piano internet
  {'v', ' ', 'u',' ', 0X09, ' ', ' ', '1'},
  {'b', ' ', 'i',' ', 'q', ' ', ' ', '2'},
  {'n', ' ', 'o',' ', 'w', ' ', ' ', '4'},
  {'m', ',', 'p','z', 'e', 'r', '6', '5'},
  {'/', '.', 'c','x', 'y', 't', '8', '9'},
  {'g', ' ', 'l',' ', ' ', ' ', ' ', 'a'},
  {'h', ' ', ';',' ', ' ', ' ', ' ', 's'},
  {'k', ' ', ' ',' ', ' ', ' ', ' ', 'd'}
  };
*/

char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado TOMPLAY)
  {'q', 'o', 'i', 'e', 'w', 'r', 'u', 'p'},
  {' ', ' ', ' ', ' ', ' ', 't', 'y', ' '},
  {'a', 'l', 'k', 'd', 's', 'f', 'j', ' '},
  {' ', ' ', ' ', ' ', ' ', 'g', 'h', ' '},
  {'z', ' ', ' ', 'c', 'x', 'v', 'm', ' '},
  {' ', ' ', ' ', ' ', ' ', 'b', 'n', ' '},
  {'1', '9', '8', '3', '2', '4', '7', '0'},
  {' ', ' ', ' ', ' ', ' ', '5', '6', ' '}
};
/*
  const char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado TOMPLAY)
  {'0', '1', '2','3', '4', '5', '6', '7'},
  {'8', '9', 'A','B', 'C', 'D', 'E', 'F'},
  {'G', 'H', 'I','J', 'K', 'L', 'M', 'N'},
  {'O', 'P', 'Q','R', 'S', 'T', 'U', 'V'},
  {'W', 'X', 'Y','Z', 'a', 'b', 'c', 'd'},
  {'e', 'f', 'g','h', 'i', 'j', 'k', 'l'},
  {'m', 'n', 'o','p', 'q', 'r', 's', 't'},
  {'u', 'v', 'w','x', 'y', 'z', '!', '@'},
  };
*/

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
  Keyboard.begin();
  // put your setup code here, to run once:
  initialiseVS1053();
  //  Serial.begin(9600);
  // This listens to all MIDI channels
  // They will be filtered out later...
  MIDI.begin(MIDI_CHANNEL_OMNI);
  talkMIDI(0xb0, 0x0b, 120);
  //
  //  for (byte i = 0; i < 128; i++) {
  //    teste_midi();
  //    Serial.println(bank);
  //    bank++;
  //  }
  delay(1000);

  // Configure the instruments for all required MIDI channels.
  // Even though MIDI channels are 1 to 16, all the code here
  // is working on 0 to 15 (bitshifts, index into array, MIDI command).
  //  for (byte ch=0; ch<16; ch++) {
  //    if (ch != 9) { // Ignore channel 10 (drums)
  //      uint16_t ch_filter = 1<<ch;
  //      if (MIDI_CHANNEL_FILTER & ch_filter) {
  //        talkMIDI(0xC0|ch, preset_instruments[ch], 0);
  //      }
  //    }
  //  }
  // For some reason, the last program change isn't registered
  // without an extra "dummy" read here.
  //  talkMIDI(0x80, 0, 0);
}
void loop() 
{
    if (tocando == 0) 
    {
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_u == 2) 
        {
            talkMIDI (0xC0, 0, 127);
            if (toca == 55) 
            {
                toca = 0;
            }  
            else 
            {
                toca = 55;
            }
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_j == 2) 
        {
            talkMIDI (0xC0, 2, 127);
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_g == 2) 
        {
            talkMIDI (0xC0, 14, 127);
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_h == 2) 
        {
            talkMIDI (0xC0, 25, 127);
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_e == 2) {
            talkMIDI (0xC0, 40, 127);
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_r == 2) {
            talkMIDI (0xC0, 66, 127);
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_t == 2) {
            talkMIDI (0xC0, 73, 127);
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_y == 2) {
            talkMIDI (0xC0, 114, 127);
            //toca betovem
            tocando = 55;
        }

        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_v == 2) {
            altura = 12;
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_b == 2) {
            altura = 24;
            //toca betovemz
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_n == 2) {
            altura = 36;
            //toca betovem
            tocando = 55;
        }
        if (tecla_z == 2 && tecla_x == 2 && tecla_c == 2 && tecla_m == 2) {
            altura = 0;
            //toca betovem
            tocando = 55;
        }
    }
  //            playerMP3.stop();
  char leitura_teclas = teclado_personalizado.getKeys(); // Atribui a variavel a leitura do teclado
  if (leitura_teclas) {
    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
      if ( teclado_personalizado.key[i].stateChanged )   // Only find keys that have changed state.
      {
        if (teclado_personalizado.key[i].kchar == 'z') {
          tecla_z = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'x') {
          tecla_x = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'c') {
          tecla_c = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'e') {
          tecla_e = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'r') {
          tecla_r = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 't') {
          tecla_t = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'y') {
          tecla_y = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'u') {
          tecla_u = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'h') {
          tecla_h = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'g') {
          tecla_g = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'j') {
          tecla_j = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'v') {
          tecla_v = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'b') {
          tecla_b = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'n') {
          tecla_n = teclado_personalizado.key[i].kstate;
        }
        if (teclado_personalizado.key[i].kchar == 'm') {
          tecla_m = teclado_personalizado.key[i].kstate;
        }

        if (teclado_personalizado.key[i].kstate == PRESSED) {
          Keyboard.print(teclado_personalizado.key[i].kchar);
          if (toca == 0) {
            tecla = teclado_personalizado.key[i].kchar;
            nota = traduzNota(tecla);
            toca_midi(nota);
            tecla = 0;
          }
          else
          {
            tecla = 0;
          }
          tocando = 0;
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
    // Serial.println(altura);
    return n + altura;
  
}

void toca_midi(byte n) {
  talkMIDI (0x90, n, 127);
  delay (180);
  talkMIDI (0x80, n, 0);
  delay (20);
}