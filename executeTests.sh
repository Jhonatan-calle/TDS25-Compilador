#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

echo "#################################"
for file in tests/pass/*.ctds; do
    echo ""
    echo "Pass - Case: $file"
    ./c-tds -t assembly -o zztmp "$file"
    echo ""
done
echo "#################################"
for file in tests/fails/*.ctds; do
    echo ""
    echo "Fails - Case: $file"
    ./c-tds -t assembly -o zztmp "$file"
    echo ""
done
rm -rf zztmp.out
echo "#################################"

echo "End of execution"
echo "___________________________"
