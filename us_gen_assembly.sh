#!/bin/bash

# Check if a file argument was provided
if [ $# -eq 0 ]; then
    echo "Error: No input file provided"
    echo "Usage: $0 <input-file>"
    exit 1
fi

# Store the input file path
input_file="$1"

if [[ "$input_file" != *.s ]]; then
    input_file="${input_file}.s"
fi

if [ ! -f "$input_file" ]; then
    echo "Error: File not found: $input_file"
    exit 1
fi

gcc -c "$input_file" -o us_file.o || exit 1
gcc us_file.o -o us_file
./us_file
