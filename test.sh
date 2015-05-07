#!/bin/sh

make || exit 1

sudo insmod calc.ko || exit 1
echo
echo "writing 12 to /proc/calc"
sudo echo 12 > /proc/calc
echo "writing + to /proc/calc"
sudo echo + > /proc/calc
echo "writing 3 to /proc/calc"
sudo echo 5 > /proc/calc
echo "reading /proc/calc:"
cat /proc/calc
sudo rmmod calc.ko || exit 1
