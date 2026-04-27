#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

/*
 * Función auxiliar para limpiar la pantalla según el sistema operativo.
 */
void limpiar_pantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/*
 * Función auxiliar para pausar el juego y esperar un Enter.
 */
void esperar_enter() {
    printf("\nPresioná ENTER para continuar...");
    // Limpiamos el buffer por si quedaron caracteres de jugadas anteriores
    while (getchar() != '\n'); 
}

int main() {
    srand((unsigned)time(NULL));
    juego_t juego;
    char jugada;
    int resultado_lectura;

    inicializar_juego(&juego);

    while (estado_juego(juego) == 0) {
        
        limpiar_pantalla();

        printf("====================================================\n");
        printf(" RITO DE ASCENSIÓN DE LOS MAGIOS - NIVEL %d\n", juego.nivel_actual);
        printf("====================================================\n");
        
        // Usamos los campos del struct respetando que son int
        printf(" Vidas: %d | Hechizos: %d | Antorchas: %d\n", 
               juego.homero.vidas_restantes, 
               juego.homero.hechizos_reveladores, 
               juego.homero.antorchas);
        
        if (juego.homero.recolecto_pergamino) {
            printf(" [!] ¡TENÉS EL PERGAMINO! Buscá el Altar ('A').\n");
        } else {
            printf(" [?] Buscando el Pergamino...\n");
        }
        printf("----------------------------------------------------\n\n");

        mostrar_juego(juego);

        // Verificamos si ganó el nivel
        if (estado_nivel(juego.niveles[juego.nivel_actual - 1], juego.homero) == 1) {
            if (juego.nivel_actual < MAX_NIVELES) {
                printf("\n¡Excelente! Superaste el nivel %d.\n", juego.nivel_actual);
                cambiar_nivel(&juego);
                esperar_enter();
                continue; 
            }
        }

        printf("\nControles: [W,A,S,D] Mover | [H] Hechizo | [L] Antorcha\n");
        printf("Tu jugada: ");
        
        // Manejo estricto de lectura para evitar advertencias de -Wunused-result
        resultado_lectura = scanf(" %c", &jugada);
        if (resultado_lectura != 1) {
            printf("Error en la lectura de jugada.\n");
            return -1;
        }

        realizar_jugada(&juego, jugada);
    }

    limpiar_pantalla();
    int resultado_final = estado_juego(juego);

    if (resultado_final == 1) {
        printf("¡FELICITACIONES! Homero ha completado el Rito.\n");
        printf("Ahora es un miembro oficial de los Magios. ¡A celebrar!\n");
    } else {
        printf("GAME OVER\n");
        printf("Homero no ha sido capaz de superar las pruebas.\n");
    }

    return 0;
}