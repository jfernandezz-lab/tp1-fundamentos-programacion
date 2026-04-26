#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

int main() {
    srand((unsigned)time(NULL));
    juego_t juego;
    char movimiento;

    // Limpieza e Inicialización
    printf("\033[H\033[J"); 
    inicializar_juego(&juego);

    printf("--- Rito de Ascensión de los Magios ---\n");

    // Bucle principal de la Etapa 2
    while (juego.homero.vidas_restantes > 0) {
        printf("\nPosición actual: [%d, %d] | Vidas: %d\n", 
               juego.homero.posicion.fil, juego.homero.posicion.col, juego.homero.vidas_restantes);
        
        printf("Ingrese movimiento (W, A, S, D): ");
        scanf(" %c", &movimiento); // El espacio antes de %c limpia el buffer del teclado

        realizar_jugada(&juego, movimiento);
    }

    printf("\n¡GAME OVER! Te has quedado sin vidas.\n");
    
    return 0;
}