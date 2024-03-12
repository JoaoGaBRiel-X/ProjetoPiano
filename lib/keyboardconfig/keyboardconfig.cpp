#include <keyboardconfig.h>

Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS); // Inicia teclado

char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado TOMPLAY)
    {'c', 'm', '8', 'j', 'r', 'u'},
    {'x', 'n', 'd', 'h', 'e', 'l'},
    {'2', '5', 's', 'g', 'o', 'y'},
    {'z', 'b', '7', '0', 'w', 'k'},
    {'1', '4', 'a', 'f', 'i', 't'},
    {'3', 'v', '6', '9', 'q', 'p'}};
