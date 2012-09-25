/**
 * \file comm/mpi/mpi_communicator.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "comm/mpi/mpi_communicator.h"

int MpiCommunicator::number_instances_ = 0;

MpiCommunicator::MpiCommunicator ( int argc, char** argv ) {
	pthread_mutex_init ( &class_mutex_, NULL );
	intra_communicator_ = MPI::COMM_NULL;

	/* The first instance must initialize MPI and all_process communicator. */
	if ( number_instances_ == 0 ) {
		//		MPI::Init ( argc, argv );
		MPI::Init_thread ( argc, argv, MPI_THREAD_MULTIPLE );
	}

	/* If this process was spawned, get the parent inter-communicator. */
	MPI::Intercomm parent = MPI::Comm::Get_parent ( );
	inter_communicator_ = parent;
	inter_communicator_.Set_errhandler ( MPI::ERRORS_THROW_EXCEPTIONS );

	++number_instances_;
}

MpiCommunicator::MpiCommunicator ( int argc, char** argv, int scope ) {
	pthread_mutex_init ( &class_mutex_, NULL );
	intra_communicator_ = MPI::COMM_NULL;
	inter_communicator_ = MPI::COMM_NULL;

	/* The first instance must initialize MPI and all_process communicator. */
	if ( number_instances_ == 0 ) {
		//		MPI::Init ( argc, argv );
		MPI::Init_thread ( argc, argv, MPI_THREAD_MULTIPLE );
	}

	/* If this process was spawned, get the parent inter-communicator. */
	MPI::Intercomm parent = MPI::Comm::Get_parent ( );

	/* If this process was spawned. */
	if ( parent != MPI::COMM_NULL ) {
		/* Creates an intra-communicator that includes the parent. */
		intra_communicator_ = MPI::COMM_WORLD;
	}
	else { /* If this process wasn't spawned. */
		if ( scope == Constants::COMM_SCOPE_WORLD ) {
			intra_communicator_ = MPI::COMM_WORLD;
		}
		else if ( scope == Constants::COMM_SCOPE_SELF ) {
			intra_communicator_ = MPI::COMM_SELF;
		}
	}
	intra_communicator_.Set_errhandler ( MPI::ERRORS_THROW_EXCEPTIONS );
	++number_instances_;
}

MpiCommunicator::MpiCommunicator ( MPI::Intracomm intracomm, MPI::Intercomm intercomm ) {
	pthread_mutex_init ( &class_mutex_, NULL );
	int argc = 0;
	char** argv = NULL;
	/* The first instance must initialize MPI and all_process communicator. */
	if ( number_instances_ == 0 ) {
		//		MPI::Init ( argc, argv );
		MPI::Init_thread ( argc, argv, MPI_THREAD_MULTIPLE );
	}

	/* If this process was spawned, get the parent inter-communicator. */
	//		MPI::Intercomm parent = MPI::Comm::Get_parent ();

	intra_communicator_ = intracomm;
	inter_communicator_ = intercomm;
	if ( intra_communicator_ != MPI::COMM_NULL ) {
		intra_communicator_.Set_errhandler ( MPI::ERRORS_THROW_EXCEPTIONS );
	}
	if ( inter_communicator_ != MPI::COMM_NULL ) {
		inter_communicator_.Set_errhandler ( MPI::ERRORS_THROW_EXCEPTIONS );
	}
	++number_instances_;
}

MpiCommunicator::~MpiCommunicator ( void ) {
	pthread_mutex_destroy ( &class_mutex_ );

	--number_instances_;

	/* The last instance must finalize MPI */
	if ( number_instances_ == 0 ) {
		MPI::Finalize ( );
	}
}

MpiCommunicator* MpiCommunicator::Accept ( string port_name ) {
	MpiCommunicator* new_mpi_communicator;
	MPI::Intercomm server_client_communicator;
	server_client_communicator = intra_communicator_.Accept ( port_name.c_str ( ), MPI::INFO_NULL, Constants::COMM_ROOT_PROCESS );
	new_mpi_communicator = new MpiCommunicator ( MPI::COMM_NULL, server_client_communicator );
	return new_mpi_communicator;
}

void MpiCommunicator::AllGather ( Message* output_message, Message* input_messages ) {
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Allgather ( output_message, sizeof(Message), MPI::BYTE, input_messages, sizeof(Message), MPI::BYTE );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Allgather ( output_message, sizeof(Message), MPI::BYTE, input_messages, sizeof(Message), MPI::BYTE );
		}
	}
	catch ( MPI::Exception e ) {

	}
}

