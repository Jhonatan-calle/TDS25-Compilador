#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

echo "#################################"
for file in tests/*.ctds; do
    echo ""
    echo "Case: $file"
    ./c-tds -d "$file"
    echo ""
done
echo "#################################"

echo "End of execution"
echo "___________________________"
