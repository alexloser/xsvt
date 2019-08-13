#!/bin/bash
# Test xsvt.onehot

rm -rf  data-onehot/*onehot.csv

echo
../xsvt.onehot.gcc.x64.d -i data-onehot/people.csv -o data-onehot/people-onehot.csv -d "," -k 1 -w 1

echo
../xsvt.onehot.gcc.x64.d -i data-onehot/num.csv    -o data-onehot/num-onehot.csv    -d " " -k 1 -w 0

