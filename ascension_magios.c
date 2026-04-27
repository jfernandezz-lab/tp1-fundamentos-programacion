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

#define VIDAS_INICIALES 5
#define HECHIZOS_INICIALES 5
#define ANTORCHAS_POR_NIVEL 5
#define RADIO_ANTORCHA 3
#define CANTIDAD_TOTEMS 5
#define CANTIDAD_PIEDRAS 10

// --- FUNCIONES AUXILIARES PRIVADAS ---

static bool buscar_coordenada(coordenada_t objetivo, coordenada_t vector[], int tope) {
    for (int i = 0; i < tope; i++) {
        if (vector[i].fil == objetivo.fil && vector[i].col == objetivo.col) return true;
    }
    return false;
}

static int distancia_manhattan(coordenada_t p1, coordenada_t p2) {
    int diff_fil = p1.fil - p2.fil;
    int diff_col = p1.col - p2.col;
    if (diff_fil < 0) diff_fil *= -1;
    if (diff_col < 0) diff_col *= -1;
    return diff_fil + diff_col;
}

static bool es_visible(juego_t juego, int f, int c) {
    coordenada_t actual = {f, c};
    nivel_t nivel = juego.niveles[juego.nivel_actual - 1];
    if (juego.camino_visible) return true;
    if (juego.homero.antorcha_encendida && distancia_manhattan(juego.homero.posicion, actual) <= RADIO_ANTORCHA) return true;
    if (juego.homero.posicion.fil == nivel.camino[0].fil && juego.homero.posicion.col == nivel.camino[0].col) return true;
    return false;
}

static void lanzar_catapulta(juego_t *juego) {
    nivel_t* nivel = &(juego->niveles[juego->nivel_actual - 1]);
    coordenada_t bola;
    bool valida = false;
    while (!valida) {
        bola.fil = rand() % MAX_FILAS;
        bola.col = rand() % MAX_COLUMNAS;
        if (!buscar_coordenada(bola, nivel->paredes, nivel->tope_paredes) &&
            !(bola.fil == nivel->camino[0].fil && bola.col == nivel->camino[0].col) &&
            !(bola.fil == nivel->camino[nivel->tope_camino-1].fil && bola.col == nivel->camino[nivel->tope_camino-1].col) &&
            !(bola.fil == juego->homero.posicion.fil && bola.col == juego->homero.posicion.col)) {
            valida = true;
        }
    }
    for (int i = 0; i < nivel->tope_camino; i++) {
        if (nivel->camino[i].fil == bola.fil && nivel->camino[i].col == bola.col) {
            for (int j = i; j < nivel->tope_camino - 1; j++) nivel->camino[j] = nivel->camino[j+1];
            nivel->tope_camino--;
            break;
        }
    }
}

// --- IMPLEMENTACIÓN DE LA BIBLIOTECA ---

void inicializar_juego(juego_t *juego) {
    if (!juego) return;
    juego->nivel_actual = 1;
    juego->tope_niveles = MAX_NIVELES;
    juego->camino_visible = false;
    juego->homero.vidas_restantes = VIDAS_INICIALES;
    juego->homero.hechizos_reveladores = HECHIZOS_INICIALES;
    juego->homero.antorchas = ANTORCHAS_POR_NIVEL;
    juego->homero.recolecto_pergamino = false;
    juego->homero.antorcha_encendida = false;

    for (int i = 0; i < MAX_NIVELES; i++) {
        nivel_t* nivel = &(juego->niveles[i]);
        obtener_mapa(nivel->paredes, &(nivel->tope_paredes), nivel->camino, &(nivel->tope_camino), i + 1);
        if (i == 0) juego->homero.posicion = nivel->camino[0];
        nivel->pergamino = nivel->camino[rand() % nivel->tope_camino];

        nivel->tope_herramientas = 0;
        while(nivel->tope_herramientas < CANTIDAD_TOTEMS) {
            coordenada_t pos = {rand() % MAX_FILAS, rand() % MAX_COLUMNAS};
            if (!buscar_coordenada(pos, nivel->paredes, nivel->tope_paredes) && !buscar_coordenada(pos, nivel->camino, nivel->tope_camino)) {
                nivel->herramientas[nivel->tope_herramientas].tipo = HERRAMIENTA_TOTEM;
                nivel->herramientas[nivel->tope_herramientas].posicion = pos;
                nivel->tope_herramientas++;
            }
        }

        nivel->tope_obstaculos = 0;
        while(nivel->tope_obstaculos < CANTIDAD_PIEDRAS) {
            coordenada_t pos = {rand() % MAX_FILAS, rand() % MAX_COLUMNAS};
            if (!buscar_coordenada(pos, nivel->paredes, nivel->tope_paredes) && !buscar_coordenada(pos, nivel->camino, nivel->tope_camino)) {
                nivel->obstaculos[nivel->tope_obstaculos].tipo = OBSTACULO_PIEDRA;
                nivel->obstaculos[nivel->tope_obstaculos].posicion = pos;
                nivel->tope_obstaculos++;
            }
        }
        coordenada_t pos_f = {rand() % MAX_FILAS, rand() % MAX_COLUMNAS};
        while(buscar_coordenada(pos_f, nivel->paredes, nivel->tope_paredes) || buscar_coordenada(pos_f, nivel->camino, nivel->tope_camino)) {
            pos_f.fil = rand() % MAX_FILAS; pos_f.col = rand() % MAX_COLUMNAS;
        }
        nivel->obstaculos[nivel->tope_obstaculos].tipo = OBSTACULO_CATAPULTA;
        nivel->obstaculos[nivel->tope_obstaculos].posicion = pos_f;
        nivel->tope_obstaculos++;
    }
}

