#!/bin/bash
options=("Run lab2_1" "Run lab2_2" "Run lab2_3" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Run lab2_1")
            g++	-c	lab2_1.cpp
            g++	-o	lab2_1	lab2_1.o		-lpthread
            ./lab2_1
            ;;
        "Run lab2_2")
            g++	-c	lab2_2.cpp
            g++	-o	lab2_2	lab2_2.o		-lpthread
            ./lab2_2
            ;;
        "Run lab2_3")
            g++	-c	lab2_3.cpp
            g++	-o	lab2_3	lab2_3.o		-lpthread
            ./lab2_3
            ;;
        "Quit")
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done