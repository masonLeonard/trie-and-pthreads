# Makefile for a2, CS480
# Mason Leonard 818030805

PROGRAM = dicttest
CC = gcc
CFLAGS = -std=c11 -g 


${PROGRAM}:      countwords.o dicttree.o populatetree.o main.o
	${CC} -pthread -o dicttest countwords.o dicttree.o populatetree.o main.o


countwords.o:	dicttree.h

dictttree.o:	dicttree.h

populatetree.o:		dicttree.h
 
main.o:		dicttree.h

splint:
	splint -warnposix +trytorecover -weak ${PROGRAM}.c

clean:
	rm -f *.o ${PROGRAM}

