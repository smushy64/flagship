#!/usr/bin/env bash

if [ ! -f "./example" ] || [ example.c -nt ./example ] || [ flagship.h -nt ./example ]; then
    set -x
    gcc example.c -o example -ggdb -O0
    { set +x; } 2>/dev/null
fi

./example "$@"
