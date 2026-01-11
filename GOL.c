#define X 100000L
#define Y 100000L
#define NUM_THREADS 12
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


static inline char evaluar_celda_XD ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char vecinos = (char) ((char)(tablero[(fila-1)*(X+2)+columna-1] + tablero[(fila-1)*(X+2)+columna])
                 +  (char) (tablero[(fila-1)*(X+2)+columna+1] + tablero[(fila)*(X+2)+columna-1]) )
                 + (char) ( (char) (tablero[(fila)*(X+2)+columna+1] + tablero[(fila+1)*(X+2)+columna-1])
                 + (char) (tablero[(fila+1)*(X+2)+columna] + tablero[(fila+1)*(X+2)+columna+1]));
  
 char celda = tablero[(fila)*(X+2)+columna];
 return (vecinos == 3) || (vecinos == 2 && celda);
}


static inline char evaluar_celda_2_step ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = evaluar_celda_XD(tablero, fila-1, columna-1) + evaluar_celda_XD(tablero, fila-1, columna) + evaluar_celda_XD(tablero, fila-1, columna+1)
         + evaluar_celda_XD(tablero, fila, columna-1) + evaluar_celda_XD(tablero, fila, columna+1)
         + evaluar_celda_XD(tablero, fila+1, columna-1) + evaluar_celda_XD(tablero, fila+1, columna) + evaluar_celda_XD(tablero, fila+1, columna+1);
 return (vecinos == 3) | (vecinos == 2 & celda);
}


static inline char evaluar_celda_2_step_old ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = evaluar_celda_XD(tablero, fila-1, columna-1) + evaluar_celda_XD(tablero, fila-1, columna) + evaluar_celda_XD(tablero, fila-1, columna+1)
         + evaluar_celda_XD(tablero, fila, columna-1) + evaluar_celda_XD(tablero, fila, columna+1)
         + evaluar_celda_XD(tablero, fila+1, columna-1) + evaluar_celda_XD(tablero, fila+1, columna) + evaluar_celda_XD(tablero, fila+1, columna+1);
 return (vecinos == 3) || (vecinos == 2 && celda);
}




char evaluar_celda_2_step_esquina_superior_izquierda( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = tablero[(fila-1)*(X+2)+columna-1] + tablero[(fila-1)*(X+2)+columna] + tablero[(fila-1)*(X+2)+columna+1]
         + tablero[(fila)*(X+2)+columna-1] + evaluar_celda_XD(tablero, fila, columna+1)
         + tablero[(fila+1)*(X+2)+columna-1] + evaluar_celda_XD(tablero, fila+1, columna) + evaluar_celda_XD(tablero, fila+1, columna+1);
 return (vecinos == 3) || (vecinos == 2 && celda);
}


char evaluar_celda_2_step_inicio ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = tablero[(fila-1)*(X+2)+columna-1] + tablero[(fila-1)*(X+2)+columna] + tablero[(fila-1)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila, columna-1) + evaluar_celda_XD(tablero, fila, columna+1)
         + evaluar_celda_XD(tablero, fila+1, columna-1) + evaluar_celda_XD(tablero, fila+1, columna) + evaluar_celda_XD(tablero, fila+1, columna+1);
 return (vecinos == 3) | (vecinos == 2 & celda);
}


char evaluar_celda_2_step_esquina_superior_derecha( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = tablero[(fila-1)*(X+2)+columna-1] + tablero[(fila-1)*(X+2)+columna] + tablero[(fila-1)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila, columna-1) + tablero[(fila)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila+1, columna-1) + evaluar_celda_XD(tablero, fila+1, columna) + tablero[(fila+1)*(X+2)+columna+1];
 return (vecinos == 3) | (vecinos == 2 & celda);
}


char evaluar_celda_2_step_esquina_inferior_izquierda( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = tablero[(fila-1)*(X+2)+columna-1] + evaluar_celda_XD(tablero, fila-1, columna) + evaluar_celda_XD(tablero, fila-1, columna+1)
         + tablero[(fila)*(X+2)+columna-1] + evaluar_celda_XD(tablero, fila, columna+1)
         + tablero[(fila+1)*(X+2)+columna-1]  + tablero[(fila+1)*(X+2)+columna] + tablero[(fila+1)*(X+2)+columna+1];
 return (vecinos == 3) | (vecinos == 2 & celda);
}


char evaluar_celda_2_step_esquina_inferior_derecha( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = evaluar_celda_XD(tablero, fila-1, columna-1) + evaluar_celda_XD(tablero, fila-1, columna) + tablero[(fila-1)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila, columna-1) + tablero[(fila)*(X+2)+columna+1]
         + tablero[(fila+1)*(X+2)+columna-1]  + tablero[(fila+1)*(X+2)+columna] + tablero[(fila+1)*(X+2)+columna+1];
 return (vecinos == 3) | (vecinos == 2 & celda);
}


