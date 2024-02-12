#!/bin/bash
g++	-c	lab1.cpp
g++	-o	lab1	lab1.o		-lpthread
sudo setcap CAP_SYS_NICE=ep lab1
./lab1