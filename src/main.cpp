#include <Arduino.h>
#include <Keypad.h>
#include <SoftwareSerial.h>


void executaComando(char tecla, byte comando);
byte traduzNota(char tecla);
byte traduzVolume(int value);

#define pinRx 0 //informe aqui qual porta Rx está sendo utilizada.
#define pinTx 1 //informe aqui qual porta Tx está sendo utilizada.
#define volumeMP3 15 //definimos o volume, entre 0 e 30.

//comandos
#define play 0x03
#define stop 0x08
#define reset 0x0c

SoftwareSerial mySerial(pinRx, pinTx);

const byte ROWS = 8; // Linhas do teclado
const byte COLS = 8; // Colunas do teclado
char keys[ROWS][COLS] = { // Matriz de caracteres (mapeamento do teclado)
  {'q', ' ', 'a',' ', 'z', ' ', ' ', '1'},
  {'w', ' ', 's',' ', 'x', ' ', ' ', '2'},
  {'e', ' ', 'd',' ', 'c', ' ', ' ', '3'},
  {'r', 't', 'f','g', 'v', 'b', '5', '4'},
  {'u', 'y', 'j','h', 'm', 'n', '6', '7'},
  {'i', ' ', 'k',' ', ' ', ' ', ' ', '8'},
  {'o', ' ', 'l',' ', ' ', ' ', ' ', '9'},
  {'p', ' ', ' ',' ', ' ', ' ', ' ', '0'}
  };
byte rowPins[ROWS] = {2, 3, 4, 5, 6, 7, 8, 9}; // C1,c2,c3,c4,c5,c6,c7,c11 //connect to the row pinouts of the kpd
byte colPins[COLS] = {10, 16, 14, 15, A0, A1, A2, A3}; // r0,r1,r2,r3,r4,r5,r6,r7 //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;

char teclaPressionada = 0;

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    loopCount = 0;
    startTime = millis();
    msg = "";

    // //set sleep mode
    // byte zero = 0x00;
    // mySerial.write(0x7e);
    // mySerial.write(0xff);
    // mySerial.write(0x06);
    // mySerial.write(0x0a);
    // mySerial.write(zero);
    // mySerial.write(zero);
    // mySerial.write(zero);
    // mySerial.write(0xfe);
    // mySerial.write(0xf1);
    // mySerial.write(0xef);  
    executaComando(teclaPressionada, stop);  
}


void loop() {
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        Serial.print("Average loops per second = ");
        Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    teclaPressionada = kpd.key[i].kchar;
                    // byte zero=0x00;
                    // byte volume = traduzVolume(volumeMP3);
                    // byte checksum = 0xffff-(0xff+0x06+0x06+0+0+volume) + 1;
                    // mySerial.write(0x7e);
                    // mySerial.write(0xff);
                    // mySerial.write(0x06);
                    // mySerial.write(0x06);
                    // mySerial.write(zero);
                    // mySerial.write(zero);
                    // mySerial.write(volume);
                    // mySerial.write(0xfe);
                    // mySerial.write(checksum);
                    // mySerial.write(0xef);
                    executaComando(teclaPressionada, play);
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    // for (int vol = volumeMP3; vol > 0; vol--)
                    // {
                    //     byte zero=0x00;
                    //     byte volume = traduzVolume(vol);
                    //     byte checksum = 0xffff-(0xff+0x06+0x06+0+0+volume) + 1;
                    //     mySerial.write(0x7e);
                    //     mySerial.write(0xff);
                    //     mySerial.write(0x06);
                    //     mySerial.write(0x06);
                    //     mySerial.write(zero);
                    //     mySerial.write(zero);
                    //     mySerial.write(volume);
                    //     mySerial.write(0xfe);
                    //     mySerial.write(checksum);
                    //     mySerial.write(0xef);
                    //     Serial.print("volume: ");
                    //     Serial.println(vol);
                    //     delay(30);
                    // }
                    executaComando(teclaPressionada, stop);
                    teclaPressionada = 0;
                    msg = " RELEASED.";
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print("Key ");
                Serial.print(kpd.key[i].kchar);
                Serial.println(msg);
            }
        }
    }
}  // End loop