void MpiCommunicator::BroadCast ( Message* data ) throw ( BadParameterException ) {

	string err_message;

	/* Check parameters */
	if ( data == NULL ) {
		err_message = "parameter data is NULL.";
		throw BadParameterException ( err_message );
	}
	try {
		for ( int i = 0; i < GetNumberProcesses ( ); ++i ) {
			Send ( data, i );
		}
	}
	catch ( MPI::Exception e ) {

	}
}

MpiCommunicator* MpiCommunicator::Connect ( string port ) {
	MPI::Intercomm server_client_communicator;
	MpiCommunicator* new_mpi_communicator;

	try {
		server_client_communicator = intra_communicator_.Connect ( port.c_str ( ), MPI::INFO_NULL, Constants::COMM_ROOT_PROCESS );
		new_mpi_communicator = new MpiCommunicator ( MPI::COMM_NULL, server_client_communicator );
	}
	catch ( MPI::Exception e ) {

	}
	return new_mpi_communicator;
}

void MpiCommunicator::ClosePort ( string port_name ) {
	char port[MPI_MAX_PORT_NAME];
	strcpy ( port, port_name.c_str ( ) );
	try {
		MPI::Close_port ( port );
	}
	catch ( MPI::Exception e ) {

	}
}

void MpiCommunicator::Disconnect ( void ) {
	inter_communicator_.Disconnect ( );
}

string MpiCommunicator::GetHostName ( void ) {
	char name[100];
	int result_lenght;
	string host_name;

	try {
		MPI::Get_processor_name ( name, result_lenght );
	}
	catch ( MPI::Exception e ) {

	}
	host_name = name;
	return host_name;
}

int MpiCommunicator::GetNumberProcesses ( void ) {
	int number_process;
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			number_process = intra_communicator_.Get_size ( );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			number_process = inter_communicator_.Get_remote_size ( );
		}
	}
	catch ( MPI::Exception e ) {

	}
	return number_process;
}

int MpiCommunicator::GetProcessRank ( void ) {
	int rank = -1;
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			rank = intra_communicator_.Get_rank ( );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			rank = inter_communicator_.Get_rank ( );
		}
	}
	catch ( MPI::Exception e ) {

	}
	return rank;
}

void MpiCommunicator::Lock ( void ) {
	pthread_mutex_lock ( &class_mutex_ );
}

string MpiCommunicator::OpenPort ( void ) {
	char port[MPI_MAX_PORT_NAME];
	try {
		MPI::Open_port ( MPI::INFO_NULL, port );
	}
	catch ( MPI::Exception e ) {

	}
	string port_name ( port );
	return port_name;
}

int MpiCommunicator::Poll ( int source, int tag ) throw ( BadParameterException ) {

	string err_message;

	/* Check tag. */
	int mpi_tag = -1;
	if ( tag == Constants::MESSAGE_OP_ANY ) {
		mpi_tag = MPI::ANY_TAG;
	}
	else {
		if ( tag <= 0 ) {
			err_message = "parameter tag is not valid.";
			throw BadParameterException ( err_message );
		}
		mpi_tag = tag;
	}

	/* Check source. */
	int mpi_source = -1;
	if ( source == Constants::COMM_ANY_SOURCE ) {
		mpi_source = MPI::ANY_SOURCE;
	}
	else {
		if ( source < 0 || source > GetNumberProcesses ( ) - 1 ) {
			err_message = "parameter source is not in process group.";
			throw BadParameterException ( err_message );
		}
		mpi_source = source;
	}

	MPI::Status status;
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Probe ( mpi_source, mpi_tag, status );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Probe ( mpi_source, mpi_tag, status );
		}
	}
	catch ( MPI::Exception e ) {
	}

	return status.Get_source ( );
}

