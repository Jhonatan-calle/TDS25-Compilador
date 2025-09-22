#!/bin/bash

echo "Starting compilation Flex and Bison"
bison -d -Wall syntax.y
flex lexicon.l

echo "Compiling sources"
gcc -Wall -c src/ast.c -o build/ast.o
gcc -Wall -c src/symbols.c -o build/symbols.o
gcc -Wall -c src/utils.c -o build/utils.o
gcc -Wall -c src/main.c -o build/main.o

echo "Linking"
gcc -Wall -Wno-unused-function syntax.tab.c lex.yy.c \
  build/main.o build/utils.o build/ast.o build/symbols.o \
  -o build/c-tds -lfl

# Create symbolic link with the output name
if [ -f "c-tds" ]; then
    rm -f c-tds
fi
ln -s build/c-tds c-tds

echo "End compilation!"
