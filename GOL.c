#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Board size is 10000 x 10000
#define X 20000
#define Y 20000

static unsigned int seed;

static inline unsigned char myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>16) & 1;
}

static inline unsigned char updateSeed() {
  seed = (214013*seed+2531011);
}

// Coloca valores aleatorios
void __attribute__((noinline)) inicializar_tablero ( unsigned char T[Y+2][X+2] )
{
  int i, j;
  unsigned copy = seed;
  //sumVal = 406518922;
  //mulVal = 1080739145;
  for ( i= 0; i < Y+2; i++ ) {
    copy = (214013*copy+2531011);
    seed = copy;
    for ( j= 0; j < X+2; j++ ) {
      T[i][j] = (seed>>16) & 1;
      seed = (1080739145*seed+406518922);
    }
  }
}

void __attribute__((noinline)) inicializar_tablero_comprimido ( unsigned char T[Y+2][X+2] )
{
  int i, j;
  unsigned copy = seed;
  //sumVal = 406518922;
  //mulVal = 1080739145;
  for ( i= 0; i < Y+2; i++ ) {
    copy = (214013*copy+2531011);
    seed = copy;
    for ( j= 0; j < X+2; j++) {
      for (int t = 0; t <= 7; t++) // si X es divisible entre 8
      {
        int randomVal = ((seed>>16) & 1);
        T[i][j] = randomVal + T[i][j]<<1;
        seed = (1080739145*seed+406518922);
      }
    }
  }
}


void ajustar_marco( unsigned char T[Y+2][X+2] )
{
  int i;
  for ( i=0; i < X+2; i++ )  T[0][i] = T[Y+1][i] = 0;
  for ( i=0; i < Y+2; i++ )  T[i][0] = T[i][X+1] = 0;
}

void __attribute__((noinline)) copiar_tablero ( unsigned char in[Y+2][X+2], unsigned char out[Y+2][X+2] )
{
  int i,j;
  for ( i=0; i < Y+2; i++ )
    for ( j=0; j < X+2; j++ )
      out[i][j] = in[i][j];
}

int evaluar_celda ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // VECTORIZADO
{
  int vecinos= -(tablero[fila][columna]);
  int i, j;
 
  // contar celdas vivas (con un 1) en la vecindad del elemento (fila,columna) 
  //   incluyendo a la celda central
  for ( j= -1 ; j < 2 ; j++ )
    for ( i= -1 ; i < 2 ; i++ )
      vecinos+= tablero[fila+i][columna+j];

  return ( ( vecinos == 2 && (tablero[fila][columna]) ) || vecinos == 3) ;
}

int evaluar_celda_2 ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // VECTORIZADO 
{
  int vecinos = ((tablero[fila-1][columna-1] + tablero[fila-1][columna]) 
                  + (tablero[fila-1][columna+1] + tablero[fila][columna-1]) ) 
                  + ( (tablero[fila][columna+1] + tablero[fila+1][columna-1]) 
                  + (tablero[fila+1][columna] + tablero[fila+1][columna+1]));
    
  int celda = tablero[fila][columna];
  return (vecinos == 3) || (vecinos == 2 && celda);
}

char evaluar_celda_XD ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // VECTORIZADO 
{
  char vecinos = ((tablero[fila-1][columna-1] + tablero[fila-1][columna]) 
                  + (tablero[fila-1][columna+1] + tablero[fila][columna-1]) ) 
                  + ( (tablero[fila][columna+1] + tablero[fila+1][columna-1]) 
                  + (tablero[fila+1][columna] + tablero[fila+1][columna+1]));
    
  char celda = tablero[fila][columna];
  return (vecinos == 3) || (vecinos == 2 && celda);
}


//static unsigned char buffer[2][X+2];
/*
void initBuffer(unsigned char tablero[Y+2][X+2], int j) {
  buffer[0][0] = tablero[j-1][0] + tablero[j][0];
  buffer[0][1] = tablero[j-1][1] + tablero[j][1];
  buffer[0][2] = tablero[j-1][2] + tablero[j][2];
  buffer[1][0] = tablero[j+1][0] + tablero[j][0];
  buffer[1][1] = tablero[j+1][1] + tablero[j][1];
  buffer[1][2] = tablero[j+1][2] + tablero[j][2];
}
*/
int evaluar_celda_3 ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // VECTORIZADO 
{
  int vecinos = ((tablero[fila-1][columna-1] + tablero[fila-1][columna]) 
                  + (tablero[fila-1][columna+1] + tablero[fila][columna-1]) ) 
                  + ( (tablero[fila][columna+1] + tablero[fila+1][columna-1]) 
                  + (tablero[fila+1][columna] + tablero[fila+1][columna+1]));
    
  int celda = tablero[fila][columna];
  return (vecinos == 3) || (vecinos == 2 && celda);
}

