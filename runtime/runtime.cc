/**
 * \file runtime/runtime.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "runtime/runtime.h"

Runtime::Runtime ( int argc, char** argv ) {

	try {
		GetParameters ( argc, argv );
		shutdown_notification_ = false;

		pthread_mutex_init ( &shutdown_notification_mutex_, NULL );

		cluster_communicator_ = new MpiCommunicator ( argc, argv, Constants::COMM_SCOPE_WORLD );
		self_communicator_ = new MpiCommunicator ( argc, argv, Constants::COMM_SCOPE_SELF );
		ReadConfigurationFile ( );
		LockLocalResources ( );

		if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
			runtime_logger_ = new Logger ( runtime_configurator_->GetLogFile ( ), true, "......................................................................................." );
			cout << endl << endl;
			runtime_logger_->PrintScreenSeparator ( );
			runtime_logger_->PrintInfo ( "starting " + Constants::SYSTEM_NAME + " daemons" );
		}

		server_port_ = self_communicator_->OpenPort ( );
		FILE* info_file = fopen ( runtime_configurator_->GetInfoFile ( ).c_str ( ), "w" );
		fprintf ( info_file, "%s", server_port_.c_str ( ) );
		fclose ( info_file );
	}
	catch ( exception& e ) {
		throw e;
	}
}

Runtime::~Runtime ( void ) {
	cluster_communicator_->Synchronize ( );
	self_communicator_->ClosePort ( server_port_ );
	if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		delete ( runtime_logger_ );
	}
	delete ( runtime_configurator_ );
	delete ( database_communicator_ );
	delete ( self_communicator_ );
	delete ( cluster_communicator_ );
	pthread_mutex_destroy ( &shutdown_notification_mutex_ );
}

void Runtime::AddProcessingModule ( Message& received_message ) throw ( ProcessSpawnningException, XMLParserException ) {

	string processing_module_configurator_file = ( char* ) received_message.GetData ( );
	ProcessingModuleConfigurator* processing_module_configurator;

	/* Creates an initial processing module configurator based on a XML configuration file. */
	try {
		processing_module_configurator = new ProcessingModuleConfigurator ( processing_module_configurator_file );
		processing_module_configurator->SetDatabasePortName ( database_port_name_ );
		processing_module_configurator->SetConfiguratorFileName ( processing_module_configurator_file );
	}
	catch ( XMLParserException& e ) {
		throw ( e );
	}

	/* Checks whether the processing module is already running on the system. */
	if ( ProcessingModuleRunning ( processing_module_configurator->GetName ( ) ) ) {
		string msg = "processing module " + processing_module_configurator->GetName ( ) + " already running on " + Constants::SYSTEM_NAME;
		delete ( processing_module_configurator );
		throw ProcessSpawnningException ( msg );
	}

	/* Checks the required number of instances */
	if ( processing_module_configurator->GetNumberInstances ( ) <= 0 && processing_module_configurator->GetNumberInstances ( ) != Constants::PROCESSING_MODULE_AUTOMATIC_NUMBER_INSTANCES ) {
		string msg = "number of required instances must be greater than 0";
		delete ( processing_module_configurator );
		throw ProcessSpawnningException ( msg );
	}

	/* Chooses hosts to receive the processing module instances. */
	database_communicator_->Lock ( );
	map < string, int > scheduler_result = runtime_scheduler_.ChooseHostsToAddProcessingModule ( runtime_configurator_->GetHosts ( ), database_communicator_, processing_module_configurator );
	database_communicator_->Unlock ( );

	if ( scheduler_result.size ( ) == 0 ) {
		string msg = "it was not possible to create the processing module instances because there are no hosts with the demanded resources";
		delete ( processing_module_configurator );
		throw ProcessSpawnningException ( msg );
	}

	/* Tries to spawn the processing module instances. */
	MpiCommunicator* new_module_communicator = SpawnProcessingModuleInstances ( processing_module_configurator, &scheduler_result );

	/* Notifies the database manager to accept the connection from the new PM. */
	Message m ( NULL, Constants::MESSAGE_OP_ACCEPT_CONNECT, 0 );
	database_communicator_->BroadCast ( &m );

	/* Sends the name of the XML configurator file, the database port name and the database instance identification */
	string init_message_data = processing_module_configurator->GetConfiguratorFileName ( ) + "\t" + processing_module_configurator->GetDatabasePortName ( ) + "\t" + Util::IntegerToString ( runtime_scheduler_.GetLastAssignedDatabase ( ) );
	Message processing_module_init_msg ( ( void* ) init_message_data.c_str ( ), Constants::MESSAGE_OP_INIT_PROCESSING_MODULE, init_message_data.length ( ) + 1 );
	new_module_communicator->BroadCast ( &processing_module_init_msg );

	/* Receives the name of the port opened by the processing module instances or an error message. */
	Message message_from_module;
	int source = new_module_communicator->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
	message_from_module.SetOperationCode ( Constants::MESSAGE_OP_ANY );
	new_module_communicator->Receive ( source, &message_from_module );

	if ( message_from_module.GetOperationCode ( ) == Constants::MESSAGE_OP_PORT_NAME ) {
		processing_module_configurator->SetPortName ( ( char* ) message_from_module.GetData ( ) );
		ProcessingModuleEntry* new_entry = new ProcessingModuleEntry ( new_module_communicator, processing_module_configurator );

		/* Update the local table with the new processing module information */
		ProcessingModuleEntry::Lock ( );
		active_processing_modules_[processing_module_configurator->GetName ( )] = new_entry;
		ProcessingModuleEntry::Unlock ( );
	}
	else if ( message_from_module.GetOperationCode ( ) == Constants::MESSAGE_OP_ERROR_LOG ) {
		delete ( new_module_communicator );
		delete ( processing_module_configurator );
		throw ProcessSpawnningException ( ( char* ) message_from_module.GetData ( ) );
	}
}

