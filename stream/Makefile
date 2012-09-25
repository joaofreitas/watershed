TOPDIR= ..
include ${TOPDIR}/Makefile.conf

OBJS = main.o stream_manager.o

.PHONY: all clean

all: ${OBJS}

main.o: main.cc
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c main.cc
	
stream_manager.o: stream_manager.cc stream_manager.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c stream_manager.cc	
		
clean:
	rm -f *.o ${TARGET}
