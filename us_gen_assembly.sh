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

echo "Compiling and linking with extern printInt..."
gcc "$assembly_file" printInt.o -o us_file.out || {
    echo "Compilation failed."
    rm -f "$assembly_file" printInt.o
    exit 1
}

echo "Executing..."
./us_file.out
