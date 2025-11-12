#!/bin/bash

# Check if a file argument was provided
if [ $# -eq 0 ]; then
    echo "Error: No input file provided"
    echo "Usage: $0 <input-file>"
    exit 1
fi

# Store the input file path
input_file="$1"

if [[ "$input_file" != *.ass ]]; then
    input_file="${input_file}.ass"
fi

if [ ! -f "$input_file" ]; then
    echo "Error: File not found: $input_file"
    exit 1
fi

# Create an executable .s file
assembly_file="${input_file%.ass}.s"

# Copy contents from .ass to .s
cp "$input_file" "$assembly_file"

# printInt
if [ ! -f "printInt.c" ]; then
    echo "Error: Missing file printInt.c (needed for linking)"
    exit 1
fi
echo "Compiling extern printInt..."
gcc -c printInt.c -o printInt.o || {
    echo "Error compiling printInt.c"
    rm -f "$assembly_file"
    exit 1
}

# getInt
if [ ! -f "getInt.c" ]; then
    echo "Error: Missing file getInt.c (needed for linking)"
    exit 1
fi
echo "Compiling extern getInt..."
gcc -c getInt.c -o getInt.o || {
    echo "Error compiling getInt.c"
    exit 1
}

# eq_assert
if [ ! -f "eq_assert.c" ]; then
    echo "Error: Missing file eq_assert.c (needed for linking)"
    exit 1
fi
echo "Compiling extern eq_assert..."
gcc -c eq_assert.c -o eq_assert.o || {
    echo "Error compiling eq_assert.c"
    exit 1
}

echo "Compiling and linking with extern printInt and getInt and eq_assert..."
cp last_generated_assembly.ass last_generated_assembly.s
gcc "$assembly_file" printInt.o getInt.o eq_assert.o -o us_file.out || {
    echo "Compilation failed."
    rm -f printInt.o
    rm -f getInt.o
    rm -f eq_assert.o
    exit 1
}

echo "Executing..."
./us_file.out
