#!/bin/bash
rows=$1
columns=$2
iterations=$3
n_threads=$4
nameExperiment=$5
seed=$6
echo "#define X $columns" > tmp.c
echo "#define Y $rows" >> tmp.c
echo "#define NUM_THREADS $n_threads" >> tmp.c
cat exampleGenerate.c >> tmp.c
gcc -Ofast -march=native -fopenmp -msse4.2 -mpopcnt ./tmp.c -o tmp
if [ -z "$seed" ]; then
    perf stat ./tmp $iterations 2> results.txt
else
    perf stat ./tmp $iterations $seed 2> results.txt
fi
rm ./tmp.c
rm ./tmp
# Extraer datos
ciclos=$(grep "cycles" results.txt | awk '{print $1}' | sed 's/,//g')
instructions=$(grep -E "instructions" results.txt | awk '{print $1}')
tiempo=$(grep "seconds time elapsed" results.txt | awk '{print $1}')
reloj=$(grep -E "cpu_core/cycles:u/        #" results.txt | awk -F'#' '{print $2}' | awk '{print $1}')
ratio=$(awk -v t="$tiempo" -v it="$iterations" -v cel=$(($rows*$columns)) \
    'BEGIN {printf "%.20f", t/(it*cel)}')
rm results.txt
# Poner en el fomato
ciclos="${ciclos//./}"
instructions="${instructions//./}"
ciclos=$(awk -v x="$ciclos" 'BEGIN {printf "%.3f", x/1e9}')
instructions=$(awk -v x="$instructions" 'BEGIN {printf "%.3f", x/1e9}')
tiempo="${tiempo//,/.}"
reloj="${reloj//,/.}"

# Calcular derivados
ipc=$(awk -v ins="$instructions" -v cyc="$ciclos" 'BEGIN {print ins/cyc}')
cat <<EOF 
"${columns}x${rows}x${n_threads}x${iterations}": {
    "ciclos": $ciclos,
    "instructions":$instructions,
    "IPC": $ipc,
    "tiempo": $tiempo,
    "reloj": $reloj,
    "(t)/(iter*celdas)": $ratio,
    "n_threads": $n_threads,
    "iterations" : $iterations,
    "nameExperiment" : "${nameExperiment}"
}
EOF