void Runtime::Daemonize ( void ) {

	int i, lock_file_pid;
	char str[10];

	/* creates a child process and exits the parent */
	if ( getppid ( ) == 1 ) /* already a daemon */
		return;
	i = fork ( );

	if ( i < 0 ) /* fork error */
		exit ( 1 );
	if ( i > 0 ) /* parent exits */
		exit ( 0 );
	setsid ( ); /* obtain a new process group */

	for ( i = getdtablesize ( ); i >= 0; --i ) /* close all descriptors */
		close ( i );

	i = open ( "/dev/null", O_RDWR );
	dup ( i );
	dup ( i ); /* handle standart I/O */

	umask ( 027 ); /* set newly created file permissions */
	cout << runtime_configurator_->GetRunningDir ( );
	chdir ( runtime_configurator_->GetRunningDir ( ).c_str ( ) ); /* change running directory */

	lock_file_pid = open ( runtime_configurator_->GetLockFile ( ).c_str ( ), O_RDWR | O_CREAT, 0640 );

	if ( lock_file_pid < 0 )
		exit ( 1 ); /* can not open */
	if ( lockf ( lock_file_pid, F_TLOCK, 0 ) < 0 )
		exit ( 0 ); /* can not lock */

	/* first instance continues */
	sprintf ( str, "%d\n", getpid ( ) );
	write ( lock_file_pid, str, strlen ( str ) ); /* record pid to lockfile */
	signal ( SIGCHLD, SIG_IGN ); /* ignore child */
	signal ( SIGTSTP, SIG_IGN ); /* ignore tty signals */
	signal ( SIGTTOU, SIG_IGN );
	signal ( SIGTTIN, SIG_IGN );
	//	signal(SIGHUP, HandleSignal);				/* catch hangup signal */
	//	signal(SIGTERM, HandleSignal);				/* catch kill signal */
}

void Runtime::DoProcessingModuleRemoval ( Message& received_message ) {

	string processing_module_name = ( char* ) received_message.GetData ( );
	string message = "removing instances of " + processing_module_name + " from " + Constants::SYSTEM_NAME;
	Util::Information ( cluster_communicator_, message );

	Message shutdown_message;
	shutdown_message.SetOperationCode ( Constants::MESSAGE_OP_SHUTDOWN );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->BroadCast ( &shutdown_message );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->Synchronize ( );
	delete ( active_processing_modules_[processing_module_name] );
	active_processing_modules_.erase ( processing_module_name );
}

void Runtime::DoProcessingModuleInstanceRemoval ( Message& received_message ) {
	string processing_module_name = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetModuleName ( );
	int instance = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetInstanceIdentification ( );
	string message = "removing instance " + Util::IntegerToString ( instance ) + " of " + processing_module_name + " from " + Constants::SYSTEM_NAME;
	Util::Information ( cluster_communicator_, message );

	Message remove_message;
	remove_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_INSTANCE );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->BroadCast ( &remove_message );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->RemoveProcess ( Constants::PROCESSING_MODULE_INVALID_INSTANCE );
}

