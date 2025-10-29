#!/bin/bash

# Check if a file argument was provided
if [ $# -eq 0 ]; then
    echo "Error: No input file provided"
    echo "Usage: $0 <input-file>"
    exit 1
fi

# Store the input file path
input_file="$1"

if [[ "$input_file" != *.c ]]; then
    input_file="${input_file}.c"
fi

if [ ! -f "$input_file" ]; then
    echo "Error: File not found: $input_file"
    exit 1
fi

echo "Generating assembler from $input_file..."
gcc -S "$input_file" || exit 1

echo "Compiling object file..."
gcc -c "$input_file" -o c_file.o || exit 1

echo "Linking executable..."
gcc c_file.o -o c_file || exit 1

echo "Running program..."
./c_file
