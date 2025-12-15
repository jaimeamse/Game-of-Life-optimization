#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Board size is 10000 x 10000
#define X 10000L
#define Y 10000L
#define NUM_THREADS 6
static unsigned int seed;

static inline unsigned char myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>16) & 1;
}

static inline unsigned char updateSeed() {
  seed = (214013*seed+2531011);
}

// Coloca valores aleatorios
void __attribute__((noinline)) inicializar_tablero ( unsigned char* T )
{
  int i, j;
  unsigned copy = seed;
  //sumVal = 406518922;
  //mulVal = 1080739145;
  for ( i= 0; i < Y+2; i++ ) {
    copy = (214013*copy+2531011);
    seed = copy;
    for ( j= 0; j < X+2; j++ ) {
      T[i*(2+X)+j] = (seed>>16) & 1;
      seed = (1080739145*seed+406518922);
    }
  }
}

void ajustar_marco( unsigned char* T )
{
  int i;
  for ( i=0; i < X+2; i++ )  T[i] = T[(Y+1)*(X+2)+i] = 0;
  for ( i=0; i < Y+2; i++ )  T[i*(X+2)] = T[(X+2)*i+X+1] = 0;
}

char evaluar_celda_XD ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO 
{
  char vecinos = ((tablero[(fila-1)*(X+2)+columna-1] + tablero[(fila-1)*(X+2)+columna]) 
                  + (tablero[(fila-1)*(X+2)+columna+1] + tablero[(fila)*(X+2)+columna-1]) ) 
                  + ( (tablero[(fila)*(X+2)+columna+1] + tablero[(fila+1)*(X+2)+columna-1]) 
                  + (tablero[(fila+1)*(X+2)+columna] + tablero[(fila+1)*(X+2)+columna+1]));
    
  char celda = tablero[(fila)*(X+2)+columna];
  return (vecinos == 3) || (vecinos == 2 && celda);
}


// Tablero del Juego de la VIDA
static unsigned char* tablero;

// Se usa para preservar valores durante el computo
static unsigned char* tablero_auxiliar;


void __attribute__((noinline)) actualizar_tablero ( unsigned char* tablero_original, unsigned char* tablero_tmp )
{
  int tid = omp_get_thread_num();
  int bloque = 10000; // tamaño del trozo horizontal
  int repartBlock = (X+1)/NUM_THREADS;
  if (repartBlock < bloque)
  {
    bloque = repartBlock;
  }
  
  for (int start = tid * bloque + 1; start < X+1; start += NUM_THREADS * bloque) {
    int end = start + bloque;
    if (end > X+1) end = X+1;
    for (int i = 1; i < Y+1; i++) { 
      for (int j = start; j < end; j++)
        tablero_tmp[i*(2+X)+j] = evaluar_celda_XD(tablero_original, i, j);
    }
  }
} 
 
void __attribute__((noinline)) actualizar_tablero_old ( unsigned char* tablero_original, unsigned char* tablero_tmp )
{
  
  #pragma omp parallel for
  for (int i = 1; i < Y+1; i++) { 
    for (int j = 1; j < X+1; j++)
      tablero_tmp[i*(2+X)+j] = evaluar_celda_XD(tablero_original, i, j);
  }
} 

int __attribute__((noinline)) actualizar_tablero_end ( unsigned char* tablero_original)
{
  int count= 0, old, neW, i,j;
  
  for ( i=1; i < Y+1; i++ )
    for ( j=1; j < X+1; j++ )
    {
      old = tablero_original[i*(2+X)+j];
      neW = evaluar_celda_XD (tablero_original, i, j);
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
  
  #pragma omp parallel shared(checkSum, tablero, tablero_auxiliar, time) num_threads(NUM_THREADS)
  {

    while (time > 2)
    {
      actualizar_tablero(tablero, tablero_auxiliar);
      #pragma omp barrier
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

int main (int argc, char **argv)
{
  int time_steps = 30; // Default number of time steps

  seed = 0; // Defined statically. By default, random seed is 0

  // obtener argumentos proporcionados en tiempo de ejecucion
  if (argc>1) {  time_steps = atoi(argv[1]); }
  if (argc>2) {  seed       = atoi(argv[2]); } 
  tablero = malloc((X + 2) * (Y + 2) );
  tablero_auxiliar = malloc((X + 2) * (Y + 2) );
  printf( "Matrix is %d x %d.\nNumber of iterations: %d\n", Y, X, time_steps);

  int chk = JuegoDeLaVida ( time_steps );  

  printf("CheckSum = %d\n", chk);

  return (0);
}
