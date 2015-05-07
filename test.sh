#!/bin/sh

make || exit 1

sudo insmod calc.ko || exit 1
echo
echo "writing 12 to /proc/calc_a"
sudo echo 12 > /proc/calc_a
echo "writing + to /proc/calc_op"
sudo echo + > /proc/calc_op
echo "writing 3 to /proc/calc_b"
sudo echo 5 > /proc/calc_b
echo "reading /proc/calc_res :"
cat /proc/calc_res
sudo rmmod calc.ko || exit 1
