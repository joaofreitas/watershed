TOPDIR= .

include ${TOPDIR}/Makefile.conf

# All subdirectories used for compilation
SUBDIRS = comm common console library runtime scheduler stream

# Objects
RUNTIME_OBJS = comm/*.o comm/mpi/*.o common/*.o library/configurator.o library/input_flow.o library/processing_module_entry.o library/xml.o runtime/*.o scheduler/*.o
CONSOLE_OBJS = comm/*.o comm/mpi/*.o common/*.o console/*.o
LIBRARY_OBJS = library/processing_module.o library/configurator.o library/input_flow.o library/label_function.o comm/*.o comm/mpi/*.o common/*.o
STREAM_OBJS = common/*.o comm/*.o comm/mpi/*.o library/configurator.o library/input_flow.o library/processing_module_entry.o stream/*.o
PROCESSING_MODULE_OBJS = comm/*.o comm/mpi/*.o common/*.o library/configurator.o library/data_consumer.o library/data_producer.o library/input_flow.o library/processing_module.o library/label_function.o library/main.o library/xml.o

# Phony rules
.PHONY: all clean install ${SUBDIRS}

# Defaul action, build the library
all: ${REAL_NAME}

${REAL_NAME}: ${SUBDIRS}
	@echo ""
	@echo "\tCompiling\t$@"
	@$(MPICPP) -fPIC -shared ${LDFLAGS} ${LIBRARY_OBJS} -o ${REAL_NAME}
	@echo "\tCreating\t${SO_NAME}"
	@ln -sf ${REAL_NAME} ${SO_NAME}
	@echo "\tCreating\t${LINKER_NAME}"
	@ln -sf ${REAL_NAME} ${LINKER_NAME}
	@echo "\tCompiling\t${SERVER_NAME}"
	@$(MPICPP) ${LDFLAGS} $(RUNTIME_OBJS) -o ${SERVER_NAME}
	@echo "\tCompiling\t${CONSOLE_NAME}"
	@$(MPICPP) ${LDFLAGS} ${CONSOLE_OBJS} -o ${CONSOLE_NAME}
	@echo "\tCompiling\t${STREAM_NAME}"
	@$(MPICPP) ${LDFLAGS} ${STREAM_OBJS} -o ${STREAM_NAME}
	@echo "\tCompiling\t${PROCESSING_MODULE_NAME}"
	@$(MPICPP) ${LDFLAGS} ${PROCESSING_MODULE_OBJS} -o ${PROCESSING_MODULE_NAME}

# Call subdirectories
${SUBDIRS}:
	@echo ""
	@make -C $@

# Install rule: install the lib and the header files
install: ${REAL_NAME}
	@echo "\tInstalling\tWatershed"
	@mkdir -p ${PREFIX}/bin
	@mkdir -p ${PREFIX}/include
	@mkdir -p ${PREFIX}/include/comm
	@mkdir -p ${PREFIX}/include/comm/mpi
	@mkdir -p ${PREFIX}/include/common
	@mkdir -p ${PREFIX}/include/library
	
	@mkdir -p ${PREFIX}/lib
	@cp -p ${REAL_NAME} ${SO_NAME} ${LINKER_NAME} ${PREFIX}/lib
	@sed -i "s,STARTUP_FILE=.*,STARTUP_FILE=${PREFIX}/${STARTUP_FILE}," ${SERVER_SCRIPT}
	@cp -p ${PROCESSING_MODULE_NAME} ${SERVER_NAME} ${CONSOLE_NAME} ${STREAM_NAME} ${PREFIX}/bin
	@cp -p ${SERVER_SCRIPT} ${STARTUP_DTD_FILE} ${STARTUP_FILE} ${PROCESSING_MODULE_DTD_FILE} ${PREFIX}
	@cp -p comm/*.h ${PREFIX}/include/comm
	@cp -p comm/mpi/*.h ${PREFIX}/include/comm/mpi
	@cp -p common/*.h ${PREFIX}/include/common
	@cp -p library/*.h ${PREFIX}/include/library
	@mv ${PREFIX}/include/library/watershed.h ${PREFIX}/include
	@echo "\tDone\n"
	
# Clean rule
clean:
	@echo ""
	@make -C comm clean
	@echo ""
	@make -C common clean
	@echo ""
	@make -C console clean
	@echo ""
	@make -C library clean
	@echo ""
	@make -C runtime clean
	@echo ""
	@make -C scheduler clean
	@echo ""
	@make -C stream clean
	@echo ""
	rm -f *.so *.so.* *.o ${SERVER_NAME} ${CONSOLE_NAME} ${STREAM_NAME} ${PROCESSING_MODULE_NAME}
