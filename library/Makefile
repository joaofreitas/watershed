TOPDIR= ..
include ${TOPDIR}/Makefile.conf

.PHONY: all clean

all: configurator.o data_consumer.o data_producer.o input_flow.o label_function.o main.o processing_module.o processing_module_entry.o xml.o
	
configurator.o: configurator.cc configurator.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c configurator.cc
	
data_consumer.o: data_consumer.cc data_consumer.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c data_consumer.cc
	
data_producer.o: data_producer.cc data_producer.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c data_producer.cc
	
input_flow.o: input_flow.cc input_flow.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c input_flow.cc	

label_function.o: label_function.cc label_function.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c label_function.cc

main.o: main.cc
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c main.cc	

processing_module.o: processing_module.cc processing_module.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c processing_module.cc
	
processing_module_entry.o: processing_module_entry.cc processing_module_entry.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -c processing_module_entry.cc
	
xml.o: xml.cc xml.h
	@echo "\tCompiling\t$<"
	@${MPICPP} ${CFLAGS} -fPIC -c xml.cc	
	
clean:
	rm -f *.o
