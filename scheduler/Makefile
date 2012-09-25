TOPDIR= ..
include ${TOPDIR}/Makefile.conf

.PHONY: all clean

all: scheduler.o scheduler_operation.o

scheduler.o: scheduler.cc scheduler.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c scheduler.cc 
	
scheduler_operation.o: scheduler_operation.cc scheduler_operation.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c scheduler_operation.cc

clean:
	rm -f *.o