char evaluar_celda_2_step_final ( unsigned char* tablero, int fila, int columna ) // VECTORIZADO
{
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = evaluar_celda_XD(tablero, fila-1, columna-1) + evaluar_celda_XD(tablero, fila-1, columna) + evaluar_celda_XD(tablero, fila-1, columna+1)
         + evaluar_celda_XD(tablero, fila, columna-1) + evaluar_celda_XD(tablero, fila, columna+1)
         + tablero[(fila+1)*(X+2)+columna-1] + tablero[(fila+1)*(X+2)+columna] + tablero[(fila+1)*(X+2)+columna+1];
 return (vecinos == 3) | (vecinos == 2 & celda);
}
// Tablero del Juego de la VIDA
static unsigned char* tablero;


// Se usa para preservar valores durante el computo
static unsigned char* tablero_auxiliar;




void __attribute__((noinline)) actualizar_tablero ( unsigned char* tablero_original, unsigned char* tablero_tmp )
{
 int tid = omp_get_thread_num();
 int bloque = 9984; // tamaño del trozo horizontal
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


void  evaluearFilaInicial( unsigned char* tablero_original, unsigned char* tablero_tmp, int start, int end ) {
 for (int i = 1; i < 2; i++) {
     for (int j = start; j < end; j++)
       tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step_inicio(tablero_original, i, j);
 }
}


void evaluearFilaFinal ( unsigned char* tablero_original, unsigned char* tablero_tmp, int start, int end ) {
 for (int i = Y; i < Y+1; i++) {
     for (int j = start; j < end; j++)
       tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step_final(tablero_original, i, j);
 }
}


char evaluarElementoInicial ( unsigned char* tablero_original, int fila, int columna) {
 char celda = evaluar_celda_XD(tablero_original, fila, columna);
 char vecinos = tablero_original[(fila-1)*(X+2)+columna-1] + evaluar_celda_XD(tablero_original, fila-1, columna) + evaluar_celda_XD(tablero_original, fila-1, columna+1)
         + tablero_original[(fila)*(X+2)+columna-1] + evaluar_celda_XD(tablero_original, fila, columna+1)
         + tablero_original[(fila+1)*(X+2)+columna-1] + evaluar_celda_XD(tablero_original, fila+1, columna) + evaluar_celda_XD(tablero_original, fila+1, columna+1);
 return (vecinos == 3) || (vecinos == 2 && celda);
}


char evaluarElementoFinal ( unsigned char* tablero_original, unsigned char* tablero_tmp, int fila, int columna) {
 char celda = evaluar_celda_XD(tablero, fila, columna);
 char vecinos = evaluar_celda_XD(tablero, fila-1, columna-1) + evaluar_celda_XD(tablero, fila-1, columna) + tablero[(fila-1)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila, columna-1) + tablero[(fila)*(X+2)+columna+1]
         + evaluar_celda_XD(tablero, fila+1, columna-1) + evaluar_celda_XD(tablero, fila+1, columna) + tablero[(fila+1)*(X+2)+columna+1];
 return (vecinos == 3) || (vecinos == 2 && celda);
}


void evaluarBloqueInicio ( unsigned char* tablero_original, unsigned char* tablero_tmp, int start, int end) {
  evaluearFilaInicial( tablero_original, tablero_tmp, start, end );


 for (int i = 2; i < Y; i++) {
   tablero_tmp[i*(X+2)+1] = evaluarElementoInicial(tablero_original, i, 1);
   for (int j = start; j < end; j++)
     tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
 }


 evaluearFilaFinal ( tablero_original, tablero_tmp, start, end);
}


void evaluarBloqueFinal ( unsigned char* tablero_original, unsigned char* tablero_tmp, int start, int end) {
 evaluearFilaInicial( tablero_original, tablero_tmp, start, end );


 for (int i = 2; i < Y; i++) {
   for (int j = start; j < end; j++)
     tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
   tablero_tmp[i*(X+2)+X] = evaluarElementoFinal(tablero_original, tablero_tmp, i, X);
 }


 evaluearFilaFinal ( tablero_original, tablero_tmp, start, end);
}




void evaluarBloqueNormal ( unsigned char* tablero_original, unsigned char* tablero_tmp, int start, int end) {
 evaluearFilaInicial( tablero_original, tablero_tmp, start, end );


 for (int i = 2; i < Y; i++) {
   for (int j = start; j < end; j++)
     tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
 }


 evaluearFilaFinal ( tablero_original, tablero_tmp, start, end);
}


char evaluarCeldaEsquinaMedioIzquierda(unsigned char* tablero_original, int i) {
 char celda = evaluar_celda_XD(tablero_original, i, 1);
 char vecinos = tablero_original[(i-1)*(X+2)] + evaluar_celda_XD(tablero_original, i-1, 1) + evaluar_celda_XD(tablero_original, i-1, 2)
         + tablero_original[i*(X+2)] + evaluar_celda_XD(tablero_original, i, 2)
         + tablero_original[(i+1)*(X+2)] + evaluar_celda_XD(tablero_original, i+1, 1) + evaluar_celda_XD(tablero_original, i+1, 2);
 return (vecinos == 3) || (vecinos == 2 && celda);
}


char evaluarCeldaEsquinaMedioDerecha(unsigned char* tablero_original, int i) {
 char celda = evaluar_celda_XD(tablero_original, i, X);
 char vecinos = evaluar_celda_XD(tablero_original, i-1, X-1) + evaluar_celda_XD(tablero_original, i-1, X) + tablero_original[(i-1)*(X+2)+X+1]
         + evaluar_celda_XD(tablero_original, i, X-1) + tablero_original[i*(X+2)+X+1]
         + evaluar_celda_XD(tablero_original, i+1, X-1) + evaluar_celda_XD(tablero_original, i+1, X) + tablero_original[(i+1)*(X+2)+X+1];
 return (vecinos == 3) || (vecinos == 2 && celda);
}


void __attribute__((noinline)) actualizar_tablero_2_steps(unsigned char* tablero_original, unsigned char* tablero_tmp)
{
 int tid = omp_get_thread_num();
  // Thread 0 calcula las 4 esquinas
 if (tid == 0) {
   tablero_tmp[1*(X+2)+1] = evaluar_celda_2_step_esquina_superior_izquierda(tablero_original, 1, 1);
   tablero_tmp[1*(X+2)+X] = evaluar_celda_2_step_esquina_superior_derecha(tablero_original, 1, X);
   tablero_tmp[Y*(X+2)+1] = evaluar_celda_2_step_esquina_inferior_izquierda(tablero_original, Y, 1);
   tablero_tmp[Y*(X+2)+X] = evaluar_celda_2_step_esquina_inferior_derecha(tablero_original, Y, X);
 }
  int bloque = 7000; // tamaño del trozo horizontal, al ser que leemos 5 filas de la matriz original + 1 de la auxiliar ya no es 9984
 int repartBlock = (X+1)/NUM_THREADS;
 if (repartBlock < bloque)
 {
   bloque = repartBlock;
 }


 int start = tid * bloque + 2;
 int end = start + bloque;
 // Thread 0 también calcula los bordes laterales (columnas 1 y X)
 if (tid == 0) {
   // Columna 1 (filas interiores 2 a Y-1)
   for (int j = start; j < end; j++) {
     tablero_tmp[1*(X+2)+j] = evaluar_celda_2_step_inicio(tablero_original, 1, j);
   }
   for (int i = 2; i < Y; i++) {
     tablero_tmp[i*(X+2)+1] = evaluarCeldaEsquinaMedioIzquierda(tablero_original, i);
     for (int j = start; j < end; j++)
       tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
   }
   for (int j = start; j < end; j++) {
     tablero_tmp[Y*(X+2)+j] = evaluar_celda_2_step_final(tablero_original, Y, j);
   }
  
   start += NUM_THREADS * bloque;
 }


 for (start; start < X; start += NUM_THREADS * bloque) {
   end = start + bloque;
   if (end > X) end = X;
   if (end == X) {
     for (int j = start; j < end; j++) {
       tablero_tmp[1*(X+2)+j] = evaluar_celda_2_step_inicio(tablero_original, 1, j);
     }
     for (int i = 2; i < Y; i++) {
       for (int j = start; j < end; j++)
         tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
       tablero_tmp[i*(X+2)+X] = evaluarCeldaEsquinaMedioDerecha(tablero_original, i);
     }
     for (int j = start; j < end; j++) {
       tablero_tmp[Y*(X+2)+j] = evaluar_celda_2_step_final(tablero_original, Y, j);
     }
   }
   else {
     for (int j = start; j < end; j++) {
       tablero_tmp[1*(X+2)+j] = evaluar_celda_2_step_inicio(tablero_original, 1, j);
     }
     for (int i = 2; i < Y; i++) {
       for (int j = start; j < end; j++)
         tablero_tmp[i*(2+X)+j] = evaluar_celda_2_step(tablero_original, i, j);
     }
     for (int j = start; j < end; j++) {
       tablero_tmp[Y*(X+2)+j] = evaluar_celda_2_step_final(tablero_original, Y, j);
     }
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


 char auxiliarFlag = 0;


 #pragma omp parallel shared(checkSum, tablero, tablero_auxiliar, time, auxiliarFlag) num_threads(NUM_THREADS)
 {


   while (time > 4)
   {
     actualizar_tablero_2_steps(tablero, tablero_auxiliar);
     #pragma omp barrier
     actualizar_tablero_2_steps(tablero_auxiliar, tablero);
     #pragma omp master
     time = time - 4;
     #pragma omp barrier
   }
   if (time >= 3) {
     actualizar_tablero_2_steps(tablero, tablero_auxiliar);
     auxiliarFlag = 1;
     #pragma omp barrier
     if (time - 2 == 2) {
       actualizar_tablero(tablero_auxiliar, tablero);
       auxiliarFlag = 0;
     }
   }
   else if (time == 2) {
     actualizar_tablero(tablero, tablero_auxiliar);
     auxiliarFlag = 1;
   }
 }


 if (auxiliarFlag)
   checkSum = actualizar_tablero_end(tablero_auxiliar);
 else
   checkSum = actualizar_tablero_end(tablero);
  return checkSum;
}


int main (int argc, char **argv)
{
 int time_steps = 100; // Default number of time steps


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
