#!/bin/bash

echo "___________________________"
echo "Starting parsing execution!"

if [ ! -f "printInt.c" ]; then
    echo "Error: Missing file printInt.c (needed for linking)"
    exit 1
fi
echo "Compiling extern printInt..."
gcc -c printInt.c -o printInt.o || {
    echo "Error compiling printInt.c"
    rm -f "$assembly_file"
    exit 1
}

if [ ! -f "getInt.c" ]; then
    echo "Error: Missing file getInt.c (needed for linking)"
    exit 1
fi
echo "Compiling extern getInt..."
gcc -c getInt.c -o getInt.o || {
    echo "Error compiling getInt.c"
    rm -f "$assembly_file"
    exit 1
}

# Helper to compile a single .ctds into .o (emits object path)
compile_ctds_to_obj() {
    local src="$1"
    local base="$(basename "$src")"
    local stem="${base%.ctds}"
    local asm="build_${stem}.s"
    local obj="build_${stem}.o"

    # Log to stderr so caller's command substitution only captures the path
    echo "  -> Generating assembly for $src" >&2
    ./c-tds -t assembly -o zztmp "$src" 1>&2 || return 1

    cp last_generated_assembly.ass "$asm" 1>&2 || {
        echo "Error: last_generated_assembly.ass not found." >&2
        return 1
    }

    echo "  -> Assembling $asm" >&2
    gcc -c "$asm" -o "$obj" 1>&2 || return 1

    # Emit only the object path to stdout
    printf "%s\n" "$obj"
    return 0
}

echo "#################################"
# First handle pairs: N_driver.ctds + N_correcto.ctds
for driver in tests/pass/*_driver.ctds; do
    [ -e "$driver" ] || continue
    bdriver="$(basename "$driver")"
    # Only consider files starting with a number prefix
    if [[ "$bdriver" =~ ^([0-9]+)_ ]]; then
        num="${BASH_REMATCH[1]}"
        correcto="tests/pass/${num}_correcto.ctds"
        if [ -f "$correcto" ]; then
            echo ""
            echo "Pass - Pair: $num (compile ${num}_correcto.ctds + ${num}_driver.ctds; run driver)"
            obj_correcto="$(compile_ctds_to_obj "$correcto")" || {
                echo "Compilation failed for $correcto."
                rm -f printInt.o
                exit 1
            }
            [ -f "$obj_correcto" ] || { echo "Missing object: $obj_correcto" >&2; exit 1; }

            echo ""
            obj_driver="$(compile_ctds_to_obj "$driver")" || {
                echo "Compilation failed for $driver."
                rm -f printInt.o
                exit 1
            }
            [ -f "$obj_driver" ] || { echo "Missing object: $obj_driver" >&2; exit 1; }

            echo ""
            echo "Linking (driver first so its main is used)..."
            gcc "$obj_driver" "$obj_correcto" printInt.o -o us_file.out || {
                echo "Linking failed."
                rm -f printInt.o
                exit 1
            }
            echo "Executing (driver)..."
            ./us_file.out
            echo ""
        fi
    fi
done

# Now handle remaining singles (files without a driver pair)
for file in tests/pass/*.ctds; do
    [ -e "$file" ] || continue
    bfile="$(basename "$file")"

    # If it has a numeric prefix and a driver counterpart exists, skip (already handled)
    if [[ "$bfile" =~ ^([0-9]+)_ ]]; then
        num="${BASH_REMATCH[1]}"
        if [ -f "tests/pass/${num}_driver.ctds" ]; then
            continue
        fi
    fi

    echo ""
    echo "Pass - Case: $file"
    obj_file="$(compile_ctds_to_obj "$file")" && {
        [ -f "$obj_file" ] || { echo "Missing object: $obj_file" >&2; exit 1; }
        echo ""
        echo "Compiling and linking with extern printInt..."
        cp last_generated_assembly.ass last_generated_assembly.s
        gcc "$obj_file" printInt.o -o us_file.out || {
            echo "Compilation failed."
            rm -f printInt.o
            exit 1
        }
        echo "Executing..."
        ./us_file.out
    }
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
