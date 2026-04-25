#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

int main() {
    srand((unsigned)time(NULL));
    juego_t juego;

    // Limpieza de pantalla inicial
    printf("\033[H\033[J"); 
    printf("--- Rito de Ascensión de los Magios ---\n");

    // Inicialización del entorno (Paso 1)
    inicializar_juego(&juego);

    printf("Paso 1: Entorno configurado correctamente.\n");
    
    return 0;
}