#!/bin/bash
g++	-c	lab6_reader.cpp
g++	-o	lab6_reader	lab6_reader.o -lrt -lpthread
./lab6_reader