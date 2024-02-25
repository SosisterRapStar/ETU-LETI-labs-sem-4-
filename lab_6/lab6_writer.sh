#!/bin/bash
g++	-c	lab6_writer.cpp
g++	-o	lab6_writer	lab6_writer.o -lrt -lpthread
./lab6_writer