#ifndef CONFIGMODE_H
#define CONFIGMODE_H

#include <Arduino.h>
#include "VS1053.h"
#include "keyboardconfig.h"

extern unsigned long timeProg;
extern uint8_t flagvolume;
extern uint8_t flagProg01;
extern uint8_t flagProg02;
extern uint8_t flagProg03;
extern uint8_t volume;
extern uint8_t prg;

void tocaErro();
void tocaSucesso(uint8_t nivel);
void handleConfigMode();

#endif // CONFIGMODE_H