int evaluar_celda_comprimido_inicio ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // COMPRIMIDO 
{
  int vecinosFilaArriba0 = tablero[fila+1][columna-1] & 1 + __builtin_popcount((tablero[fila+1][columna] & 192));
  int vecinosFilaMedia = tablero[fila][columna-1] & 1 + __builtin_popcount((tablero[fila][columna] & 64));
  int vecinosFilaAbajo = tablero[fila-1][columna-1] & 1 + __builtin_popcount((tablero[fila-1][columna] & 192));

  int celda = tablero[fila][columna]&128 >> 7;
  int vecinos = vecinosFilaArriba0 + vecinosFilaMedia + vecinosFilaAbajo;
  return (vecinos == 3) || (vecinos == 2 && celda);
}

int evaluar_celda_comprimido_medio ( unsigned char tablero[Y+2][X+2], int fila, int columna, int shifts) // COMPRIMIDO 
{
  int vecinosFilaArriba0 = __builtin_popcount((tablero[fila+1][columna] & (224>>shifts)));
  int vecinosFilaMedia = __builtin_popcount((tablero[fila][columna] & (160>>shifts)));
  int vecinosFilaAbajo = __builtin_popcount((tablero[fila-1][columna] & (224>>shifts)));;

  int celda = tablero[fila][columna]&(64>>shifts) >> (6-shifts);
  int vecinos = vecinosFilaArriba0 + vecinosFilaMedia + vecinosFilaAbajo;
  return (vecinos == 3) || (vecinos == 2 && celda);
}

int evaluar_celda_comprimido_final ( unsigned char tablero[Y+2][X+2], int fila, int columna ) // COMPRIMIDO 
{
  int vecinosFilaArriba0 = tablero[fila+1][columna+1] >> 7 + __builtin_popcount((tablero[fila+1][columna] & 3));
  int vecinosFilaMedia = tablero[fila][columna+1] >> 7 + __builtin_popcount((tablero[fila][columna] & 2));
  int vecinosFilaAbajo = tablero[fila-1][columna+1] >> 7 + __builtin_popcount((tablero[fila-1][columna] & 3));

  int celda = tablero[fila][columna]&1;
  int vecinos = vecinosFilaArriba0 + vecinosFilaMedia + vecinosFilaAbajo;
  return (vecinos == 3) || (vecinos == 2 && celda);
}

char montarByte(unsigned char tablero[Y+2][X+2], int fila, int columna) {
  char byte = evaluar_celda_comprimido_inicio(tablero,fila,columna) << 7;
  for (int i = 0; i <= 5; i++)
  {
    byte += evaluar_celda_comprimido_medio(tablero, fila, columna, i) << (6-i);
  }
  byte += evaluar_celda_comprimido_final(tablero, fila, columna);
  return byte;
}

// Tablero del Juego de la VIDA
static unsigned char tablero[Y+2][X+2];

// Se usa para preservar valores durante el computo
static unsigned char tablero_auxiliar[Y+2][X+2];


void __attribute__((noinline)) actualizar_tablero ( unsigned char  tablero_original[Y+2][X+2], unsigned char tablero_tmp[Y+2][X+2] )
{
  int i,j;

  //copiar_tablero ( tablero_original, tablero_tmp );
  #pragma omp for 
  for ( i=1; i < Y+1; i++ )
    for ( j=1; j < X+1; j++ )
    {
      tablero_tmp[i][j] = evaluar_celda_XD (tablero_original, i, j);
    }
}

void __attribute__((noinline)) actualizar_tablero_comprimido ( unsigned char  tablero_original[Y+2][X+2], unsigned char tablero_tmp[Y+2][X+2] )
{
  int i,j;

  //copiar_tablero ( tablero_original, tablero_tmp );
  #pragma omp for 
  for ( i=1; i < Y+1; i++ ) {

    for ( j=1; j < X+1; j++ )
    {
      tablero_tmp[i][j] = montarByte (tablero_original, i, j);
    }
  }
}

