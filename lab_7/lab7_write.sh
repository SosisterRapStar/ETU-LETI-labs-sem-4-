#!/bin/bash
g++	-c	lab7_writer.cpp
g++	-o	lab7_writer	lab7_writer.o -lrt -lpthread
./lab7_writer