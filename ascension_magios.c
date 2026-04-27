#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ascension_magios.h"
#include "utiles.h"

#define OBSTACULO_PIEDRA 'R'
#define OBSTACULO_CATAPULTA 'F'
#define HERRAMIENTA_TOTEM 'T'
#define HERRAMIENTA_PERGAMINO 'P'
#define ALTARES 'A'
#define RUNAS 'U'

// --- FUNCIONES AUXILIARES PRIVADAS (ETAPA 3) ---

/*
 * Post: Desplaza los elementos del vector camino hacia la izquierda desde el indice dado
 * y decrementa el tope. (Algoritmo de eliminación manteniendo el orden).
 */
static void eliminar_baldosa_camino(coordenada_t camino[MAX_CAMINO], int* tope_camino, int indice_a_borrar) {
    for (int i = indice_a_borrar; i < (*tope_camino) - 1; i++) {
        camino[i] = camino[i + 1];
    }
    (*tope_camino)--;
}

/*
 * Post: Mueve el pergamino a una posición aleatoria del camino que no sea ni la Runa ni el Altar.
 */
static void relocalizar_pergamino(juego_t* juego) {
    nivel_t* nivel = &(juego->niveles[juego->nivel_actual - 1]);
    int indice_random;
    bool posicion_valida = false;

    while (!posicion_valida) {
        indice_random = rand() % nivel->tope_camino;
        coordenada_t posible_pos = nivel->camino[indice_random];

        // Validamos que no sea la Runa (inicio) ni el Altar (final)
        // El enunciado implica que Runa y Altar están en los extremos del vector camino.
        if (indice_random != 0 && indice_random != (nivel->tope_camino - 1)) {
            nivel->pergamino = posible_pos;
            posicion_valida = true;
        }
    }
}

/*
 * Post: Procesa la colisión de Homero con herramientas (Tótem, Pergamino).
 */
static void verificar_herramientas(juego_t* juego) {
    nivel_t* nivel = &(juego->niveles[juego->nivel_actual - 1]);
    
    // 1. Verificar Pergamino
    if (juego->homero.posicion.fil == nivel->pergamino.fil && 
        juego->homero.posicion.col == nivel->pergamino.col) {
        juego->homero.recolecto_pergamino = true;
    }

    // 2. Verificar Tótems (Búsqueda lineal en el vector de herramientas)
    for (int i = 0; i < nivel->tope_herramientas; i++) {
        if (juego->homero.posicion.fil == nivel->herramientas[i].posicion.fil &&
            juego->homero.posicion.col == nivel->herramientas[i].posicion.col) {
            
            if (nivel->herramientas[i].tipo == HERRAMIENTA_TOTEM) {
                juego->homero.vidas_restantes++;
                // Eliminar tótem para que no se use infinitamente (Swap con el último)
                nivel->herramientas[i] = nivel->herramientas[nivel->tope_herramientas - 1];
                (nivel->tope_herramientas)--;
            }
        }
    }
}

/*
 * Post: Procesa la colisión con obstáculos (Piedras) y la lógica de la Catapulta.
 */
static void verificar_obstaculos(juego_t* juego) {
    nivel_t* nivel = &(juego->niveles[juego->nivel_actual - 1]);

    // 1. Piedra del Castigo
    for (int i = 0; i < nivel->tope_obstaculos; i++) {
        if (nivel->obstaculos[i].tipo == OBSTACULO_PIEDRA &&
            juego->homero.posicion.fil == nivel->obstaculos[i].posicion.fil &&
            juego->homero.posicion.col == nivel->obstaculos[i].posicion.col) {
            
            relocalizar_pergamino(juego);
            // La piedra se rompe y desaparece
            nivel->obstaculos[i] = nivel->obstaculos[nivel->tope_obstaculos - 1];
            (nivel->tope_obstaculos)--;
        }
    }

    // 2. Lógica de Catapulta (si está activa por Runa o Hechizo)
    // Nota: Esta lógica se dispara según el estado del juego. 
    // Aquí implementamos el efecto de "borrar baldosa".
    if (juego->camino_visible) { // Simplificación: si el camino se ve, la catapulta puede tirar.
        int indice_a_borrar = rand() % nivel->tope_camino;
        // No borramos ni el inicio ni el final para no romper el juego
        if (indice_a_borrar != 0 && indice_a_borrar != (nivel->tope_camino - 1)) {
            eliminar_baldosa_camino(nivel->camino, &(nivel->tope_camino), indice_a_borrar);
        }
    }
}

// --- ACTUALIZACIÓN DE REALIZAR_JUGADA (ETAPA 3) ---

void realizar_jugada(juego_t* juego, char movimiento) {
    if (juego == NULL) return;

    nivel_t* nivel_actual = &(juego->niveles[juego->nivel_actual - 1]);
    coordenada_t proxima_pos = juego->homero.posicion;

    // Movimiento (W, A, S, D)
    if (movimiento == 'W' || movimiento == 'w') proxima_pos.fil--;
    else if (movimiento == 'S' || movimiento == 's') proxima_pos.fil++;
    else if (movimiento == 'A' || movimiento == 'a') proxima_pos.col--;
    else if (movimiento == 'D' || movimiento == 'd') proxima_pos.col++;
    else return;

    // Validación de límites y paredes
    if (proxima_pos.fil >= 0 && proxima_pos.fil < MAX_FILAS && 
        proxima_pos.col >= 0 && proxima_pos.col < MAX_COLUMNAS) {
        
        // Uso de la función estática del Paso 2
        // (Nota: Asegurate de que buscar_coordenada esté declarada arriba)
        // if (!buscar_coordenada(proxima_pos, nivel_actual->paredes, nivel_actual->tope_paredes)) { ... }
        
        // ... (Aquí iría tu código de movimiento de la Etapa 2) ...
        juego->homero.posicion = proxima_pos;

        // --- NUEVA LÓGICA ETAPA 3 ---
        verificar_herramientas(juego);
        verificar_obstaculos(juego);
        // ----------------------------
    }
}