void Runtime::ExchangeInitialInformation ( void ) {

	IdentificationMessage my_information;
	my_information.SetProcessIdentification ( cluster_communicator_->GetProcessRank ( ) );
	my_information.SetHostName ( cluster_communicator_->GetHostName ( ) );

	Message output_message ( &my_information, Constants::MESSAGE_OP_PRESENTATION, sizeof ( my_information ) );
	Message input_messages[cluster_communicator_->GetNumberProcesses ( )];

	cluster_communicator_->AllGather ( &output_message, input_messages );

	IdentificationMessage* remote_process;
	for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
		remote_process = ( IdentificationMessage* ) input_messages[i].GetData ( );
		( *runtime_configurator_->GetHosts ( ) )[remote_process->GetHostName ( )].SetRuntimeDaemonId ( remote_process->GetProcessIdentification ( ) );
	}

	Message input_database_messages[database_communicator_->GetNumberProcesses ( )];
	database_communicator_->AllGather ( &output_message, input_database_messages );

	for ( int i = 0; i < database_communicator_->GetNumberProcesses ( ); ++i ) {
		remote_process = ( IdentificationMessage* ) input_database_messages[i].GetData ( );
		( *runtime_configurator_->GetHosts ( ) )[remote_process->GetHostName ( )].SetDatabaseDaemonId ( remote_process->GetProcessIdentification ( ) );
	}

	/* Communicates the running directory to the database daemon running at this host. */
	if ( ( *runtime_configurator_->GetHosts ( ) )[database_communicator_->GetHostName ( )].IsDatabaseServer ( ) ) {
		Message db_environment_dir_message ( ( void* ) runtime_configurator_->GetRunningDir ( ).c_str ( ), Constants::MESSAGE_OP_DB_ENVIRONMENT_DIR, runtime_configurator_->GetRunningDir ( ).length ( ) + 1 );
		database_communicator_->Send ( &db_environment_dir_message, ( *runtime_configurator_->GetHosts ( ) )[database_communicator_->GetHostName ( )].GetDatabaseDaemonId ( ) );
	}

	Message port_name_received;
	int source = database_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_PORT_NAME );
	port_name_received.SetOperationCode ( Constants::MESSAGE_OP_PORT_NAME );
	database_communicator_->Receive ( source, &port_name_received );
	database_port_name_ = ( char* ) port_name_received.GetData ( );

	if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		string message = Constants::SYSTEM_NAME + " is ready to work";
		runtime_logger_->PrintInfo ( message );
	}
	cluster_communicator_->Synchronize ( );
}

void Runtime::GetParameters ( int argc, char* argv[] ) {
#define OPTSTR "i:"
	char option;

	while ( ( option = getopt ( argc, argv, OPTSTR ) ) > 0 ) {
		switch ( option ) {
			case 'i' :
				configuration_file_name_ = optarg;
				break;
		}
	}
}

void Runtime::HandleDatabaseMessage ( Message& received_message ) {
	switch ( received_message.GetOperationCode ( ) ) {
		default : {
			break;
		}
	}
}

void Runtime::HandleProcessingModuleMessage ( string processing_module_name, Message& received_message ) {
	switch ( received_message.GetOperationCode ( ) ) {
		case Constants::MESSAGE_OP_ERROR_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintError ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Error ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		case Constants::MESSAGE_OP_INFO_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintInfo ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Information ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		case Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY : {
			QueryProcessingModulePorts ( true, processing_module_name, received_message );
			break;
		}

		case Constants::MESSAGE_OP_TERMINATION : {
			active_processing_modules_[processing_module_name]->GetConfigurator ( )->SetNumberTerminationMessages ( active_processing_modules_[processing_module_name]->GetConfigurator ( )->GetNumberTerminationMessages ( ) + 1 );
			if ( active_processing_modules_[processing_module_name]->GetConfigurator ( )->GetNumberTerminationMessages ( ) == active_processing_modules_[processing_module_name]->GetConfigurator ( )->GetNumberInstances ( ) ) {
				RemoveProcessingModule ( true, received_message );
			}
			break;
		}

		case Constants::MESSAGE_OP_WARNING_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintWarning ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Warning ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		default : {
			break;
		}
	}
}

void Runtime::HandleRuntimeMessage ( Message& received_message ) {
	int source = received_message.GetSource ( );
	switch ( received_message.GetOperationCode ( ) ) {

		case Constants::MESSAGE_OP_ERROR_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintError ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Error ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		case Constants::MESSAGE_OP_INFO_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintInfo ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Information ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		case Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY : {
			QueryProcessingModulePorts ( false, "", received_message );
			break;
		}

		case Constants::MESSAGE_OP_PROCESSING_MODULE_RUNNING_QUERY : {
			IsOwnerOfProcessingModule ( received_message, source );
			break;
		}

		case Constants::MESSAGE_OP_REMOVE_INSTANCE : {
			RemoveProcessingModuleInstance ( false, received_message );
			break;
		}

		case Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE : {
			RemoveProcessingModule ( false, received_message );
			break;
		}

		case Constants::MESSAGE_OP_SHUTDOWN : {
			Shutdown ( false, received_message );
			break;
		}

		case Constants::MESSAGE_OP_WARNING_LOG : {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintWarning ( ( char* ) received_message.GetData ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Warning ( cluster_communicator_, ( char* ) received_message.GetData ( ) );
			}
			break;
		}

		default : {
			break;
		}
	}
}

void Runtime::HandleSignal ( int signal ) {
	switch ( signal ) {
		case SIGHUP :
			break;
		case SIGTERM :
			exit ( 0 );
			break;
	}
}

