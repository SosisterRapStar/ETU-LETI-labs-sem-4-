#!/bin/bash

g++ -c lab3.cpp
g++ -o lab3 lab3.o -lpthread


if [ $# -lt 1 ]; then
    echo "Создан файл lab3"
    echo "Для запуска программы используя pipe"
    echo "Используется аргумент командной строки pipe"
    echo "pipe2 и fcntl для запуска с pipe2 и fcntl соответсвенно"
    echo "При запуске без аргументов используется pipe"
    echo "Для запуска программы через скрипт в параметрах скрипта использутся pipe, pipe2 и fcntl"
    echo ""
    echo "Пример скрипта: ./script.sh pipe | pipe2 | fcntl"
    echo "Пример программы: ./lab3 pipe | pipe2 | fcntl"
    exit 1
fi


if [ "$1" = "pipe" ]; then
    ./lab3 pipe
elif [ "$1" = "pipe2" ]; then
    ./lab3 pipe2
elif [ "$1" = "fcntl" ]; then
    ./lab3 fcntl
fi