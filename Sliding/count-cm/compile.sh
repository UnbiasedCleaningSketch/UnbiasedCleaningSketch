#!/bin/bash
make clean
make
./demo.out 5 20000 100000
./demo.out 5 20000 200000
./demo.out 5 20000 400000
./demo.out 5 20000 800000
./demo.out 5 20000 1600000
./demo.out 5 20000 3200000
./demo.out 5 20000 6400000