void realizar_jugada(juego_t *juego, char movimiento) {
    nivel_t* nivel = &(juego->niveles[juego->nivel_actual - 1]);
    coordenada_t proxima_pos = juego->homero.posicion;
    bool movio = false;
    static int contador_hechizo = 0; // Para persistencia de 5 turnos

    // Reseteos obligatorios por turno
    juego->homero.antorcha_encendida = false;
    
    // Si quedan turnos de hechizo, mantenemos el camino visible
    if (contador_hechizo > 0) {
        juego->camino_visible = true;
    } else {
        juego->camino_visible = false;
    }

    if (movimiento == 'W' || movimiento == 'w') { proxima_pos.fil--; movio = true; }
    else if (movimiento == 'S' || movimiento == 's') { proxima_pos.fil++; movio = true; }
    else if (movimiento == 'A' || movimiento == 'a') { proxima_pos.col--; movio = true; }
    else if (movimiento == 'D' || movimiento == 'd') { proxima_pos.col++; movio = true; }
    else if ((movimiento == 'H' || movimiento == 'h') && juego->homero.hechizos_reveladores > 0) {
        juego->camino_visible = true;
        contador_hechizo = 5; // <--- Implementación de los 5 turnos
        juego->homero.hechizos_reveladores--;
        lanzar_catapulta(juego);
    }
    else if ((movimiento == 'L' || movimiento == 'l') && juego->homero.antorchas > 0) {
        juego->homero.antorcha_encendida = true;
        juego->homero.antorchas--;
    }

    if (movio) {
        // Si el jugador se movió, descontamos un turno del hechizo si estaba activo
        if (contador_hechizo > 0) contador_hechizo--;

        if (proxima_pos.fil >= 0 && proxima_pos.fil < MAX_FILAS && proxima_pos.col >= 0 && proxima_pos.col < MAX_COLUMNAS &&
            !buscar_coordenada(proxima_pos, nivel->paredes, nivel->tope_paredes)) {
            
            juego->homero.posicion = proxima_pos;

            // Runa: Lanzar catapulta si vuelve a pisarla y activar visibilidad momentánea
            if (juego->homero.posicion.fil == nivel->camino[0].fil && juego->homero.posicion.col == nivel->camino[0].col) {
                lanzar_catapulta(juego);
            }

            // Lógica de Piedras
            for (int i = 0; i < nivel->tope_obstaculos; i++) {
                if (nivel->obstaculos[i].tipo == OBSTACULO_PIEDRA && 
                    nivel->obstaculos[i].posicion.fil == juego->homero.posicion.fil && 
                    nivel->obstaculos[i].posicion.col == juego->homero.posicion.col) {
                    nivel->pergamino = nivel->camino[rand() % nivel->tope_camino];
                    juego->homero.recolecto_pergamino = false;
                    nivel->obstaculos[i] = nivel->obstaculos[nivel->tope_obstaculos - 1];
                    nivel->tope_obstaculos--;
                }
            }

            // Lógica de Pergamino: Visible hasta el próximo movimiento
            if (juego->homero.posicion.fil == nivel->pergamino.fil && 
                juego->homero.posicion.col == nivel->pergamino.col && !juego->homero.recolecto_pergamino) {
                juego->homero.recolecto_pergamino = true;
                juego->camino_visible = true;
            }

            // Lógica de Tótems (+1 vida)
            for (int i = 0; i < nivel->tope_herramientas; i++) {
                if (nivel->herramientas[i].posicion.fil == juego->homero.posicion.fil && 
                    nivel->herramientas[i].posicion.col == juego->homero.posicion.col) {
                    juego->homero.vidas_restantes++;
                    nivel->herramientas[i] = nivel->herramientas[nivel->tope_herramientas - 1];
                    nivel->tope_herramientas--;
                }
            }

            // Penalización por pisar fuera del camino
            if (!buscar_coordenada(juego->homero.posicion, nivel->camino, nivel->tope_camino)) {
                juego->homero.vidas_restantes--;
                juego->homero.posicion = nivel->camino[0];
                contador_hechizo = 0; // Al morir o fallar se pierde el hechizo
            }
        }
    }
}

