#!/usr/bin/env bash

# Get a list of all .c files inside src.
mapfile -d '' cFilenames < <(find src -type f -name '*.c' -print0)

appName="yap"

# Header search paths
includeFlags=(-Iinclude -Isrc)

mkdir -p bin

clang -std=c23 "${cFilenames[@]}" "${includeFlags[@]}" ${compilerFlags:-} -o "./bin/$appName"

if [ $? -ne 0 ]; then
    echo
    echo "$appName build failed."
    exit 1
fi

echo
echo "$appName build succeeded."
exit 0