void Runtime::IsOwnerOfProcessingModule ( Message received_message, int requester ) {
	bool response = active_processing_modules_.find ( ( char* ) received_message.GetData ( ) ) != active_processing_modules_.end ( );
	Message response_message ( ( void* ) &response, Constants::MESSAGE_OP_PROCESSING_MODULE_RUNNING_ACK, sizeof ( response ) );
	cluster_communicator_->Send ( &response_message, requester );
}

void Runtime::LockLocalResources ( void ) throw ( FileOperationException ) {
	int lock_file_pid;
	char str[10];
	string err_message;

	umask ( 027 ); /* set newly created file permissions */

	/* Attempts to create the running directory. */
	if ( mkdir ( runtime_configurator_->GetRunningDir ( ).c_str ( ), 0750 ) < 0 && errno != EEXIST ) {
		err_message = "running directory " + runtime_configurator_->GetRunningDir ( ) + " could not be created at " + cluster_communicator_->GetHostName ( );
		throw FileOperationException ( err_message ); /* can not open */
	}

	/* Changes to the running directory */
	if ( chdir ( runtime_configurator_->GetRunningDir ( ).c_str ( ) ) < 0 ) { /* change running directory */
		err_message = "running directory " + runtime_configurator_->GetRunningDir ( ) + " could not be open at " + cluster_communicator_->GetHostName ( );
		throw FileOperationException ( err_message ); /* can not open */
	}

	/* Attempts to create the lock file */
	lock_file_pid = open ( runtime_configurator_->GetLockFile ( ).c_str ( ), O_RDWR | O_CREAT, 0640 );
	if ( lock_file_pid < 0 ) {
		err_message = "lock file " + runtime_configurator_->GetLockFile ( ) + " could not be open at " + cluster_communicator_->GetHostName ( );
		throw FileOperationException ( err_message ); /* can not open */
	}
	if ( lockf ( lock_file_pid, F_TLOCK, 0 ) < 0 ) {
		err_message = "lock file " + runtime_configurator_->GetLockFile ( ) + " could not be locked at " + cluster_communicator_->GetHostName ( );
		throw FileOperationException ( err_message ); /* can not lock */
	}
	sprintf ( str, "%d\n", getpid ( ) );
	write ( lock_file_pid, str, strlen ( str ) ); /* record pid to lockfile */
}

void MigrateProcessingModuleInstance ( int instance, string module_name, string target_host ) {
	cout << "Migrando " << module_name << instance << " para " << target_host << endl;
}

bool Runtime::ProcessingModuleRunning ( string processing_module_name ) {
	bool processing_module_already_running = false;
	int received_acks = 0;
	Message output_message;
	Message input_message;

	ProcessingModuleEntry::Lock ( );
	processing_module_already_running = active_processing_modules_.find ( processing_module_name ) != active_processing_modules_.end ( );
	ProcessingModuleEntry::Unlock ( );

	if ( !processing_module_already_running ) {
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_RUNNING_QUERY );
		output_message.SetData ( ( void* ) processing_module_name.c_str ( ), processing_module_name.length ( ) + 1 );

		/* Sends a query to other managers asking if the processing module is running. */
		cluster_communicator_->Lock ( );
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
			if ( i != cluster_communicator_->GetProcessRank ( ) ) {
				cluster_communicator_->Send ( &output_message, i );
			}
		}

		/* Receives the responses. */
		while ( received_acks < cluster_communicator_->GetNumberProcesses ( ) - 1 ) {
			int source = cluster_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_PROCESSING_MODULE_RUNNING_ACK );
			input_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_RUNNING_ACK );
			cluster_communicator_->Receive ( source, &input_message );
			++received_acks;
			processing_module_already_running = processing_module_already_running || * ( ( bool* ) input_message.GetData ( ) );
		}
		cluster_communicator_->Unlock ( );
	}
	return processing_module_already_running;
}

