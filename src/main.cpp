#include <Arduino.h>
#include <Keyboard.h>
#include <Keypad.h>
#include <Adafruit_VS1053.h>
#include <MIDI.h>
#include "VS1053.h"
#include "configmode.h"
#include "keyboardconfig.h"


byte c = 0;
char leitura_teclas;

char tecla = 0;
byte nota = 0;

// This is required to set up the MIDI library.
// The default MIDI setup uses the built-in serial port.
MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
    // Interrupcao
    //  Configuração do timer1
    TCCR1A = 0;                          // confira timer para operação normal pinos OC1A e OC1B desconectados
    TCCR1B = 0;                          // limpa registrador
    TCCR1B |= (1 << CS10) | (1 << CS12); // configura prescaler para 1024: CS12 = 1 e CS10 = 1

    TCNT1 = 0xFF9F; // incia timer com valor para que estouro ocorra em 1 segundo
                    // 65536-(16MHz/1024/1Hz) = 65.146 = 0xFF9F

    TIMSK1 |= (1 << TOIE1); // habilita a interrupção do TIMER1

    Keyboard.begin();

    initialiseVS1053();

    MIDI.begin(MIDI_CHANNEL_OMNI);
    talkMIDI(0xb0, 0x0b, 127);

    midiSetChannelVolume(0, volume);

    delay(100);

    toca_midi((byte)72);
    delay(300);
    toca_midi((byte)76);
    delay(300);
    toca_midi((byte)79);

    pinMode (PD4 , OUTPUT);
    digitalWrite (PD4, LOW);
}
void loop()
{
    if (leitura_teclas)
    {
        for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
        {
            if (teclado_personalizado.key[i].stateChanged) // Only find keys that have changed state.
            {
//                uint8_t state = teclado_personalizado.key[i].kstate;
//                if (state > 0)
//                {
//                    Serial.print(traduzNota(teclado_personalizado.key[i].kchar));
//                    Serial.println(state);
//                }

                if (teclado_personalizado.key[i].kstate == PRESSED)
                {
                    Keyboard.print(teclado_personalizado.key[i].kchar);
                    tecla = teclado_personalizado.key[i].kchar;
                    // if(tecla != 0x31)
                    // {
                    nota = traduzNota(tecla);
                    toca_midi(nota);
                    // }
                    // else
                    // {
                    //     flagvolume = 1;
                    // }

                    if (tecla == 0x70)
                        flagProg01 = 1;
//                        Serial.println(nota);
                    if (tecla == 0x6b)
                        flagProg02 = 1;
//                        Serial.println(nota);
                    if (tecla == 0x6c)
                        flagProg03 = 1;
//                        Serial.println(nota);
                }
                if (teclado_personalizado.key[i].kstate == RELEASED)
                {
                    tecla = teclado_personalizado.key[i].kchar;
                    nota = traduzNota(tecla);
                    // if(tecla == 0x31 && flagvolume){
                    //     if(volume <= 30){
                    //         volume = 127;
                    //         midiSetChannelVolume(0, volume);
                    //         toca_midi(36);
                    //         delay(300);
                    //         para_midi(36);
                    //     }
                    //     else{
                    //         tecla = 0;
                    //         volume -= 10;
                    //         midiSetChannelVolume(0, volume);
                    //         toca_midi(71);
                    //         delay(300);
                    //         para_midi(71);
                    //     }
                    //     flagvolume = 0;
                    // } else{
                    if (tecla == 0x70)
                        flagProg01 = 0;
                    if (tecla == 0x6b)
                        flagProg02 = 0;
                    if (tecla == 0x6c)
                        flagProg03 = 0;
//                    Serial.print("parou");
//                    Serial.println(tecla);
                    para_midi(nota);
                    // }
                }
            }
            //   delay(10);
        }
    }
    handleConfigMode();
}

ISR(TIMER1_OVF_vect) // interrupção do TIMER1
{
    if (prg == 1)
        return;
    TCNT1 = 0xFF9F; // Renicia TIMER
    leitura_teclas = teclado_personalizado.getKeys();
}