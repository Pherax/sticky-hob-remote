#!/bin/sh
# Host tests for the Sticky UI logic (no device needed).
set -eu
cd "$(dirname "$0")"
for t in *.cpp; do
  bin=$(mktemp)
  g++ -std=c++17 -I mocks -o "$bin" "$t"
  "$bin" > /dev/null
  rm -f "$bin"
  echo "PASS $t"
done
