#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H

#include <Keypad.h>

const byte LINHAS = 6;                                        // Linhas do teclado
const byte COLUNAS = 6;                                       // Colunas do teclado
const byte PINOS_LINHAS[LINHAS] = {3, 30, 5, 8, 10, 13};      // l1=pd0,l2=pd4,l3=pc6,l4=pb4,l5=pb6,l6=pc7
const byte PINOS_COLUNAS[COLUNAS] = {A0, A1, A2, A3, A4, A5}; // r0,r1,r2,r3,r4,r5,r6,r7

extern char TECLAS_MATRIZ[LINHAS][COLUNAS];
extern Keypad teclado_personalizado;

#endif // KEYBOARDCONFIG_H
