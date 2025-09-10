#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

for file in tests/*.ctds; do
    echo "#################################"
    echo "Case: $file"
    ./c-tds "$file"
    echo "#################################"
done

echo "End of execution"
echo "___________________________"
