#!/bin/bash
# Aquí poner los experimentos a calcular
# Formato: row col iter threads name
experiments=(
    "120 12000 10000 1 Entra_L1"
    "120 320000 1000 1 Entra_L2"
    "120 4700000 100 1 Entra_L3"
    "120 5000000 100 1 Entra_MM"
    "12 10000000 100 1 Codigo_Original"
    "12 20000000 100 1 King_Batch"
    "120 12000 100000 2 Entra_L1"
    "120 320000 10000 2 Entra_L2"
    "120 4700000 1000 2 Entra_L3"
    "120 5000000 1000 2 Entra_MM"
    "12 10000000 100 2 Codigo_Original"
    "12 20000000 100 2 King_Batch"
    "120 12000 100000 6 Entra_L1"
    "120 320000 10000 6 Entra_L2"
    "120 4700000 1000 6 Entra_L3"
    "120 5000000 1000 6 Entra_MM"
    "12 10000000 100 6 Codigo_Original"
    "12 20000000 100 6 King_Batch"
    "120 12000 100000 12 Entra_L1"
    "120 320000 10000 12 Entra_L2"
    "120 4700000 1000 12 Entra_L3"
    "120 5000000 1000 12 Entra_MM"
    "12 10000000 100 12 Codigo_Original"
    "12 20000000 100 12 King_Batch"
)
echo "{" > result.json
for exp in "${experiments[@]}"; do
    # Expande los valores de la línea en argumentos
    bash createAnalisisJSON.sh $exp >> result.json
    echo "," >> result.json
done
sed -i '$d' result.json
echo "}" >> result.json
