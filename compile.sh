#!/bin/bash

echo "Starting compilation Flex and Bison"
bison -d -Wall syntax.y
flex lexicon.l

echo "Compiling sources"
gcc -Wall -g -c src/ast.c -o build/ast.o
gcc -Wall -g -c src/simbolos.c -o build/simbolos.o
gcc -Wall -g -c src/utils.c -o build/utils.o
gcc -Wall -g -c src/main.c -o build/main.o

echo "Linking"
gcc -Wall -g -Wno-unused-function syntax.tab.c lex.yy.c \
  build/main.o build/utils.o build/ast.o build/simbolos.o \
  -o build/c-tds -lfl

# Create symbolic link with the output name
if [ -f "c-tds" ]; then
  rm -f c-tds
fi
ln -s build/c-tds c-tds

echo "End compilation!"
