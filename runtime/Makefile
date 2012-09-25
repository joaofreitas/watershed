TOPDIR= ..
include ${TOPDIR}/Makefile.conf

OBJS = configurator.o host.o main.o runtime.o

.PHONY: all clean

all: ${OBJS}

configurator.o: configurator.cc configurator.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c configurator.cc

host.o: host.cc host.h	
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c host.cc
	
main.o: main.cc
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c main.cc
	
runtime.o: runtime.cc runtime.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c runtime.cc		

clean:
	rm -f *.o