void Runtime::QueryProcessingModulePorts ( bool is_query_manager, string processing_module_name, Message& received_message ) {
	vector < string > processing_module_list = Util::TokenizeString ( " ", ( char* ) received_message.GetData ( ) );
	string message_data = "";

	for ( int i = 0; i < ( int ) processing_module_list.size ( ); ++i ) {
		if ( active_processing_modules_.find ( processing_module_list[i] ) != active_processing_modules_.end ( ) ) {
			Message m ( NULL, Constants::MESSAGE_OP_ACCEPT_CONNECT, 0 );
			active_processing_modules_[processing_module_list[i]]->GetCommunicator ( )->BroadCast ( &m );
			message_data.append ( active_processing_modules_[processing_module_list[i]]->GetConfigurator ( )->GetPortName ( ) );
			message_data.append ( " " );
		}
	}

	if ( is_query_manager ) {

		/* Send the query to other runtime daemons. */
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
			if ( cluster_communicator_->GetProcessRank ( ) != i ) {
				cluster_communicator_->Send ( &received_message, i );
			}
		}

		/* Receives the ack messages from other runtime daemons. */
		Message input_message;
		input_message.SetOperationCode ( Constants::MESSAGE_OP_RUNTIME_QUERY_PROCESSING_MODULE_PORTS_ACK );
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ) - 1; ++i ) {
			int source = cluster_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_RUNTIME_QUERY_PROCESSING_MODULE_PORTS_ACK );
			cluster_communicator_->Receive ( source, &input_message );
			message_data.append ( ( char* ) input_message.GetData ( ) );
			message_data.append ( " " );
		}

		/* Send the response to the processing module. */
		Message output_message;
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
		output_message.SetData ( ( void* ) message_data.c_str ( ), message_data.length ( ) + 1 );
		active_processing_modules_[processing_module_name]->GetCommunicator ( )->BroadCast ( &output_message );
	}
	else {
		Message output_message ( ( void* ) message_data.c_str ( ), Constants::MESSAGE_OP_RUNTIME_QUERY_PROCESSING_MODULE_PORTS_ACK, message_data.length ( ) + 1 );
		cluster_communicator_->Send ( &output_message, received_message.GetSource ( ) );
	}
}

void Runtime::ReadConfigurationFile ( void ) throw ( FileOperationException, XMLParserException ) {

	try {
		runtime_configurator_ = new RuntimeConfigurator ( configuration_file_name_ );
		runtime_configurator_->FillConfiguratorItems ( );
	}
	catch ( FileOperationException e ) {
		throw e;
	}
	catch ( XMLParserException& e ) {
		throw e;
	}
}

void Runtime::RemoveProcessingModule ( bool is_removal_manager, Message& received_message ) {
	string processing_module_name = ( char* ) received_message.GetData ( );
	bool status = active_processing_modules_.find ( processing_module_name ) != active_processing_modules_.end ( );

	/** Communicates all the modules, except the target, about the removal. */
	received_message.SetOperationCode ( Constants::MESSAGE_OP_DISCONNECT );
	for ( map < string, ProcessingModuleEntry* >::iterator m = active_processing_modules_.begin ( ); m != active_processing_modules_.end ( ); ++m ) {
		if ( m->first != processing_module_name ) {
			m->second->GetCommunicator ( )->BroadCast ( &received_message );
			m->second->GetCommunicator ( )->Synchronize ( );
		}
	}

	/* If I am the operation manager, I have to pass the command to the other processes */
	if ( is_removal_manager ) {
		received_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE );

		// Notifies the other managers
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
			if ( i != cluster_communicator_->GetProcessRank ( ) ) {
				cluster_communicator_->Send ( &received_message, i );
			}
		}

		// Notifies the database group
		database_communicator_->BroadCast ( &received_message );

		Message input_message;
		int received_acks = 0;
		while ( received_acks < cluster_communicator_->GetNumberProcesses ( ) - 1 ) {
			int source = cluster_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK );
			input_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK );
			cluster_communicator_->Receive ( source, &input_message );
			++received_acks;
			status = status || * ( ( bool* ) input_message.GetData ( ) );
		}

		if ( !status ) {
			string processing_module_name = ( char* ) received_message.GetData ( );
			string error_message = "processing module " + processing_module_name + " was not running on " + Constants::SYSTEM_NAME;
			throw ProcessRemovingException ( error_message );
		}
	}

	/* If this server is responsible for the processing module, it sends a shutdown message to it */
	if ( active_processing_modules_.find ( processing_module_name ) != active_processing_modules_.end ( ) ) {
		DoProcessingModuleRemoval ( received_message );
	}

	if ( !is_removal_manager ) {
		Message response_message ( &status, Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK, sizeof(bool) );
		cluster_communicator_->Send ( &response_message, received_message.GetSource ( ) );
	}
}

