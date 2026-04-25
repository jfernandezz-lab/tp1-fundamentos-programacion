#include <stdlib.h>
#include "ascension_magios.h"
#include "utiles.h"

void inicializar_juego(juego_t* juego) {
    // Por ahora, solo aseguramos que el puntero no sea nulo 
    // y definimos el nivel de arranque para el Paso 1.
    if (juego != NULL) {
        juego->nivel_actual = 1;
    }
}