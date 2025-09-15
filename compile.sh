#!/bin/bash

echo "Starting compilation Flex and Bison"
gcc -Wall -c src/main.c -o build/main.o
gcc -Wall -c src/utils.c -o build/utils.o
bison -d -Wall syntax.y
flex lexicon.l
gcc -Wall -Wno-unused-function syntax.tab.c lex.yy.c build/main.o build/utils.o -o c-tds -lfl
echo "End compilation!"
