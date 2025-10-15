#include <stdio.h>
#include <stdlib.h>

// Board size is 10000 x 10000
#define X 10000
#define Y 10000

static unsigned char seed;

static inline char myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>16) & 1;
}

// Coloca valores aleatorios
void inicializar_tablero ( char T[Y+2][X+2] )
{
  int i, j;
  for ( i= 0; i < X+2; i++ )
    for ( j= 0; j < Y+2; j++ )
      T[i][j] = myRandom();
}

void ajustar_marco( char T[Y+2][X+2] )
{
  int i;
  for ( i=0; i < X+2; i++ )  T[0][i] = T[Y+1][i] = 0;
  for ( i=0; i < Y+2; i++ )  T[i][0] = T[i][X+1] = 0;
}

void copiar_tablero ( char in[Y+2][X+2], char out[Y+2][X+2] )
{
  int i,j;
  
  for ( i=0; i < X+2; i++ )
    for ( j=0; j < Y+2; j++ )
      out[i][j] = in[i][j];
}


int evaluar_celda ( char tablero[Y+2][X+2], int fila, int columna )
{
  int vecinos, i, j;
  //int YO =  tablero[fila][columna];

  vecinos = -tablero[fila][columna];
  // contar celdas vivas (con un 1) en la vecindad del elemento (fila,columna) 
  //   incluyendo a la celda central
  for ( i= -1 ; i < 2 ; i++ )
    for ( j= -1 ; j < 2 ; j++ )
        vecinos += tablero[fila+i][columna+j];

  return (vecinos == 2 || vecinos == 3);
}

// Tablero del Juego de la VIDA
static char tablero[Y+2][X+2];

// Se usa para preservar valores durante el computo
static char tablero_auxiliar[Y+2][X+2];

int actualizar_tablero ( char tablero_original[Y+2][X+2] )
{
  int count= 0, old, new, i,j;

  copiar_tablero ( tablero_original, tablero_auxiliar );
  
  
  for ( i=1; i < X+1; i++ )
    for ( j=1; j < Y+1; j++ )
    {
      old = tablero_auxiliar[i][j];
      new = evaluar_celda (tablero_auxiliar, i, j);
      tablero_original[i][j] = new;
      count += old != new;
    }
  return count; // numero de celdas que han cambiado
}

int JuegoDeLaVida ( int time )
{
  int cambios;

  // inicializar de forma aleatoria
  inicializar_tablero ( tablero );
  ajustar_marco       ( tablero );

  while (time > 0)
  {
    cambios = actualizar_tablero(tablero);
    time = time - 1;
  }
  return cambios;
}

int main (int argc, char **argv)
{
  int time_steps = 30; // Default number of time steps

  seed = 0; // Defined statically. By default, random seed is 0

  // obtener argumentos proporcionados en tiempo de ejecucion
  if (argc>1) {  time_steps = atoi(argv[1]); }
  if (argc>2) {  seed       = atoi(argv[2]); } 

  printf( "Matrix is %d x %d.\nNumber of iterations: %d\n", Y, X, time_steps);

  int chk = JuegoDeLaVida ( time_steps );

  printf("CheckSum = %d\n", chk);

  return (0);
}