void Runtime::RemoveProcessingModuleInstance ( bool removal_manager, Message& received_message ) {
	string module_name = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetModuleName ( );
	bool status = active_processing_modules_.find ( module_name ) != active_processing_modules_.end ( );

	/** Communicates all the modules, except the target module, about the removal of the instance. */
	received_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_PEER_INSTANCE );
	for ( map < string, ProcessingModuleEntry* >::iterator m = active_processing_modules_.begin ( ); m != active_processing_modules_.end ( ); ++m ) {
		if ( m->first != module_name ) {
			m->second->GetCommunicator ( )->BroadCast ( &received_message );
			m->second->GetCommunicator ( )->Synchronize ( );
		}
	}

	/* If I am the operation manager, I pass the command to the other managers */
	if ( removal_manager ) {
		received_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_INSTANCE );

		// Notifies all managers
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
			if ( i != cluster_communicator_->GetProcessRank ( ) ) {
				cluster_communicator_->Send ( &received_message, i );
			}
		}

		// Notifies the database group
		database_communicator_->BroadCast ( &received_message );

		Message input_message;
		int received_acks = 0;
		while ( received_acks < cluster_communicator_->GetNumberProcesses ( ) - 1 ) {
			int source = cluster_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_REMOVE_INSTANCE_ACK );
			input_message.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_INSTANCE_ACK );
			cluster_communicator_->Receive ( source, &input_message );
			++received_acks;
			status = status || * ( ( bool* ) input_message.GetData ( ) );
		}

		if ( !status ) {
			string error_message = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetModuleName ( ) + " was not running on " + Constants::SYSTEM_NAME;
			throw ProcessRemovingException ( error_message );
		}
	}

	/* If this server is responsible for the processing module, it sends a message informing the instance to leave the system. */
	if ( active_processing_modules_.find ( module_name ) != active_processing_modules_.end ( ) ) {
		DoProcessingModuleInstanceRemoval ( received_message );
	}

	if ( !removal_manager ) {
		Message response_message ( &status, Constants::MESSAGE_OP_REMOVE_INSTANCE_ACK, sizeof(bool) );
		cluster_communicator_->Send ( &response_message, received_message.GetSource ( ) );
	}

}

void Runtime::Run ( void ) {
	string message = "running " + Constants::SYSTEM_NAME + " on " + cluster_communicator_->GetHostName ( );
	Util::Information ( cluster_communicator_, message );
	SpawnDatabaseDaemon ( );
	ExchangeInitialInformation ( );

	pthread_create ( &console_thread_, 0, &Runtime::StartConsoleThread, this );
	pthread_create ( &server_thread_, 0, &Runtime::StartServerThread, this );
	pthread_join ( console_thread_, NULL );
	pthread_join ( server_thread_, NULL );
}

void Runtime::Shutdown ( bool is_shutdown_manager, Message& received_message ) {
	cluster_communicator_->Lock ( );
	if ( is_shutdown_manager ) {
		for ( int i = 0; i < cluster_communicator_->GetNumberProcesses ( ); ++i ) {
			if ( cluster_communicator_->GetProcessRank ( ) != i ) {
				cluster_communicator_->Send ( &received_message, i );
			}
		}
	}
	else {
		pthread_cancel ( console_thread_ );
	}
	cluster_communicator_->Synchronize ( );

	Message shutdown_message ( NULL, Constants::MESSAGE_OP_SHUTDOWN, 0 );
	string log_message_text;

	/* Sends a message to all database daemons. This message tells the daemons to stop running. */
	database_communicator_->Lock();
	shutdown_message.SetOperationCode ( Constants::MESSAGE_OP_SHUTDOWN );
	if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		log_message_text = "stopping database daemons";
		Logger::Lock ( );
		runtime_logger_->PrintInfo ( log_message_text );
		Logger::Unlock ( );
		database_communicator_->BroadCast ( &shutdown_message );
	}
	database_communicator_->Synchronize ( );
	database_communicator_->Unlock();

	/* Sends a message to all processing modules under this server responsibility. This message tells the processing module to stop running. */
	ProcessingModuleEntry::Lock();
	shutdown_message.SetOperationCode ( Constants::MESSAGE_OP_SHUTDOWN );
	for ( map < string, ProcessingModuleEntry* >::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it ) {
		log_message_text = "stopping instances of processing module " + it->first;
		Util::Information ( cluster_communicator_, log_message_text );
		it->second->GetCommunicator ( )->BroadCast ( &shutdown_message );
	}
	for ( map < string, ProcessingModuleEntry* >::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it ) {
		it->second->GetCommunicator ( )->Synchronize ( );
		delete ( it->second );
	}
	active_processing_modules_.clear ( );
	ProcessingModuleEntry::Unlock();

	/* Sends a message to be written at a log file in the master daemon */
	log_message_text = "stopping " + Constants::SYSTEM_NAME + " daemon at " + cluster_communicator_->GetHostName ( );
	shutdown_message.SetOperationCode ( Constants::MESSAGE_OP_SHUTDOWN_ACK );
	shutdown_message.SetData ( ( void* ) log_message_text.c_str ( ), log_message_text.length ( ) + 1 );
	cluster_communicator_->Send ( &shutdown_message, Constants::COMM_ROOT_PROCESS );

	/* Sets the notification flag to get out of thread */
	pthread_mutex_lock ( &shutdown_notification_mutex_ );
	shutdown_notification_ = true;
	pthread_mutex_unlock ( &shutdown_notification_mutex_ );
	cluster_communicator_->Unlock ( );
}