int MpiCommunicator::Probe ( int source, int tag ) throw ( BadParameterException ) {

	string err_message;

	/* Check tag. */
	int mpi_tag = -1;
	if ( tag == Constants::MESSAGE_OP_ANY ) {
		mpi_tag = MPI::ANY_TAG;
	}
	else {
		if ( tag <= 0 ) {
			err_message = "parameter tag is not valid.";
			throw BadParameterException ( err_message );
		}
		mpi_tag = tag;
	}

	/* Check source. */
	int mpi_source = -1;
	if ( source == Constants::COMM_ANY_SOURCE ) {
		mpi_source = MPI::ANY_SOURCE;
	}
	else {
		if ( source < 0 || source > GetNumberProcesses ( ) - 1 ) {
			err_message = "parameter source is not in process group.";
			throw BadParameterException ( err_message );
		}
		mpi_source = source;
	}

	MPI::Status status;
	bool has_message;
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			has_message = intra_communicator_.Iprobe ( mpi_source, mpi_tag, status );
			has_message = intra_communicator_.Iprobe ( mpi_source, mpi_tag, status );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			has_message = inter_communicator_.Iprobe ( mpi_source, mpi_tag, status );
			has_message = inter_communicator_.Iprobe ( mpi_source, mpi_tag, status );
		}
	}
	catch ( MPI::Exception e ) {
	}

	if ( !has_message ) {
		return -1;
	}

	return status.Get_source ( );
}

int MpiCommunicator::Receive ( int source, Message* data ) throw ( BadParameterException ) {

	string err_message;

	/* Check parameters. */
	if ( data == NULL ) {
		err_message = "parameter data or size is not valid.";
		throw BadParameterException ( err_message );
	}

	/* Check source. */
	int mpi_source = -1;
	if ( source == Constants::COMM_ANY_SOURCE ) {
		mpi_source = MPI::ANY_SOURCE;
	}
	else {
		if ( source < 0 || source > GetNumberProcesses ( ) - 1 ) {
			err_message = "parameter source is not in process group.";
			throw BadParameterException ( err_message );
		}
		mpi_source = source;
	}

	/* Check tag. */
	int tag = data->GetOperationCode ( );
	int mpi_tag = -1;
	if ( tag == Constants::MESSAGE_OP_ANY ) {
		mpi_tag = MPI::ANY_TAG;
	}
	else {
		if ( tag < 0 ) {
			err_message = "parameter tag is not valid.";
			throw BadParameterException ( err_message );
		}
		mpi_tag = tag;
	}

	int size = sizeof(Message);
	MPI::Status operation_status;
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Recv ( data, size, MPI::BYTE, mpi_source, mpi_tag, operation_status );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Recv ( data, size, MPI::BYTE, mpi_source, mpi_tag, operation_status );
		}
	}
	catch ( MPI::Exception e ) {

	}
	data->SetSource ( operation_status.Get_source ( ) );
	return operation_status.Get_source ( );
}

void MpiCommunicator::RemoveProcess ( int process_rank ) {
	if ( intra_communicator_ != MPI::COMM_NULL ) {
		MPI::Intracomm tmp_intra_comm;
		tmp_intra_comm = intra_communicator_.Create ( intra_communicator_.Get_group ( ).Excl ( 1, &process_rank ) );
		intra_communicator_ = tmp_intra_comm;
	}
	else if ( inter_communicator_ != MPI::COMM_NULL ) {
		MPI::Intercomm tmp_inter_comm;
		if ( process_rank == Constants::PROCESSING_MODULE_INVALID_INSTANCE ) {
			tmp_inter_comm = inter_communicator_.Create ( inter_communicator_.Get_group ( ) );
		}
		else {
			tmp_inter_comm = inter_communicator_.Create ( inter_communicator_.Get_group ( )/*.Excl ( 1, &process_rank )*/ );
		}
		inter_communicator_.Free ( );
		inter_communicator_ = tmp_inter_comm;
	}
}

void MpiCommunicator::SBroadCast ( Message* data ) throw ( BadParameterException ) {

	string err_message;

	/* Check parameters */
	if ( data == NULL ) {
		err_message = "parameter data is NULL.";
		throw BadParameterException ( err_message );
	}
	try {
		for ( int i = 0; i < GetNumberProcesses ( ); ++i ) {
			SSend ( data, i );
		}
	}
	catch ( MPI::Exception e ) {

	}
}

void MpiCommunicator::Send ( Message* data, int destination ) throw ( BadParameterException ) {

	string err_message;
	time_t timestamp = time ( NULL );

	/* Check parameters. */
	if ( data == NULL ) {
		err_message = "parameter data is not valid.";
		throw BadParameterException ( err_message );
	}
	/* Check destination. */
	if ( destination < 0 || destination > GetNumberProcesses ( ) - 1 ) {
		err_message = "parameter destination is not valid.";
		throw BadParameterException ( err_message );
	}

	try {
		data->SetTimestamp ( timestamp );
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Send ( data, data->GetSize ( ), MPI::BYTE, destination, data->GetOperationCode ( ) );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Send ( data, data->GetSize ( ), MPI::BYTE, destination, data->GetOperationCode ( ) );
		}

	}
	catch ( MPI::Exception e ) {
	}
}

