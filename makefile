###########################################################
#
#	Makefile for Computer Graphics Assignment 2
#	@file assignment1.cpp
#	@author Aidan Becker
#	@islandID ABecker2
#	@professor Dr. Scott King
#	@class COSC 4328-001 Computer Graphics
#	@version 1.0
#
###########################################################
Compiler =g++  -std=c++11
LDLIBS =-lGLEW -lGL -lX11 -lglfw
Remove =rm
Object =AidanBeckerAssignment3.cpp -o
Name =AidanBeckerAssignment3.out
	
assignment5:
	$(Compiler) $(Object) $(Name) $(LDLIBS)
	
clean:
	$(Remove) $(Name)

run:
	$(Compiler) $(Object) $(Name) $(LDLIBS)
	./$(Name)

remake:
	$(Remove) $(Name)
	$(Compiler) $(Object) $(Name) $(LDLIBS)
	./$(Name)