void Runtime::SpawnDatabaseDaemon ( void ) {
	vector < string > hosts;
	vector < string > commands;
	vector < vector < string > > arguments;

	for ( map < string, Host >::iterator it = runtime_configurator_->GetHosts ( )->begin ( ); it != runtime_configurator_->GetHosts ( )->end ( ); ++it ) {
		if ( it->second.IsDatabaseServer ( ) ) {
			hosts.push_back ( it->first );
			commands.push_back ( runtime_configurator_->GetDBExeName ( ) );
			arguments.push_back ( Util::TokenizeString ( " ", runtime_configurator_->GetDBArguments ( ) ) );
		}
	}

	int number_process[hosts.size ( )];
	for ( size_t i = 0; i < hosts.size ( ); ++i ) {
		number_process[i] = 1;
	}

	int spawned;
	if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		string message = "spawning database daemons";
		runtime_logger_->PrintInfo ( message );
	}

	if ( hosts.size ( ) > 0 ) {
		try {
			string server_work_directory = runtime_configurator_->GetExeDir ( );
			database_communicator_ = cluster_communicator_->Spawn ( arguments, commands, hosts, number_process, server_work_directory );
			spawned = database_communicator_->GetNumberProcesses ( );
		}
		catch ( MPI::Exception e ) {
			string msg = "it was not possible to create the database process at " + cluster_communicator_->GetHostName ( );
			throw ProcessSpawnningException ( msg );
		}
		if ( spawned != ( int ) hosts.size ( ) ) {
			string msg = "it was not possible to create the database process at " + cluster_communicator_->GetHostName ( );
			throw ProcessSpawnningException ( msg );
		}
	}
}

MpiCommunicator* Runtime::SpawnProcessingModuleInstances ( ProcessingModuleConfigurator* processing_module_configurator, map < string, int >* scheduler_result ) throw ( ProcessSpawnningException ) {

	/* Build the information to spawn processing module instances */
	vector < string > commands;
	vector < vector < string > > arguments;
	vector < string > hosts_to_receive_processing_module;
	int number_process[scheduler_result->size ( )];
	int total_instances = 0;

	for ( map < string, int >::iterator it = scheduler_result->begin ( ); it != scheduler_result->end ( ); ++it ) {
		commands.push_back ( runtime_configurator_->GetProcessingModuleExeName ( ) );
		arguments.push_back ( Util::TokenizeString ( " ", processing_module_configurator->GetArguments ( ) ) );
		hosts_to_receive_processing_module.push_back ( it->first );
		number_process[hosts_to_receive_processing_module.size ( ) - 1] = it->second;
		total_instances += it->second;
	}

	/* Spawns the processing module instances. */
	MpiCommunicator* new_processing_module_communicator;
	if ( hosts_to_receive_processing_module.size ( ) > 0 ) {
		try {
			char number_instances[Constants::MAX_INT_TO_STRING_LENGTH];
			sprintf ( number_instances, "%d", total_instances );
			string text_number_instances = number_instances;
			string message = "spawning " + text_number_instances + " instances of " + processing_module_configurator->GetName ( ) + " from host " + cluster_communicator_->GetHostName ( );
			Util::Information ( cluster_communicator_, message );
			string server_work_directory = runtime_configurator_->GetExeDir ( );
			new_processing_module_communicator = self_communicator_->Spawn ( arguments, commands, hosts_to_receive_processing_module, number_process, server_work_directory );
		}
		catch ( MPI::Exception e ) {
		}

		if ( new_processing_module_communicator->GetNumberProcesses ( ) != total_instances ) {
			string msg = "it was not possible to create the processing module processes from " + cluster_communicator_->GetHostName ( );
			throw ProcessSpawnningException ( msg );
		}
	}
	return new_processing_module_communicator;
}

void* Runtime::StartConsoleThread ( void* obj ) {
	reinterpret_cast < Runtime * > ( obj )->WaitConnections ( );
	pthread_exit ( NULL);
}

void* Runtime::StartServerThread ( void* obj ) {
	reinterpret_cast < Runtime * > ( obj )->WaitMessages ( );
	pthread_exit ( NULL);
}