void MpiCommunicator::SSend ( Message* data, int destination ) throw ( BadParameterException ) {

	string err_message;
	time_t timestamp = time ( NULL );

	/* Check parameters. */
	if ( data == NULL ) {
		err_message = "parameter data is not valid.";
		throw BadParameterException ( err_message );
	}
	/* Check destination. */
	if ( destination < 0 || destination > GetNumberProcesses ( ) - 1 ) {
		err_message = "parameter destination is not valid.";
		throw BadParameterException ( err_message );
	}

	try {
		data->SetTimestamp ( timestamp );
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Ssend ( data, data->GetSize ( ), MPI::BYTE, destination, data->GetOperationCode ( ) );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Ssend ( data, data->GetSize ( ), MPI::BYTE, destination, data->GetOperationCode ( ) );
		}

	}
	catch ( MPI::Exception e ) {
	}
}

MpiCommunicator* MpiCommunicator::Spawn ( vector < vector < string > > & argv, vector < string >& commands, vector < string >& hosts, int* number_process, string& work_directory ) throw ( ProcessSpawnningException ) {

	int spawned = 0; // number of spawned process
	char* cmd[commands.size ( )]; // commands to be spawned
	char **arguments[argv.size ( )]; // arguments to be passed to the process
	MPI::Info info[hosts.size ( )]; // mpi info objects
	MPI::Intercomm children_intercomm;
	string err_message;

	if ( commands.size ( ) != hosts.size ( ) || commands.size ( ) != argv.size ( ) ) {
		err_message = "number of hosts is different from number of commands.";
		throw BadParameterException ( err_message );
	}

	/* Creates the data structures required by the mpi spawn_multiple command. */
	for ( int i = 0; i < ( int ) hosts.size ( ); ++i ) {
		/* Commands */
		cmd[i] = ( char* ) malloc ( sizeof(char) * commands[i].length ( ) );
		strcpy ( cmd[i], commands[i].c_str ( ) );

		/* Commands' arguments */
		arguments[i] = ( char** ) malloc ( sizeof(char*) * ( argv[i].size ( ) + 1 ) );
		for ( int j = 0; j < ( int ) argv[i].size ( ); ++j ) {
			arguments[i][j] = ( char* ) malloc ( sizeof(char) * argv[i][j].length ( ) );
			strcpy ( arguments[i][j], argv[i][j].c_str ( ) );
		}
		arguments[i][argv[i].size ( )] = NULL;

		/* Infos */
		info[i] = MPI::Info::Create ( );
		info[i].Set ( "host", hosts[i].c_str ( ) );
		info[i].Set ( "wdir", work_directory.c_str ( ) );
	}

	try {

		/* Tries to spawn the processes. */
		children_intercomm = intra_communicator_.Spawn_multiple ( hosts.size ( ), ( const char** ) cmd, ( const char*** ) arguments, number_process, info, Constants::COMM_ROOT_PROCESS );

		/* Get how many process were successful spawned. */
		spawned += children_intercomm.Get_remote_size ( );
	}
	catch ( MPI::Exception e ) {
		err_message = "it was not possible to spawn the process.";
		throw BadParameterException ( err_message );
	}

	/* Releases the used memory space. */
	for ( int i = 0; i < ( int ) hosts.size ( ); ++i ) {
		info[i].Free ( );
		free ( cmd[i] );
		for ( int j = 0; j < ( int ) argv[i].size ( ); ++j ) {
			free ( arguments[i][j] );
		}
		free ( arguments[i] );
	}
	MpiCommunicator* new_communicator = new MpiCommunicator ( MPI::COMM_NULL, children_intercomm );
	return new_communicator;
}

void MpiCommunicator::Synchronize ( void ) {
	try {
		if ( intra_communicator_ != MPI::COMM_NULL ) {
			intra_communicator_.Barrier ( );
		}
		else if ( inter_communicator_ != MPI::COMM_NULL ) {
			inter_communicator_.Barrier ( );
		}
	}
	catch ( MPI::Exception e ) {
	}
}

void MpiCommunicator::Unlock ( void ) {
	pthread_mutex_unlock ( &class_mutex_ );
}
