#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

int main() {
    srand((unsigned)time(NULL));
    juego_t juego;
    char movimiento;

    // 1. Inicialización completa
    // (Esto ya carga el Nivel 1 y posiciona a Homero)
    inicializar_juego(&juego);

    // Bucle principal del juego
    // El juego sigue mientras no se gane (estado 1) y no se pierda (estado -1)
    while (estado_juego(juego) == 0) {
        
        // Limpiar pantalla en cada turno para que parezca un juego fluido
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

        // 2. Mostrar el juego (Visualización)
        // Esta función es la que dibuja la matriz de 20x30 con los caracteres.
        mostrar_juego(juego);

        // 3. Verificar si el nivel actual se ganó para cambiar de nivel
        // Si el estado del nivel es 1 (ganado), llamamos a cambiar_nivel
        if (estado_nivel(juego.niveles[juego.nivel_actual - 1], juego.homero) == 1) {
            cambiar_nivel(&juego);
            printf("\n ¡Felicidades! Pasaste al nivel %d. Presioná una tecla para continuar...", juego.nivel_actual);
            getchar(); // Pausa para que el usuario vea el mensaje
        }

        printf("\nIngrese movimiento (W, A, S, D) o acción (H, L): ");
        if (scanf(" %c", &movimiento) != 1) return 1;

        // 4. Ejecutar la jugada
        // Aquí adentro es donde ahora se ejecutan las colisiones con piedras,
        // tótems, el pergamino y la lógica de la catapulta.
        realizar_jugada(&juego, movimiento);
    }

    // 5. Finalización
    printf("\033[H\033[J"); 
    if (estado_juego(juego) == 1) {
        printf("¡EXCELENTE TRABAJO! Homero ha ascendido a los Magios.\n");
    } else {
        printf("¡GAME OVER! Homero no fue digno de los Magios.\n");
    }
    
    return 0;
}