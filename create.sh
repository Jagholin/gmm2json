#!/usr/bin/env bash

shopt -s globstar nullglob
export CFLAGS="$CFLAGS -ggdb"

make
