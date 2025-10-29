#!/bin/bash

# Check if a file argument was provided
if [ $# -eq 0 ]; then
    echo "Error: No input file provided"
    echo "Usage: $0 <input-file>"
    exit 1
fi

# Store the input file path
input_file_concat="$1.c"

# Check if the file exists
if [ ! -f "$1" ]; then
    echo "Error: File '$1' does not exist"
fi
if [ ! -f "$input_file_concat" ]; then
  echo "Error: File '$input_file_concat' does not exist"
  echo "Exiting"
  exit 1
fi
echo "File "$input_file_concat" found"

gcc -S "$input_file_concat"
gcc -c "$1".s -o c_file.o
gcc c_file.o -o c_file
./c_file
