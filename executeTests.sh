#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

for file in tests/*.txt; do
    echo "#################################"
    echo "Case: $file"
    ./parser "$file"
    echo "#################################"
done

echo "End of execution"
echo "___________________________"
