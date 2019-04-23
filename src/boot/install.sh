#!/usr/bin/env bash

# https://stackoverflow.com/a/1683850/3726096
trim()
{
    local trimmed="$1"

    # Strip leading spaces.
    while [[ $trimmed == ' '* ]]; do
       trimmed="${trimmed## }"
    done
    # Strip trailing spaces.
    while [[ $trimmed == *' ' ]]; do
        trimmed="${trimmed%% }"
    done

    echo "$trimmed"
}

BINARY=$(awk -F "=" '/TARGET/ {print $2}' Makefile);
BINARY="$(trim "$BINARY")"
BOOTFILE=1ST_READ.BIN

if [ ! -f "$BINARY" ]; then
  echo "fatal: no such file: $BINARY";
  exit 1;
fi

if [ -f "$BOOTFILE" ]; then
  rm -f "$BOOTFILE"
fi

kos-strip $BINARY
kos-objcopy -R .stack -O binary $BINARY $BINARY.bin
scramble $BINARY.bin $BOOTFILE

rm -f $BINARY.bin
mv $BOOTFILE ../../cd_root/

if [ -f "../../cd_root/$BOOTFILE" ]; then
  echo "$BOOTFILE processed successfully"
fi
