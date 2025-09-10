#!/bin/bash

echo "Starting compilation Flex and Bison"
bison -d -Wall syntax.y
flex lexicon.l
gcc -Wall -o c-tds syntax.tab.c lex.yy.c -lfl
echo "End compilation!"
