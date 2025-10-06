#!/bin/bash

# Check if a file argument was provided
if [ $# -eq 0 ]; then
    echo "Error: No input file provided"
    echo "Usage: $0 <input-file>"
    exit 1
fi

# Store the input file path
input_file="$1"

# Check if the file exists
if [ ! -f "$input_file" ]; then
    # Concatenate the extension in case of missing
    input_file="$input_file.ctds"
    # Check again if the file exists
    if [ ! -f "$input_file" ]; then
        echo "Error: File '$input_file' does not exist"
        exit 1
    fi
fi

# Check if the compiler exists
if [ ! -f "./c-tds" ]; then
    echo "Error: Compiler 'c-tds' not found. Please compile the project first."
    exit 1
fi

echo "---------------------------"
echo "Compiling individual file: $input_file"
echo "---------------------------"

echo "___________________________"
./c-tds -d -t codinter -o outtt "$input_file"
echo "___________________________"
echo ""