void executaComando(char tecla, byte comando)
{
    byte track;
    if (comando == play) track = traduzNota(tecla);
    if (comando == stop) track = 0x25;
    byte checksum = 0xffff-(0xff+0x06+comando+0+0+track) + 1;
    byte zero=0x00;
    mySerial.write(0x7e);
    mySerial.write(0xff);
    mySerial.write(0x06);
    mySerial.write(comando);
    mySerial.write(zero);
    mySerial.write(zero);
    mySerial.write(track);
    mySerial.write(0xfe);
    mySerial.write(checksum);
    mySerial.write(0xef);    
}

byte traduzNota(char tecla)
{
  byte track = 0;
  if (tecla == 'z') track=0x0c;
  if (tecla == '1') track=0x0d;
  if (tecla == 'x') track=0x0e;
  if (tecla == '2') track=0x0f;
  if (tecla == 'c') track=0x10;
  if (tecla == 'v') track=0x11;
  if (tecla == '3') track=0x12;
  if (tecla == 'b') track=0x13;
  if (tecla == '4') track=0x14;
  if (tecla == 'n') track=0x15;
  if (tecla == '5') track=0x16;
  if (tecla == 'm') track=0x17;
  if (tecla == 'a') track=0x18;
  if (tecla == '6') track=0x19;
  if (tecla == 's') track=0x1a;
  if (tecla == '7') track=0x1b;
  if (tecla == 'd') track=0x1c;
  if (tecla == 'f') track=0x1d;
  if (tecla == '8') track=0x1e;
  if (tecla == 'g') track=0x1f;
  if (tecla == '9') track=0x20;
  if (tecla == 'h') track=0x21;
  if (tecla == '0') track=0x22;
  if (tecla == 'j') track=0x23;
  if (tecla == 'q') track=0x24;
  if (tecla == 'i') track=0x01;
  if (tecla == 'w') track=0x02;
  if (tecla == 'o') track=0x03;
  if (tecla == 'e') track=0x04;
  if (tecla == 'r') track=0x05;
  if (tecla == 'p') track=0x06;
  if (tecla == 't') track=0x07;
  if (tecla == 'k') track=0x08;
  if (tecla == 'y') track=0x09;
  if (tecla == 'l') track=0x0a;
  if (tecla == 'u') track=0x0b;

  return track;
}

byte traduzVolume(int value)
{
    byte volume;
    if(value == 1) volume=0x01;
    if(value == 2) volume=0x02;
    if(value == 3) volume=0x03;
    if(value == 4) volume=0x04;
    if(value == 5) volume=0x05;
    if(value == 6) volume=0x06;
    if(value == 7) volume=0x07;
    if(value == 8) volume=0x08;
    if(value == 9) volume=0x09;
    if(value == 10) volume=0x0a;
    if(value == 11) volume=0x0b;
    if(value == 12) volume=0x0c;
    if(value == 13) volume=0x0d;
    if(value == 14) volume=0x0e;
    if(value == 15) volume=0x0f;
    if(value == 16) volume=0x10;
    if(value == 17) volume=0x11;
    if(value == 18) volume=0x12;
    if(value == 19) volume=0x13;
    if(value == 20) volume=0x14;
    if(value == 21) volume=0x15;
    if(value == 22) volume=0x16;
    if(value == 23) volume=0x17;
    if(value == 24) volume=0x18;
    if(value == 25) volume=0x19;
    if(value == 26) volume=0x1a;
    if(value == 27) volume=0x1b;
    if(value == 28) volume=0x1c;
    if(value == 29) volume=0x1d;
    if(value == 30) volume=0x1e;

    return value;
}
