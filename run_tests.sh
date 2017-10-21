#!/bin/bash -e

mkdir -p build

for test_file in $(grep -Rl '#ifdef TEST' | grep '\.c$'); do
    gcc -o build/test_binary -DTEST -DICACHE_FLASH_ATTR= ${test_file} user/jsonparse/*.c
    ./build/test_binary
done
