TOPDIR= ..
include ${TOPDIR}/Makefile.conf

.PHONY: all ${SUBDIRS} clean

all: constants.o logger.o parser_error_handler.o util.o xml_parser.o xml_query.o 

constants.o: constants.cc constants.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c constants.cc

logger.o: logger.cc logger.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c logger.cc	
	
parser_error_handler.o: parser_error_handler.cc parser_error_handler.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c parser_error_handler.cc
	
util.o: util.cc util.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c util.cc	
	
xml_parser.o: xml_parser.cc xml_parser.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c xml_parser.cc

xml_query.o: xml_query.cc xml_query.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c xml_query.cc

clean:
	rm -f *.so *.o	
