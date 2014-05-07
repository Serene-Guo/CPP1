#!/bin/bash
clear
cd $BAPHOME/project; 
for file2 in `ls`; 
	do $BAPHOME/bin/BAPS $file2; 
done