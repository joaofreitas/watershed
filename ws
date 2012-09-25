#!/bin/bash
#
# Run level script to start StreamServer services on Ubuntu Linux
# Script should work on other UNIX like oses :)
# -------------------------------------------------------------------------
# description: Startup/Shutdown Watershed

function check_server_status() {
	local NOW=`date '+[%Y-%m-%d %H:%M:%S]'`
			
	if [ "$(pidof $SERVER_NAME)" ] 
	then		
		echo -e $NOW "- system running"
	else
		echo -e $NOW "- system not running"
	fi
}

# Force open-mpi degraded mode
export OMPI_MCA_mpi_yield_when_idle="1"

# Extracts some variables from the startup file
STARTUP_FILE=/home/rodrigo/libwatershed/ws-startup.xml
OMPI_PREFIX=`cat $STARTUP_FILE | grep "prefix " | sed 's/=/ /g' | sed 's/"/ /g' | awk '{print $2}'`
WATERSHED_HOME=`cat $STARTUP_FILE | grep "home " | sed 's/=/ /g' | sed 's/"/ /g' | awk '{print $2}'`
RUNNING_DIR=`cat $STARTUP_FILE | grep "running_dir " | sed 's/=/ /g' | sed 's/"/ /g' | awk '{print $2}'`

# Some server configuration
INFO_FILE=$RUNNING_DIR/watershed.info
BIN_PATH=${WATERSHED_HOME}/bin
SERVER_NAME=ws-manager
CONSOLE_NAME=${BIN_PATH}/ws-console
HOST_FILE=${WATERSHED_HOME}/hostfile

# Generates the hostfile
cat $STARTUP_FILE | grep -v "hostdec" | grep -v "hostfile" | grep "<host" | sed 's/=/ /g' | sed 's/"/ /g' | awk '{if($2=="name"){print $3}else{print $5}}'  > $HOST_FILE
NUMBER_OF_SERVER_DAEMONS=`cat $HOST_FILE | wc -l`
NUMBER_OF_SERVER_DAEMONS=1

case "$1" in

	check)
	mpirun -v --prefix $OMPI_PREFIX --mca btl_tcp_use_nagle 1 -np $NUMBER_OF_SERVER_DAEMONS --hostfile $HOST_FILE valgrind --tool=memcheck --leak-check=yes $BIN_PATH/$SERVER_NAME -i $STARTUP_FILE &
	;;

	start)
	mpirun -v --prefix $OMPI_PREFIX --mca btl_tcp_use_nagle 1 -mca mpi_yield_when_idle 1 -np $NUMBER_OF_SERVER_DAEMONS --hostfile $HOST_FILE $BIN_PATH/$SERVER_NAME -i $STARTUP_FILE &
	;;
	
	stop)
	mpirun -v --prefix $OMPI_PREFIX -np 1 $CONSOLE_NAME -i $INFO_FILE -c "shutdown" -a "  "
	mpirun --pernode --hostfile $HOST_FILE orte-clean --verbose
	;;

	restart)
	$0 stop
	$0 start
	;;
	
	status)
	echo
	check_server_status
	echo 
	;;
	
	add-module)
	echo
	mpirun -v --prefix $OMPI_PREFIX -np 1 $CONSOLE_NAME -i $INFO_FILE -c $1 -a $2
	echo 
	;;
	
	remove-module)
	echo
	mpirun -v --prefix $OMPI_PREFIX -np 1 $CONSOLE_NAME -i $INFO_FILE -c $1 -a $2
	echo 
	;;
	
	remove-instance)
	echo
	mpirun -v --prefix $OMPI_PREFIX -np 1 $CONSOLE_NAME -i $INFO_FILE -c $1 -a $2 $3
	echo
	;;
	
	*)
	echo -e $"\nUsage: $0 {start|stop|restart|status|console [command]}"
	echo -e "\n"	
	exit 1
esac

exit 0
