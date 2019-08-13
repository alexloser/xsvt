#!/bin/bash
# Test xsvt.checker

echo test people.csv
../xsvt.checker.gcc.x64.d -i data-checker/people.csv -d ","

echo test wrong.csv 
../xsvt.checker.gcc.x64.d -i data-checker/wrong.csv -d " "

echo test num.csv 
../xsvt.checker.gcc.x64.d -i data-checker/num.csv -d "\t"

