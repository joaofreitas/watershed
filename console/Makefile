TOPDIR= ..
include ${TOPDIR}/Makefile.conf

.PHONY: all clean

all: console.o main.o

console.o: console.cc console.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c console.cc 
	
main.o: main.cc
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c main.cc	

clean:
	rm -f *.o
