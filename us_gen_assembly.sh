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

# Create a temporary .s file
temp_file="$(mktemp /tmp/tmp_assembly_XXXXXX.s)"

# Copy contents from .ass to .s
cp "$input_file" "$temp_file"

echo "Compiling the assembly..."
gcc "$temp_file" -o us_file || {
    echo "Compilation failed."
    rm -f "$temp_file"
    exit 1
}

# Remove temporary file
rm -f "$temp_file"

echo "Executing..."
./us_file
