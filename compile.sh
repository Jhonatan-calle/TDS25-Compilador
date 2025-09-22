#!/bin/bash

echo "Starting compilation Flex and Bison"
bison -d -Wall -Wcounterexamples syntax.y
flex lexicon.l

echo "Compiling sources"
gcc -Wall -c src/ast.c -o build/ast.o
gcc -Wall -c src/simbolos.c -o build/simbolos.o
gcc -Wall -c src/utils.c -o build/utils.o
gcc -Wall -c src/main.c -o build/main.o

echo "Linking"
gcc -Wall -Wno-unused-function syntax.tab.c lex.yy.c \
  build/main.o build/utils.o build/ast.o build/simbolos.o \
  -o build/c-tds -lfl

# Create symbolic link with the output name
if [ -f "c-tds" ]; then
    rm -f c-tds
fi
ln -s build/c-tds c-tds

echo "End compilation!"
