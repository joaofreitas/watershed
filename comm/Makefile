TOPDIR= ..
include ${TOPDIR}/Makefile.conf

SUBDIRS = mpi

.PHONY: all ${SUBDIRS} clean

all: ${SUBDIRS} message.o

${SUBDIRS}:
	@echo ""
	@make -C $@
	@echo ""
	
message.o: message.cc message.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c message.cc	

clean:
	@echo ""
	@make -C mpi clean
	@echo ""
	rm -f *.so *.o	
