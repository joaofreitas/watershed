TOPDIR= ../..

include ${TOPDIR}/Makefile.conf

.PHONY: all test clean 

all: mpi_communicator.o

mpi_communicator.o: mpi_communicator.cc mpi_communicator.h
	@echo "\tCompiling\t$<"
	@$(MPICPP) $(CFLAGS) -c mpi_communicator.cc -o mpi_communicator.o	
	
test: mpi_test.cc mpi_communicator.o
	@echo "\tCompiling\t$<"
	@$(MPICPP) $(CFLAGS) -c mpi_test.cc
	@$(MPICPP) ${CLIBSDIR} ${CLIBS} *.o -o mpi_test

clean:
	rm -f *.so *.o mpi_test
