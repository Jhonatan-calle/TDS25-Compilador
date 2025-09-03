#!/bin/bash

echo "___________________________"
echo "Inicio ejecucion parser!"

echo "#################################"
echo "Caso 1: correcto.txt"
./parser tests/correcto.txt
echo "#################################"

echo "#################################"
echo "Caso 2: correcto2.txt"
./parser tests/correcto2.txt
echo "#################################"

echo "#################################"
echo "Caso 3: correcto3.txt"
./parser tests/correcto3.txt
echo "#################################"

echo "#################################"
echo "Caso 4: falla.txt"
./parser tests/falla.txt
echo "#################################"

echo "#################################"
echo "Caso 5: falla2.txt"
./parser tests/falla2.txt
echo "#################################"

echo "#################################"
echo "Caso 6: falla3.txt"
./parser tests/falla3.txt
echo "#################################"

echo "Fin ejecucion"
echo "___________________________"

