#!/bin/bash
g++	-c	lab7_reader.cpp
g++	-o	lab7_reader	lab7_reader.o -lrt -lpthread
./lab7_reader