void mostrar_juego(juego_t juego) {
    nivel_t nivel = juego.niveles[juego.nivel_actual - 1];
    char matriz[MAX_FILAS][MAX_COLUMNAS];
    for (int f = 0; f < MAX_FILAS; f++) {
        for (int c = 0; c < MAX_COLUMNAS; c++) matriz[f][c] = ' ';
    }
    for (int i = 0; i < nivel.tope_paredes; i++) matriz[nivel.paredes[i].fil][nivel.paredes[i].col] = 'X';
    for (int i = 0; i < nivel.tope_camino; i++) {
        if (es_visible(juego, nivel.camino[i].fil, nivel.camino[i].col))
            matriz[nivel.camino[i].fil][nivel.camino[i].col] = 'c';
    }
    for (int i = 0; i < nivel.tope_herramientas; i++) {
        if (es_visible(juego, nivel.herramientas[i].posicion.fil, nivel.herramientas[i].posicion.col))
            matriz[nivel.herramientas[i].posicion.fil][nivel.herramientas[i].posicion.col] = nivel.herramientas[i].tipo;
    }
    for (int i = 0; i < nivel.tope_obstaculos; i++) {
        if (es_visible(juego, nivel.obstaculos[i].posicion.fil, nivel.obstaculos[i].posicion.col))
            matriz[nivel.obstaculos[i].posicion.fil][nivel.obstaculos[i].posicion.col] = nivel.obstaculos[i].tipo;
    }
    if (!juego.homero.recolecto_pergamino && es_visible(juego, nivel.pergamino.fil, nivel.pergamino.col))
        matriz[nivel.pergamino.fil][nivel.pergamino.col] = HERRAMIENTA_PERGAMINO;

    matriz[nivel.camino[0].fil][nivel.camino[0].col] = RUNAS;
    matriz[nivel.camino[nivel.tope_camino-1].fil][nivel.camino[nivel.tope_camino-1].col] = ALTARES;
    matriz[juego.homero.posicion.fil][juego.homero.posicion.col] = 'H';

    for (int f = 0; f < MAX_FILAS; f++) {
        for (int c = 0; c < MAX_COLUMNAS; c++) printf("%c", matriz[f][c]);
        printf("\n");
    }
    printf("\nLEYENDA: H: Homero | U: Runa | A: Altar | c: Camino | X: Pared | P: Pergamino | T: Tótem | R: Piedra | F: Catapulta\n");
}

int estado_nivel(nivel_t nivel, personaje_t homero) {
    coordenada_t altar = nivel.camino[nivel.tope_camino - 1];
    if (homero.posicion.fil == altar.fil && homero.posicion.col == altar.col && homero.recolecto_pergamino) {
        return 1;
    }
    return 0;
}

int estado_juego(juego_t juego) {
    if (juego.homero.vidas_restantes <= 0) return -1;
    if (juego.nivel_actual == MAX_NIVELES && estado_nivel(juego.niveles[MAX_NIVELES-1], juego.homero) == 1) return 1;
    return 0;
}

void cambiar_nivel(juego_t* juego) {
    if (juego->nivel_actual < MAX_NIVELES) {
        juego->nivel_actual++;
        juego->homero.posicion = juego->niveles[juego->nivel_actual - 1].camino[0];
        juego->homero.recolecto_pergamino = false;
        juego->homero.antorcha_encendida = false;
        juego->camino_visible = false;
        juego->homero.antorchas = ANTORCHAS_POR_NIVEL;
    }
}