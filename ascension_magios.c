#include <stdlib.h>
#include <stdbool.h>
#include "ascension_magios.h"
#include "utiles.h"

// --- FUNCIONES DE BÚSQUEDA (Tema 2 del Cronograma) ---

/*
 * Post: Devuelve true si la coordenada existe en el vector, false de lo contrario.
 */
static bool buscar_coordenada(coordenada_t objetivo, coordenada_t vector[], int tope) {
    int i = 0;
    bool encontrado = false;
    while (i < tope && !encontrado) {
        if (vector[i].fil == objetivo.fil && vector[i].col == objetivo.col) {
            encontrado = true;
        }
        i++;
    }
    return encontrado;
}

// --- IMPLEMENTACIÓN (Etapa 2) ---

void inicializar_juego(juego_t* juego) {
    if (juego == NULL) return; // Validación de puntero

    juego->nivel_actual = 1;
    juego->camino_visible = false;
    
    // Configuración inicial de Homero (Paso 1)
    juego->homero.vidas_restantes = 5;
    juego->homero.hechizos_reveladores = 5;
    juego->homero.antorchas = 5;
    juego->homero.recolecto_pergamino = false;

    // Carga de datos mediante la biblioteca externa
    obtener_mapa(juego->niveles[0].paredes, &(juego->niveles[0].tope_paredes),
                 juego->niveles[0].camino, &(juego->niveles[0].tope_camino), 1);

    // Posición inicial: Inicio del camino
    juego->homero.posicion = juego->niveles[0].camino[0];
}

void realizar_jugada(juego_t* juego, char movimiento) {
    if (juego == NULL) return;

    nivel_t* nivel_actual = &(juego->niveles[juego->nivel_actual - 1]);
    coordenada_t proxima_pos = juego->homero.posicion;

    // 1. Capturar dirección (Tema: Lógica y Buenas Prácticas)
    if (movimiento == 'W' || movimiento == 'w') proxima_pos.fil--;
    else if (movimiento == 'S' || movimiento == 's') proxima_pos.fil++;
    else if (movimiento == 'A' || movimiento == 'a') proxima_pos.col--;
    else if (movimiento == 'D' || movimiento == 'd') proxima_pos.col++;
    else return;

    // 2. Validar Matriz (20x30) y Paredes
    if (proxima_pos.fil >= 0 && proxima_pos.fil < 20 && 
        proxima_pos.col >= 0 && proxima_pos.col < 30) {
        
        if (!buscar_coordenada(proxima_pos, nivel_actual->paredes, nivel_actual->tope_paredes)) {
            juego->homero.posicion = proxima_pos;

            // 3. Gestión de Vidas (Búsqueda Lineal en el camino)
            if (!buscar_coordenada(juego->homero.posicion, nivel_actual->camino, nivel_actual->tope_camino)) {
                juego->homero.vidas_restantes--;
                juego->homero.posicion = nivel_actual->camino[0]; // Vuelve al inicio
            }
        }
    }
}