#!/bin/bash

project_name="project"

echo "compiling $project_name"
rm -f lex.yy.c
rm -f $project_name.tab.c
rm -f $project_name
bison -d $project_name.y -Wcounterexamples
lex $project_name.l
g++ SymbolTable.cpp lex.yy.c types.cpp AST.cpp $project_name.tab.c -std=c++20 -o $project_name
