#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

echo "#################################"
for file in tests/*.ctds; do
    echo ""
    echo "Case: $file"
    ./c-tds -t codinter -o zztmp "$file"
    echo ""
done
rm -rf zztmp.out
echo "#################################"

echo "End of execution"
echo "___________________________"
