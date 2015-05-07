#!/bin/sh

make || exit 1

insmod calc.ko || exit 1
echo
echo "reading /proc/calc:"
cat /proc/calc
rmmod calc.ko || exit 1
