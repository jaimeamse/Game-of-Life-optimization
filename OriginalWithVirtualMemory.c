#include <stdio.h>
#include <stdlib.h>

#define X 100000
#define Y 10000

static unsigned int seed;

static inline int myRandom() {
    seed = (214013 * seed + 2531011);
    return (seed >> 16) & 1;
}

/* Inicializa tablero */
void inicializar_tablero(int *T) {
    for (int j = 0; j < X + 2; j++)
        for (int i = 0; i < Y + 2; i++)
            T[i*(X+2) + j] = myRandom();
}

/* Ajusta marco */
void ajustar_marco(int *T) {
    for (int i = 0; i < X + 2; i++) {
        T[0*(X+2) + i]     = 0;
        T[(Y+1)*(X+2) + i] = 0;
    }
    for (int i = 0; i < Y + 2; i++) {
        T[i*(X+2) + 0]     = 0;
        T[i*(X+2) + X+1]   = 0;
    }
}

/* Copia tablero */
void copiar_tablero(int *in, int *out) {
    for (int j = 0; j < X + 2; j++)
        for (int i = 0; i < Y + 2; i++)
            out[i*(X+2) + j] = in[i*(X+2) + j];
}

/* Evalúa una celda */
int evaluar_celda(int *tablero, int fila, int columna) {
    int vecinos = 0;
    int YO = tablero[fila*(X+2) + columna];

    for (int j = -1; j <= 1; j++)
        for (int i = -1; i <= 1; i++)
            if (tablero[(fila+i)*(X+2) + (columna+j)] == 1)
                vecinos++;

    vecinos -= YO;

    if (YO == 1 && vecinos < 2) return 0;
    if (YO == 1 && (vecinos == 2 || vecinos == 3)) return 1;
    if (YO == 1 && vecinos > 3) return 0;
    if (YO == 0 && vecinos == 3) return 1;

    return 0;
}

/* Actualiza tablero */
int actualizar_tablero(int *tablero, int *aux) {
    int count = 0;

    copiar_tablero(tablero, aux);

    for (int j = 1; j < X + 1; j++)
        for (int i = 1; i < Y + 1; i++) {
            int old = aux[i*(X+2) + j];
            int neW = evaluar_celda(aux, i, j);
            tablero[i*(X+2) + j] = neW;
            if (old != neW) count++;
        }
    return count;
}

int JuegoDeLaVida(int time) {
    size_t size = (X + 2) * (Y + 2);

    int *tablero = malloc(size * sizeof(int));
    int *aux     = malloc(size * sizeof(int));

    if (!tablero || !aux) {
        fprintf(stderr, "Error reservando memoria\n");
        exit(EXIT_FAILURE);
    }

    inicializar_tablero(tablero);
    ajustar_marco(tablero);

    int cambios = 0;
    while (time-- > 0)
        cambios = actualizar_tablero(tablero, aux);

    free(tablero);
    free(aux);

    return cambios;
}

int main(int argc, char **argv) {
    int time_steps = 30;
    seed = 0;

    if (argc > 1) time_steps = atoi(argv[1]);
    if (argc > 2) seed = atoi(argv[2]);

    printf("Matrix is %d x %d.\nNumber of iterations: %d\n", Y, X, time_steps);

    int chk = JuegoDeLaVida(time_steps);
    printf("CheckSum = %d\n", chk);

    return 0;
}
