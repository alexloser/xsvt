#!/bin/bash
# Test xsvt.transfer

echo "Test csv => svm"
../xsvt.transfer.gcc.x64.d -c2s -i data-transfer/eta.csv -l data-transfer/eta.label -o data-transfer/eta.svm.dat -d ","

echo "Test svm => csv"
../xsvt.transfer.gcc.x64.d -s2c -i data-transfer/hs.svm.dat -l data-transfer/hs.label -o data-transfer/hs.csv  -d " "