int __attribute__((noinline)) actualizar_tablero_comprimido_end ( unsigned char tablero_original[Y+2][X+2])
{
  int count= 0, i,j;
  char old, neW;
  
  for ( i=1; i < Y+1; i++ )
    for ( j=1; j < X+1; j++)
    {

      old = tablero_original[i][j];
      neW = montarByte (tablero_original, i, j);
      count+=__builtin_popcount(~(old ^ neW)); 
    }
  return count; // numero de celdas que han cambiado
}


int __attribute__((noinline)) actualizar_tablero_end ( unsigned char tablero_original[Y+2][X+2])
{
  int count= 0, old, neW, i,j;
  
  for ( i=1; i < Y+1; i++ )
    for ( j=1; j < X+1; j++ )
    {
      old = tablero_original[i][j];
      neW = evaluar_celda_2 (tablero_original, i, j);
      count+= old != neW; 
    }
  return count; // numero de celdas que han cambiado
}


int JuegoDeLaVida ( int time )
{
  /*
  unsigned int mulVal = 1;
  unsigned int sumVal = 0;
  unsigned copy = seed;
  for (int i = 0; i < Y+2; i++)
  {
    sumVal = sumVal + mulVal*2531011;
    mulVal *= 214013;
  }*/
    
  //sumVal = 406518922;
  //mulVal = 1080739145;
  int checkSum;
  // inicializar de forma aleatoria
  inicializar_tablero ( tablero );
  ajustar_marco       ( tablero );
  ajustar_marco       ( tablero_auxiliar );
  
  #pragma omp parallel shared(checkSum, tablero, tablero_auxiliar, time) num_threads(12)
  {
    while (time > 2)
    {
      actualizar_tablero(tablero, tablero_auxiliar);
      actualizar_tablero(tablero_auxiliar, tablero);
      #pragma omp master 
      time = time - 2;
      #pragma omp barrier
    }
    if (time == 2) {
      actualizar_tablero(tablero, tablero_auxiliar);
    }
  }

  if (time == 2) 
    checkSum = actualizar_tablero_end(tablero_auxiliar);
  else 
    checkSum = actualizar_tablero_end(tablero);
  
  return checkSum;
} 

int MiniJuegoDeLaVida ( int time )
{
  /*
  unsigned int mulVal = 1;
  unsigned int sumVal = 0;
  unsigned copy = seed;
  for (int i = 0; i < Y+2; i++)
  {
    sumVal = sumVal + mulVal*2531011;
    mulVal *= 214013;
  }*/
    
  //sumVal = 406518922;
  //mulVal = 1080739145;
  int checkSum;
  // inicializar de forma aleatoria
  inicializar_tablero_comprimido ( tablero );
  ajustar_marco       ( tablero );
  ajustar_marco       ( tablero_auxiliar );
  
  #pragma omp parallel shared(checkSum, tablero, tablero_auxiliar, time) num_threads(2)
  {
    while (time > 2)
    {
      actualizar_tablero_comprimido(tablero, tablero_auxiliar);
      actualizar_tablero_comprimido(tablero_auxiliar, tablero);
      #pragma omp master 
      time = time - 2;
      #pragma omp barrier
    }
    if (time == 2) {
      actualizar_tablero_comprimido(tablero, tablero_auxiliar);
    }
  }

  if (time == 2) 
    checkSum = actualizar_tablero_comprimido_end(tablero_auxiliar);
  else 
    checkSum = actualizar_tablero_comprimido_end(tablero);
  
  return checkSum;
} 


int main (int argc, unsigned char **argv)
{
  int time_steps = 100; // Default number of time steps

  seed = 0; // Defined statically. By default, random seed is 0

  // obtener argumentos proporcionados en tiempo de ejecucion
  if (argc>1) {  time_steps = atoi(argv[1]); }
  if (argc>2) {  seed       = atoi(argv[2]); } 

  printf( "Matrix is %d x %d.\nNumber of iterations: %d\n", Y, X, time_steps);

  int chk = JuegoDeLaVida ( time_steps );  

  printf("CheckSum = %d\n", chk);

  return (0);
}