void Runtime::WaitConnections ( void ) {

	Message message_from_console;
	Message message_to_console;
	int source;

	/* Waits message from a console and decides what have be done */
	do {
		try {
			console_communicator_ = self_communicator_->Accept ( server_port_ );
			source = console_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			message_from_console.SetOperationCode ( Constants::MESSAGE_OP_ANY );
			console_communicator_->Receive ( source, &message_from_console );

			switch ( message_from_console.GetOperationCode ( ) ) {

				case Constants::MESSAGE_OP_ADD_PROCESSING_MODULE : {
					AddProcessingModule ( message_from_console );
					message_to_console.SetOperationCode ( Constants::MESSAGE_OP_ADD_PROCESSING_MODULE_ACK );
					break;
				}

				case Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE : {
					RemoveProcessingModule ( true, message_from_console );
					message_to_console.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK );
					break;
				}

				case Constants::MESSAGE_OP_REMOVE_INSTANCE : {
					RemoveProcessingModuleInstance ( true, message_from_console );
					message_to_console.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_INSTANCE_ACK );
					break;
				}

				case Constants::MESSAGE_OP_SHUTDOWN : {
					Shutdown ( true, message_from_console );
					break;
				}

				default : {
					break;
				}
			}
		}
		catch ( ProcessSpawnningException& e ) {
			Util::Error ( cluster_communicator_, e.ToString ( ) );
			message_to_console.SetOperationCode ( Constants::MESSAGE_OP_ADD_PROCESSING_MODULE_ERROR );
			message_to_console.SetData ( ( void* ) e.ToString ( ).c_str ( ), strlen ( e.ToString ( ).c_str ( ) ) + 1 );
		}
		catch ( ProcessRemovingException& e ) {
			Util::Error ( cluster_communicator_, e.ToString ( ) );
			message_to_console.SetOperationCode ( Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ERROR );
			message_to_console .SetData ( ( void* ) e.ToString ( ).c_str ( ), e.ToString ( ).length ( ) + 1 );
		}
		catch ( XMLParserException& e ) {
			Util::Error ( cluster_communicator_, e.ToString ( ) );
			message_to_console.SetOperationCode ( Constants::MESSAGE_OP_PARSER_ERROR );
			message_to_console .SetData ( ( void* ) e.ToString ( ).c_str ( ), e.ToString ( ).length ( ) + 1 );
		}
		catch ( exception& e ) {
			Util::Error ( cluster_communicator_, e.what ( ) );
		}

		if ( message_from_console.GetOperationCode ( ) != Constants::MESSAGE_OP_SHUTDOWN ) {
			console_communicator_->Send ( &message_to_console, source );
		}

		console_communicator_->Disconnect ( );
		delete ( console_communicator_ );
	}
	while ( !shutdown_notification_ );
}

void Runtime::WaitMessages ( void ) {

	Message received_message;
	string processing_module_name;
	int source;

	do {
		try {
			received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );

			cluster_communicator_->Lock ( );
			database_communicator_->Lock ( );
			ProcessingModuleEntry::Lock ( );

			/* Checks whether there is a message from other runtime daemon */
			source = cluster_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			if ( !shutdown_notification_ && source != -1 ) { // Receives the message from other runtime
				cluster_communicator_->Receive ( source, &received_message );
				HandleRuntimeMessage ( received_message );
			}
			else { /* Checks whether there is a message from a database daemon */
				source = database_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
				if ( !shutdown_notification_ && source != -1 ) {
					database_communicator_->Receive ( source, &received_message );
					HandleDatabaseMessage ( received_message );
				}
			}

			/* If there is no message from other runtime daemons neither database daemons, checks whether there is a message from processing module */
			if ( source == -1 ) {
				for ( map < string, ProcessingModuleEntry* >::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it ) {
					source = it->second->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
					if ( !shutdown_notification_ && source != -1 ) {
						it->second->GetCommunicator ( )->Receive ( source, &received_message );
						HandleProcessingModuleMessage ( it->first, received_message );
						break;
					}
				}
			}
			ProcessingModuleEntry::Unlock ( );
			database_communicator_->Unlock ( );
			cluster_communicator_->Unlock ( );
		}
		catch ( exception& e ) {
			if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				Logger::Lock ( );
				runtime_logger_->PrintError ( e.what ( ) );
				Logger::Unlock ( );
			}
			else {
				Util::Error ( cluster_communicator_, e.what ( ) );
			}
		}
		if ( source == -1 ) {
			usleep ( Constants::SLEEP_TIME );
		}
	}
	while ( !shutdown_notification_ );

	try {

		/* The root daemon may receive all the termination messages from the other daemons in order to write to the log file */
		if ( cluster_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
			int received_ack_messages = cluster_communicator_->GetNumberProcesses ( );
			while ( received_ack_messages > 0 ) {
				int source = cluster_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
				received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );
				cluster_communicator_->Receive ( source, &received_message );
				if ( received_message.GetOperationCode ( ) == Constants::MESSAGE_OP_SHUTDOWN_ACK ) {
					--received_ack_messages;
					Logger::Lock ( );
					runtime_logger_->PrintInfo ( ( char* ) received_message.GetData ( ) );
					Logger::Unlock ( );
				}
				else if ( received_message.GetOperationCode ( ) == Constants::MESSAGE_OP_INFO_LOG ) {
					Logger::Lock ( );
					runtime_logger_->PrintInfo ( ( char* ) received_message.GetData ( ) );
					Logger::Unlock ( );
				}
			}
			string log_message_text = Constants::SYSTEM_NAME + " stopped";
			Logger::Lock ( );
			runtime_logger_->PrintInfo ( log_message_text );
			runtime_logger_->PrintScreenSeparator ( );
			Logger::Unlock ( );
		}
	}
	catch ( exception e ) {
	}
}
