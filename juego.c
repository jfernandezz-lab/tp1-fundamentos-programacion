#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

int main() {
    srand((unsigned)time(NULL));
    juego_t juego;
    char movimiento;

    inicializar_juego(&juego);

    while (estado_juego(juego) == 0) {
        
        // Limpiar pantalla
        printf("\033[H\033[J"); 

        printf("--- Rito de Ascensión de los Magios ---\n");
        printf("Nivel: %d | Vidas: %d | Hechizos: %d | Antorchas: %d\n", 
               juego.nivel_actual, juego.homero.vidas_restantes, 
               juego.homero.hechizos_reveladores, juego.homero.antorchas);
        
        if (juego.homero.recolecto_pergamino) {
            printf("¡ESTADO: TENÉS EL PERGAMINO! Buscá el altar.\n");
        } else {
            printf("¡ESTADO: Buscando el pergamino...\n");
        }

        mostrar_juego(juego);

        if (estado_nivel(juego.niveles[juego.nivel_actual - 1], juego.homero) == 1) {
            if (juego.nivel_actual < MAX_NIVELES) {
                cambiar_nivel(&juego);
                printf("\n¡NIVEL COMPLETADO! Presioná ENTER para el siguiente...");
                while (getchar() != '\n'); // Limpia buffer previo
                getchar(); // Espera el Enter real
            }
        }

        printf("\nIngrese movimiento (W, A, S, D) o acción (H, L): ");
        if (scanf(" %c", &movimiento) != 1) return 1;
        while (getchar() != '\n'); // Limpia el \n del buffer del scanf

        realizar_jugada(&juego, movimiento);
    }

    printf("\033[H\033[J"); 
    if (estado_juego(juego) == 1) {
        printf("¡EXCELENTE TRABAJO! Homero ha ascendido a los Magios.\n");
    } else {
        printf("¡GAME OVER! Homero no fue digno de los Magios.\n");
    }
    
    return 0;
}