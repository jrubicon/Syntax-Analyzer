#!/bin/bash

#Author: Justin Drouin
#Assignment 2
#Date: October 30th, 2019
#Program name: Assignment 2

rm *.o
rm *.out
rm tokenOutput.txt

echo "Compiling Assignment 1 Lexer"

g++ -std=c++14 main.cpp -o Run.out

echo "Running Assignment 1 Lexer"
echo "Please enter the source file:"
read source
./Run.out $source

echo "Lexer Complete. Output file Generated. Terminating."
open tokenOutput.txt
