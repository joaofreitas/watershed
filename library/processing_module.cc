/**
 * \file library/processing_module.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include <library/processing_module.h>

map < string, string > ProcessingModule::arguments_;
ProcessingModule* ProcessingModule::this_instance_;
extern int argc_;
extern char** argv_;

ProcessingModule::ProcessingModule ( void ) {
	SetInitialTime ( );
	message_sequence_number_ = 0;
	error_on_init_ = false;
	termination_requested_ = false;
	error_message_on_init_ = "";

	/* Communicator including all the processing module instances. */
	group_communicator_ = new MpiCommunicator ( argc_, argv_, Constants::COMM_SCOPE_WORLD );
	database_communicator_ = NULL;

	shutdown_notification_ = false;
	CreateArguments ( );
	this_instance_ = this;
}

ProcessingModule::~ProcessingModule ( void ) {

	string message_data;

	/* Synchronizes all instances to terminate together */
	group_communicator_->Synchronize ( );

	arguments_.clear ( );
	pthread_t disconnection_threads[2];
	pthread_create ( &disconnection_threads[0], 0, &ProcessingModule::ThreadDisconnectConsumers, this );
	pthread_create ( &disconnection_threads[1], 0, &ProcessingModule::ThreadDisconnectProducers, this );
	pthread_join ( disconnection_threads[0], NULL );
	pthread_join ( disconnection_threads[1], NULL );

	/* Disconnects from database daemons */
	if ( database_communicator_ != NULL ) {
		database_communicator_->Disconnect ( );
		delete ( database_communicator_ );
	}

	delete ( runtime_communicator_ );
	delete ( group_communicator_ );
	delete ( processing_module_configurator_ );
}

void ProcessingModule::AcceptConnection ( void ) {
	Message input_message;
	Message output_message;
	string log_message_data;
	int source;

	try {
		group_communicator_->Synchronize ( );
		MpiCommunicator * new_communicator = group_communicator_->Accept ( processing_module_configurator_->GetPortName ( ) );
		group_communicator_->Synchronize ( );

		source = new_communicator->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
		input_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );
		new_communicator->Receive ( source, &input_message );
		switch ( input_message.GetOperationCode ( ) ) {
			case Constants::MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION : {
				log_message_data = AddProducer ( new_communicator, input_message );
				break;
			}

			case Constants::MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION : {
				log_message_data = AddConsumer ( new_communicator, input_message );
				break;
			}

			default : {
				break;
			}
		}

		if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
			Util::Information ( runtime_communicator_, log_message_data );
		}

	}
	catch ( FileOperationException& e ) {
		Util::Error ( runtime_communicator_, e.ToString ( ) );
	}
	catch ( exception e ) {

	}
}

string ProcessingModule::AddConsumer ( MpiCommunicator* new_communicator, Message& received_message ) throw ( FileOperationException ) {
	string configurator_file_name = ( char* ) received_message.GetData ( );
	ProcessingModuleConfigurator* consumer_configurator;
	try {
		consumer_configurator = new ProcessingModuleConfigurator ( configurator_file_name );
		consumer_configurator->SetConfiguratorFileName ( configurator_file_name );
	}
	catch ( XMLParserException& e ) {
		throw ( e );
	}

	DataConsumer* new_consumer;
	vector < InputFlow >* consumer_inputs = consumer_configurator->GetInputs ( );
	for ( uint i = 0; i < consumer_inputs->size ( ); ++i ) {
		if ( consumer_inputs->at ( i ).GetName ( ).compare ( processing_module_configurator_->GetFlowOut ( ) ) == 0 ) {
			try {
				new_consumer = new DataConsumer ( consumer_inputs->at ( i ).GetPolicyFunctionFile ( ), consumer_configurator->GetName ( ), consumer_inputs->at ( i ).GetPolicy ( ), consumer_configurator->GetQueryFlowIn ( ) );
				new_consumer->SetCommunicator ( new_communicator );
			}
			catch ( FileOperationException& e ) {
				throw e;
			}
		}
	}

	// Sends the producer name to the new consumer
	if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		Message info_message ( ( void* ) processing_module_configurator_->GetConfiguratorFileName ( ).c_str ( ), Constants::MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION, processing_module_configurator_->GetConfiguratorFileName ( ).length ( ) + 1 );
		new_consumer->GetCommunicator ( )->BroadCast ( &info_message );
	}

	consumers_[new_consumer->GetName ( )] = new_consumer;
	string log_message_data = consumer_configurator->GetName ( ) + " has connected to " + processing_module_configurator_->GetName ( ) + " as consumer";
	delete ( consumer_configurator );
	return log_message_data;
}

string ProcessingModule::AddProducer ( MpiCommunicator* new_communicator, Message& received_message ) {
	Message output_message;
	string configurator_file_name = ( char* ) received_message.GetData ( );
	ProcessingModuleConfigurator* producer_configurator;
	try {
		producer_configurator = new ProcessingModuleConfigurator ( configurator_file_name );
	}
	catch ( XMLParserException& e ) {
		throw ( e );
	}

	if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION );
		string message_data = processing_module_configurator_->GetConfiguratorFileName ( );
		output_message.SetData ( ( void* ) message_data.c_str ( ), message_data.length ( ) + 1 );
		new_communicator->BroadCast ( &output_message );
	}

	DataProducer* new_producer = new DataProducer ( producer_configurator->GetName ( ) );
	new_producer->SetCommunicator ( new_communicator );
	new_producer->SetFlowOut ( producer_configurator->GetFlowOut ( ) );
	producers_[new_producer->GetName ( )] = new_producer;

	for ( int i = 0; i < new_producer->GetNumberInstances ( ); ++i ) {
		SendCreditToProducer ( i, new_producer->GetName ( ) );
	}

	group_communicator_->Synchronize ( );

	string log_message_data = new_producer->GetName ( ) + " has connected to " + processing_module_configurator_->GetName ( ) + " as producer";
	return log_message_data;
}

int ProcessingModule::ComputeProducerCredit ( void ) {
	if ( GetNumberProducerInstances ( ) != 0 ) {
		return Constants::SHARED_CREDIT / GetNumberProducerInstances ( );
	}
	else {
		return 0;
	}
}

void ProcessingModule::ComputeResourcesUsage ( void ) {
	struct rusage t;
	getrusage ( RUSAGE_THREAD, &t );
	user_time_ = ( t.ru_utime.tv_sec ) + ( t.ru_utime.tv_usec ) * 0.000001;
	system_time_ = ( t.ru_stime.tv_sec ) + ( t.ru_stime.tv_usec ) * 0.000001;
}

void ProcessingModule::ConfigureProcess ( void ) {
	/* Changes the process name to the processing module name. */
	char name[16];
	sprintf ( name, "%d/%d-%s", group_communicator_->GetProcessRank ( ), group_communicator_->GetNumberProcesses ( ), processing_module_configurator_->GetName ( ).c_str ( ) );
	prctl ( PR_SET_NAME, ( char* ) name );

	/*signal ( SIGCHLD, SIG_IGN );  ignore child
	 signal ( SIGTSTP, SIG_IGN );  ignore tty signals
	 signal ( SIGTTOU, SIG_IGN );
	 signal ( SIGTTIN, SIG_IGN );
	 signal ( SIGHUP, &ProcessingModule::HandleSignal );  catch hangup signal
	 signal ( SIGABRT, &ProcessingModule::HandleSignal );  (Signal Abort) Abnormal termination, such as is initiated by the abort function.
	 signal ( SIGFPE, &ProcessingModule::HandleSignal );  (Signal Floating-Point Exception) Erroneous arithmetic operation, such as zero divide or an operation resulting in overflow (not necessarily with a floating-point operation).
	 signal ( SIGILL, &ProcessingModule::HandleSignal );  (Signal Illegal Instruction) Invalid function image, such as an illegal instruction. This is generally due to a corruption in the code or to an attempt to execute data.
	 signal ( SIGINT, &ProcessingModule::HandleSignal );  (Signal Interrupt) Interactive attention signal. Generally generated by the application user.
	 signal ( SIGSEGV, &ProcessingModule::HandleSignal );  (Signal Segmentation Violation) Invalid access to storage: When a program tries to read or write outside the memory it is allocated for it.
	 signal ( SIGTERM, &ProcessingModule::HandleSignal );  (Signal Terminate) Termination request sent to program. */
}

void ProcessingModule::ConnectToConsumers ( void ) throw ( FileOperationException ) {
	int source;
	Message output_message;
	Message input_message;

	try {

		/* Requests the consumers for the output stream. */
		if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
			output_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS );
			output_message.SetData ( ( void* ) processing_module_configurator_->GetName ( ).c_str ( ), processing_module_configurator_->GetName ( ).length ( ) + 1 );
			database_communicator_->Send ( &output_message, processing_module_configurator_->GetDatabasePeerIdentification ( ) );

			/* Receives its consumers list from the output stream. */
			input_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS );
			source = database_communicator_->Poll ( processing_module_configurator_->GetDatabasePeerIdentification ( ), Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS );
			database_communicator_->Receive ( source, &input_message );

			/* Requests its consumers ports for runtime. */
			output_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
			output_message.SetData ( input_message.GetData ( ), input_message.GetDataSize ( ) );
			runtime_communicator_->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
		}

		/* Receives its consumers ports from runtime. */
		input_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
		source = runtime_communicator_->Poll ( Constants::COMM_ROOT_PROCESS, Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
		runtime_communicator_->Receive ( source, &input_message );

		/* Connects to all consumers. */
		vector < string > consumers_ports = Util::TokenizeString ( " ", ( char* ) input_message.GetData ( ) );
		for ( int i = 0; i < ( int ) consumers_ports.size ( ); ++i ) {
			group_communicator_->Synchronize ( );
			MpiCommunicator* new_communicator = group_communicator_->Connect ( consumers_ports[i] );

			if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
				output_message.SetOperationCode ( Constants::MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION );
				output_message.SetData ( ( void* ) processing_module_configurator_->GetConfiguratorFileName ( ).c_str ( ), processing_module_configurator_->GetConfiguratorFileName ( ).length ( ) + 1 );
				new_communicator->BroadCast ( &output_message );
			}

			/* Receives the consumer information */
			input_message.SetOperationCode ( Constants::MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION );
			source = new_communicator->Poll ( Constants::COMM_ROOT_PROCESS, Constants::MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION );
			new_communicator->Receive ( source, &input_message );

			string configurator_file_name = ( char* ) input_message.GetData ( );
			ProcessingModuleConfigurator* consumer_configurator;
			try {
				consumer_configurator = new ProcessingModuleConfigurator ( configurator_file_name );
				consumer_configurator->SetConfiguratorFileName ( configurator_file_name );
			}
			catch ( XMLParserException& e ) {
				throw ( e );
			}

			/* Creates the consumer object and inserts it into the consumers list. */
			DataConsumer* new_consumer;
			vector < InputFlow >* consumer_inputs = consumer_configurator->GetInputs ( );
			for ( uint i = 0; i < consumer_inputs->size ( ); ++i ) {
				if ( consumer_inputs->at ( i ).GetName ( ) == processing_module_configurator_->GetFlowOut ( ) ) {
					try {
						new_consumer = new DataConsumer ( consumer_inputs->at ( i ).GetPolicyFunctionFile ( ), consumer_configurator->GetName ( ), consumer_inputs->at ( i ).GetPolicy ( ), consumer_configurator->GetQueryFlowIn ( ) );
						new_consumer->SetCommunicator ( new_communicator );
						consumers_[new_consumer->GetName ( )] = new_consumer;
						group_communicator_->Synchronize ( );
					}
					catch ( FileOperationException& e ) {
						throw e;
					}

				}
			}
		}
	}
	catch ( FileOperationException& e ) {
		throw e;
	}
	catch ( exception& e ) {
		throw e;
	}
}

void ProcessingModule::ConnectToDatabaseDaemon ( void ) {
	int source;
	Message output_message;
	Message input_message;

	/* Connects to the database group */
	group_communicator_->Synchronize ( );
	database_communicator_ = group_communicator_->Connect ( processing_module_configurator_->GetDatabasePortName ( ) );
	group_communicator_->Synchronize ( );

	/* Registers at the database group, opens a communication port and send it to interested processes. */
	if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
		string message_data = processing_module_configurator_->GetConfiguratorFileName ( );
		output_message.SetData ( ( void* ) message_data.c_str ( ), message_data.length ( ) + 1 );
		output_message.SetOperationCode ( Constants::MESSAGE_OP_ADD_PROCESSING_MODULE );
		database_communicator_->BroadCast ( &output_message );

		string port_name = group_communicator_->OpenPort ( );
		output_message.SetData ( ( void* ) port_name.c_str ( ), port_name.length ( ) + 1 );
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PORT_NAME );
		group_communicator_->BroadCast ( &output_message );
		runtime_communicator_->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
	}

	/* Receives the name of the opened port. */
	input_message.SetOperationCode ( Constants::MESSAGE_OP_PORT_NAME );
	source = group_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_PORT_NAME );
	group_communicator_->Receive ( source, &input_message );
	processing_module_configurator_->SetPortName ( ( char* ) input_message.GetData ( ) );
}

void ProcessingModule::ConnectToProducers ( void ) {
	int source;
	Message output_message;
	Message input_message;

	if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {

		/* Requests the producers for my input stream. */
		output_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS );
		output_message.SetData ( ( void* ) processing_module_configurator_->GetName ( ).c_str ( ), processing_module_configurator_->GetName ( ).length ( ) + 1 );
		database_communicator_->Send ( &output_message, processing_module_configurator_->GetDatabasePeerIdentification ( ) );

		/* Receives its producers list from my output stream. */
		input_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS );
		source = database_communicator_->Poll ( processing_module_configurator_->GetDatabasePeerIdentification ( ), Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS );
		database_communicator_->Receive ( source, &input_message );

		/* Requests its producers ports from runtime. */
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
		output_message.SetData ( input_message.GetData ( ), input_message.GetDataSize ( ) );
		runtime_communicator_->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
	}

	/* Receives its producers ports from runtime. */
	input_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
	source = runtime_communicator_->Poll ( Constants::COMM_ROOT_PROCESS, Constants::MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY );
	runtime_communicator_->Receive ( source, &input_message );

	/* Connects to its producers. */
	vector < string > producers_ports = Util::TokenizeString ( " ", ( char* ) input_message.GetData ( ) );
	for ( int i = 0; i < ( int ) producers_ports.size ( ); ++i ) {
		group_communicator_->Synchronize ( );
		MpiCommunicator* new_communicator = group_communicator_->Connect ( producers_ports[i] );
		DataProducer* new_producer = new DataProducer ( Constants::EMPTY_ATTRIBUTE );
		new_producer->SetCommunicator ( new_communicator );

		/* Sends the initial information to all producers. */
		if ( group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS ) {
			string message_data = processing_module_configurator_->GetConfiguratorFileName ( );
			output_message.SetOperationCode ( Constants::MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION );
			output_message.SetData ( ( void* ) message_data.c_str ( ), message_data.length ( ) + 1 );
			new_producer->GetCommunicator ( )->BroadCast ( &output_message );
		}

		/* Receives the initial information from the new producer and sends a initial credit to all its instance. */
		input_message.SetOperationCode ( Constants::MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION );
		source = new_producer->GetCommunicator ( )->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION );
		new_producer->GetCommunicator ( )->Receive ( source, &input_message );

		string configurator_file_name = ( char* ) input_message.GetData ( );
		ProcessingModuleConfigurator* producer_configurator;
		try {
			producer_configurator = new ProcessingModuleConfigurator ( configurator_file_name );
		}
		catch ( XMLParserException& e ) {
			throw ( e );
		}

		new_producer->SetProcessingModuleName ( producer_configurator->GetName ( ) );
		new_producer->SetFlowOut ( producer_configurator->GetFlowOut ( ) );
		producers_[new_producer->GetName ( )] = new_producer;
		for ( int i = 0; i < new_producer->GetNumberInstances ( ); ++i ) {
			SendCreditToProducer ( i, new_producer->GetName ( ) );
		}
	}
	group_communicator_->Synchronize ( );
}

void ProcessingModule::CreateArguments ( void ) {
	if ( argc_ % 2 == 0 ) {
		error_on_init_ = true;
		error_message_on_init_ = "invalid number of parameters";
		return;
	}

	for ( int i = 1; i < argc_; i += 2 ) {
		if ( argv_[i][0] != '-' ) {
			string argv_i = argv_[i];
			error_on_init_ = true;
			error_message_on_init_ = "the argument name " + argv_i + " should start with '-'";
			return;
		}
		if ( strlen ( argv_[i] ) == 1 ) {
			error_on_init_ = true;
			error_message_on_init_ = "argument without name";
			return;
		}
		arguments_.insert ( map < string, string >::value_type ( &argv_[i][1], argv_[i + 1] ) );
	}
}

void ProcessingModule::DisconnectConsumers ( void ) {
	/* Disconnects from consumers */
	Message M ( NULL, Constants::MESSAGE_OP_TERMINATION, 0 );
	for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) {
		c->second->GetCommunicator ( )->BroadCast ( &M );
		c->second->GetCommunicator ( )->Synchronize ( );
		c->second->GetCommunicator ( )->Disconnect ( );
		delete ( c->second );
	}
	consumers_.clear ( );
}

void ProcessingModule::DisconnectFromProcessingModule ( Message* received_message ) {
	runtime_communicator_->Synchronize ( );
	string module_name = ( char* ) received_message->GetData ( );
	ReceiveLastMessages ( module_name );

	// Disconnects from the module if it is a consumer
	if ( producers_.find ( module_name ) != producers_.end ( ) ) {
		producers_[module_name]->GetCommunicator ( )->Synchronize ( );
		producers_[module_name]->GetCommunicator ( )->Disconnect ( );
		delete ( producers_[module_name] );
		producers_.erase ( module_name );
	}

	// Disconnects from the module if it is producer
	if ( consumers_.find ( module_name ) != consumers_.end ( ) ) {
		consumers_[module_name]->GetCommunicator ( )->Synchronize ( );
		consumers_[module_name]->GetCommunicator ( )->Disconnect ( );
		delete ( consumers_[module_name] );
		consumers_.erase ( module_name );
	}
}

void ProcessingModule::DisconnectProducers ( void ) {
	/* Disconnects from producers */
	Message M ( NULL, Constants::MESSAGE_OP_TERMINATION, 0 );
	for ( map < string, DataProducer* >::iterator p = producers_.begin ( ); p != producers_.end ( ); ++p ) {
		p->second->GetCommunicator ( )->BroadCast ( &M );
		p->second->GetCommunicator ( )->Synchronize ( );
		p->second->GetCommunicator ( )->Disconnect ( );
		delete ( p->second );
	}
	producers_.clear ( );
}

bool ProcessingModule::ErrorOnInit ( void ) {
	return error_on_init_;
}

string ProcessingModule::GetArgument ( string argument_name ) {
	return arguments_[argument_name];
}

double ProcessingModule::GetClockTime ( void ) {
	struct timeval final_time;
	gettimeofday ( &final_time, NULL );
	double total_time = ( final_time.tv_sec - initial_time_.tv_sec ) + ( final_time.tv_usec - initial_time_.tv_usec ) * 0.000001;
	return total_time;
}

double ProcessingModule::GetCPUTime ( void ) {
	return ( GetUserTime ( ) + GetSystemTime ( ) );
}

string ProcessingModule::GetErrorMessageOnInit ( void ) {
	return error_message_on_init_;
}

string ProcessingModule::GetModuleName ( void ) {
	return processing_module_configurator_->GetName ( );
}

int ProcessingModule::GetNumberInstances ( void ) {
	return group_communicator_->GetNumberProcesses ( );
}

int ProcessingModule::GetNumberOfProducers ( string stream_name ) {
	int total = 0;
	for ( map < string, DataProducer* >::iterator p = producers_.begin ( ); p != producers_.end ( ); ++p ) {
		if ( stream_name == p->second->GetFlowOut ( ) ) {
			total += p->second->GetNumberInstances ( );
		}
	}
	return total;
}

int ProcessingModule::GetNumberOfConsumers ( void ) {
	int total = 0;
	for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) {
		total += c->second->GetNumberInstances ( );
	}
	return total;
}

int ProcessingModule::GetNumberProducerInstances ( void ) {
	int total_producer_instances = 0;
	for ( map < string, DataProducer* >::iterator p = producers_.begin ( ); p != producers_.end ( ); ++p ) {
		total_producer_instances += p->second->GetNumberInstances ( );
	}
	return total_producer_instances;
}

int ProcessingModule::GetRank ( void ) {
	return group_communicator_->GetProcessRank ( );
}

double ProcessingModule::GetSystemTime ( void ) {
	return system_time_;
}

double ProcessingModule::GetUserTime ( void ) {
	return user_time_;
}

void ProcessingModule::HandleDatabaseMessage ( Message& received_message ) {

}

void ProcessingModule::HandleProcessingModuleMessage ( string processing_module_id, int source, Message& received_message ) {
	switch ( received_message.GetOperationCode ( ) ) {
		case Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT : {
			SetDataConsumerCredit ( processing_module_id, received_message );
			break;
		}

		case Constants::MESSAGE_OP_PROCESSING_MODULE_DATA : {
			producers_[processing_module_id]->SetCredit ( source, producers_[processing_module_id]->GetCredit ( source ) - 1 );
			if ( producers_[processing_module_id]->GetCredit ( source ) == 0 ) {
				SendCreditToProducer ( source, processing_module_id );
			}
			if ( !termination_requested_ ) {
				Process ( received_message );
			}
			break;
		}

		default : {
			break;
		}
	}
}

void ProcessingModule::HandleRuntimeMessage ( Message& received_message ) {
	switch ( received_message.GetOperationCode ( ) ) {

		case Constants::MESSAGE_OP_ACCEPT_CONNECT : {
			AcceptConnection ( );
			break;
		}

		case Constants::MESSAGE_OP_DISCONNECT : {
			DisconnectFromProcessingModule ( &received_message );
			break;
		}

		case Constants::MESSAGE_OP_REMOVE_INSTANCE : {
			RemoveInstance ( received_message );
			break;
		}

		case Constants::MESSAGE_OP_REMOVE_PEER_INSTANCE : {
			RemovePeerInstance ( received_message );
			break;
		}

		case Constants::MESSAGE_OP_SHUTDOWN : {
			Shutdown ( );
			break;
		}

		default : {
			break;
		}
	}
}

void ProcessingModule::HandleSignal ( int signal ) {
	string message = this_instance_->GetModuleName ( ) + "[" + Util::IntegerToString ( this_instance_->GetRank ( ) ) + "] received ";

	switch ( signal ) {
		case SIGHUP :
			message += "SIGHUP";
			break;
		case SIGABRT :
			message += "SIGABRT";
			break;
		case SIGFPE :
			message += "SIGFPE";
			break;
		case SIGILL :
			message += "SIGILL";
			break;
		case SIGINT :
			message += "SIGINT";
			break;
		case SIGSEGV :
			message += "SIGSEGV";
			break;
		case SIGTERM :
			message += "SIGTERM";
			break;
	}
	Util::Error ( this_instance_->runtime_communicator_, message );
}

void ProcessingModule::InitProcessingModule ( void ) {
	ConnectToDatabaseDaemon ( );
	if ( processing_module_configurator_->GetFlowOut ( ) != Constants::EMPTY_ATTRIBUTE ) {
		ConnectToConsumers ( );
	}
	if ( processing_module_configurator_->GetInputs ( )->size ( ) != 0 ) {
		ConnectToProducers ( );
	}
	group_communicator_->Synchronize ( );
}

void ProcessingModule::MainLoop ( void ) {
	int source;
	Message received_message;

	do {
		try {
			received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );

			/* Checks whether there is a message from runtime */
			source = runtime_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );

			if ( source != -1 ) { /* Receives the message from runtime */
				runtime_communicator_->Receive ( source, &received_message );
				HandleRuntimeMessage ( received_message );
			}
			else { /* Checks whether there is a message from a database daemon */
				source = database_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
				if ( source != -1 ) { /* Receives a message from a database daemon */
					database_communicator_->Receive ( source, &received_message );
					HandleDatabaseMessage ( received_message );
				}
			}

			/* If there is no message from runtime neither from database daemons, checks whether there is a message from processing module */
			if ( source == -1 ) {
				for ( map < string, DataProducer* >::iterator p = producers_.begin ( ); p != producers_.end ( ); ++p ) { // Checks for producers messages
					source = p->second->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
					if ( source != -1 ) { /* Receives the message from processing module. */
						p->second->GetCommunicator ( )->Receive ( source, &received_message );
						HandleProcessingModuleMessage ( p->first, source, received_message );
						break;
					}
				}
				if ( source == -1 ) {
					for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) { // Checks for consumers messages
						source = c->second->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
						if ( source != -1 ) { /* Receives the message from processing module. */
							c->second->GetCommunicator ( )->Receive ( source, &received_message );
							HandleProcessingModuleMessage ( c->first, source, received_message );
							break;
						}
					}
				}
			}

			/* Process the message */
			if ( !shutdown_notification_ ) {
				if ( source == -1 && processing_module_configurator_->GetInputs ( )->size ( ) == 0 and !termination_requested_ ) {
					received_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_DATA );
					received_message.SetData ( NULL, 0 );
					Process ( received_message );
				}
			}
		}
		catch ( exception e ) {

		}
		if ( source == -1 ) {
			usleep ( Constants::SLEEP_TIME );
		}
		source = -1;
	}
	while ( !shutdown_notification_ );
}

void ProcessingModule::ReceiveLastMessages ( string module_name ) {
	Message received_message;

	/* Receives the last messages from a producer */
	if ( producers_.find ( module_name ) != producers_.end ( ) ) {
		int number_terminations = 0;
		int source = -1;
		do {
			source = producers_[module_name]->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			if ( source != -1 ) { /* Receives the message from processing module. */
				received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );
				producers_[module_name]->GetCommunicator ( )->Receive ( source, &received_message );
				if ( received_message.GetOperationCode ( ) == Constants::MESSAGE_OP_TERMINATION ) {
					++number_terminations;
				}
				else {
					HandleProcessingModuleMessage ( module_name, source, received_message );
				}
			}
		}
		while ( number_terminations < producers_[module_name]->GetCommunicator ( )->GetNumberProcesses ( ) );
	}

	/* Receives the last messages from a consumer */
	if ( consumers_.find ( module_name ) != consumers_.end ( ) ) {
		int number_terminations = 0;
		int source = -1;
		do {
			source = consumers_[module_name]->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			if ( source != -1 ) { /* Receives the message from processing module. */
				received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );
				consumers_[module_name]->GetCommunicator ( )->Receive ( source, &received_message );
				if ( received_message.GetOperationCode ( ) == Constants::MESSAGE_OP_TERMINATION ) {
					++number_terminations;
				}
				else {
					HandleProcessingModuleMessage ( module_name, source, received_message );
				}
			}
		}
		while ( number_terminations < consumers_[module_name]->GetCommunicator ( )->GetNumberProcesses ( ) );
	}
}

void ProcessingModule::RemoveConsumerInstance ( Message& received_message ) {
	string module_name = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetModuleName ( );
	int instance_rank = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetInstanceIdentification ( );
	if ( consumers_.find ( module_name ) != consumers_.end ( ) ) {
		consumers_[module_name]->RemoveInstance ( instance_rank );
	}
}

void ProcessingModule::RemoveInstance ( Message& received_message ) {
	int instance_to_remove = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetInstanceIdentification ( );
	int my_rank = group_communicator_->GetProcessRank ( );

	// Removes the instance from the PM communicator
	group_communicator_->RemoveProcess ( instance_to_remove );

	// Removes the instance from the interconnections with the consumers
	for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) {
		c->second->GetCommunicator ( )->RemoveProcess ( instance_to_remove );
	}

	// Removes the instance from the interconnections with the producers
	for ( map < string, DataProducer* >::iterator p = producers_.begin ( ); p != producers_.end ( ); ++p ) {
		p->second->GetCommunicator ( )->RemoveProcess ( instance_to_remove );
		if ( my_rank != instance_to_remove ) { // Sends new credits to the producers in order to avoid error in the flow control mechanism
			for ( int j = 0; j < p->second->GetNumberInstances ( ); ++j ) {
				SendCreditToProducer ( j, p->first );
			}
		}
	}

	// Removes the instance from the database communicator
	database_communicator_->RemoveProcess ( instance_to_remove );

	// Removes the instance from the runtime communicator
	runtime_communicator_->RemoveProcess ( instance_to_remove );

	if ( instance_to_remove == my_rank ) {
		shutdown_notification_ = true;
	}

}

void ProcessingModule::RemovePeerInstance ( Message& received_message ) {
	runtime_communicator_->Synchronize ( );
	RemoveProducerInstance ( received_message );
	RemoveConsumerInstance ( received_message );
}

void ProcessingModule::RemoveProducerInstance ( Message& received_message ) {
	string module_name = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetModuleName ( );
	int instance_rank = ( ( RemoveInstanceMessage* ) received_message.GetData ( ) )->GetInstanceIdentification ( );

	if ( producers_.find ( module_name ) != producers_.end ( ) ) {
		producers_[module_name]->RemoveInstance ( instance_rank );
		for ( int j = 0; j < producers_[module_name]->GetNumberInstances ( ); ++j ) {
			SendCreditToProducer ( j, module_name );
		}
	}
}

void ProcessingModule::Run ( void ) {
	ConfigureProcess ( );
	InitProcessingModule ( );
	MainLoop ( );
}

void ProcessingModule::Send ( Message& output_message ) {
	if ( !error_on_init_ && consumers_.size ( ) > 0 ) {
		/* Prepares the message and sends it to the consumers according to their policies. */
		output_message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_DATA );
		output_message.SetSequenceNumber ( message_sequence_number_++ );
		output_message.SetSourceStream ( processing_module_configurator_->GetFlowOut ( ) );

		vector < string > consumer_names;
		for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) {
			consumer_names.push_back ( c->first );
		}

		for ( uint c = 0; c < consumer_names.size ( ); ++c ) {
			UpdateConsumerCredits ( consumer_names[c], output_message );
			if ( !shutdown_notification_ && consumers_.find ( consumer_names[c] ) != consumers_.end ( ) ) {
				if ( consumers_[consumer_names[c]]->GetPolicy ( ) == Constants::POLICY_BROADCAST ) {
					consumers_[consumer_names[c]]->GetCommunicator ( )->BroadCast ( &output_message );
				}
				else {
					int destination = consumers_[consumer_names[c]]->GetNextToReceive ( output_message );
					consumers_[consumer_names[c]]->GetCommunicator ( )->Send ( &output_message, destination );
				}
			}
		}
	}
}

void ProcessingModule::SendCreditToProducer ( int instance, string producer_id ) {
	int credit = ComputeProducerCredit ( );
	producers_[producer_id]->SetCredit ( instance, credit );
	Message credit_message ( ( void* ) &credit, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT, sizeof(int) );
	producers_[producer_id]->GetCommunicator ( )->Send ( &credit_message, instance );
}

void ProcessingModule::SetConfigurator ( ProcessingModuleConfigurator* configurator ) {
	processing_module_configurator_ = configurator;
	vector < InputFlow >* inputs = processing_module_configurator_->GetInputs ( );
	for ( uint i = 0; i < inputs->size ( ); ++i ) {
		if ( inputs->at ( i ).GetPolicy ( ).compare ( Constants::POLICY_LABELED ) == 0 ) {
			ValidateLabelFunction ( inputs->at ( i ).GetPolicyFunctionFile ( ) );
		}
	}
}

void ProcessingModule::SetDataConsumerCredit ( string consumer_id, Message& credit_message ) {
	int credit = * ( ( int* ) credit_message.GetData ( ) );
	consumers_[consumer_id]->SetCredit ( credit_message.GetSource ( ), credit );
}

void ProcessingModule::SetInitialTime ( void ) {
	gettimeofday ( &initial_time_, NULL );
}

void ProcessingModule::SetRuntimeCommunicator ( MpiCommunicator* runtime_communicator ) {
	runtime_communicator_ = runtime_communicator;
}

void ProcessingModule::Shutdown ( void ) {
	runtime_communicator_->Synchronize ( );
	ComputeResourcesUsage ( );
	shutdown_notification_ = true;
}

void ProcessingModule::SynchronizeConsumers ( Message& message ) {
	message.SetOperationCode ( Constants::MESSAGE_OP_PROCESSING_MODULE_DATA );
	message.SetSequenceNumber ( message_sequence_number_++ );
	message.SetSourceStream ( processing_module_configurator_->GetFlowOut ( ) );
	for ( map < string, DataConsumer* >::iterator c = consumers_.begin ( ); c != consumers_.end ( ); ++c ) {
		c->second->GetCommunicator ( )->BroadCast ( &message );
	}
}

void ProcessingModule::TerminateModule ( void ) {
	/* Asks the runtime to terminate this module instances. */
	Message termination_message ( ( void* ) GetModuleName ( ).c_str ( ), Constants::MESSAGE_OP_TERMINATION, GetModuleName ( ).length ( ) + 1 );
	runtime_communicator_->Send ( &termination_message, Constants::COMM_ROOT_PROCESS );
	termination_requested_ = true;
}

void* ProcessingModule::ThreadDisconnectConsumers ( void* obj ) {
	reinterpret_cast < ProcessingModule * > ( obj )->DisconnectConsumers ( );
	pthread_exit ( NULL);
}

void* ProcessingModule::ThreadDisconnectProducers ( void* obj ) {
	reinterpret_cast < ProcessingModule * > ( obj )->DisconnectProducers ( );
	pthread_exit ( NULL);
}

void ProcessingModule::UpdateConsumerCredits ( string consumer_id, Message& received_message ) {

	/* checks whether all instances of the consumer have space. If one of those instances has no space, the code waits for a message updating the credit value. */
	if ( consumers_[consumer_id]->GetPolicy ( ) == Constants::POLICY_BROADCAST ) {
		UpdateCreditsForBroadcastConsumer ( consumer_id );
	}

	/* looks for the first instance that can receive the message in a round robin fashion */
	else if ( consumers_[consumer_id]->GetPolicy ( ) == Constants::POLICY_ROUND_ROBIN ) {
		UpdateCreditsForRoundRobinConsumer ( consumer_id, received_message );
	}

	/* checks if the specific instance of the consumer can receive the message */
	else if ( consumers_[consumer_id]->GetPolicy ( ) == Constants::POLICY_LABELED ) {
		UpdateCreditsForLabeledStreamConsumer ( consumer_id, received_message );
	}
}

void ProcessingModule::UpdateCreditsForBroadcastConsumer ( string consumer_id ) {
	/* Check all the instances until they have credits to receive the message. */
	for ( int i = 0; i < consumers_[consumer_id]->GetNumberInstances ( ); ++i ) {
		if ( consumers_[consumer_id]->GetCredit ( i ) == 0 ) {
			bool ready = false;
			while ( !shutdown_notification_ && !ready ) {
				usleep ( Constants::SLEEP_TIME );
				/* Deal with control message in case it exists. */
				int source = runtime_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
				if ( source != -1 ) {
					Message m ( NULL, Constants::MESSAGE_OP_ANY, 0 );
					runtime_communicator_->Receive ( source, &m );
					HandleRuntimeMessage ( m );
					if ( shutdown_notification_ || consumers_.find ( consumer_id ) == consumers_.end ( ) ) {
						return;
					}
				}
				/* Waits for credit announcement. */
				source = consumers_[consumer_id]->GetCommunicator ( )->Probe ( i, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT );
				if ( source != -1 ) {
					Message credit_announcement ( NULL, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT, 0 );
					consumers_[consumer_id]->GetCommunicator ( )->Receive ( i, &credit_announcement );
					SetDataConsumerCredit ( consumer_id, credit_announcement );
					ready = true;
				}
			}
		}
		consumers_[consumer_id]->SetCredit ( i, consumers_[consumer_id]->GetCredit ( i ) - 1 );
	}
}

void ProcessingModule::UpdateCreditsForLabeledStreamConsumer ( string consumer_id, Message& received_message ) {
	int instance_to_receive = consumers_[consumer_id]->GetNextToReceive ( received_message );
	if ( consumers_[consumer_id]->GetCredit ( instance_to_receive ) == 0 ) {
		bool ready = false;
		while ( !shutdown_notification_ && !ready ) {
			usleep ( Constants::SLEEP_TIME );
			/* Deal with control message in case it exists. */
			int source = runtime_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			if ( source != -1 ) {
				Message m ( NULL, Constants::MESSAGE_OP_ANY, 0 );
				runtime_communicator_->Receive ( source, &m );
				HandleRuntimeMessage ( m );
				if ( shutdown_notification_ || consumers_.find ( consumer_id ) == consumers_.end ( ) ) {
					return;
				}
			}
			source = consumers_[consumer_id]->GetCommunicator ( )->Probe ( instance_to_receive, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT );
			if ( source != -1 ) {
				Message credit_announcement ( NULL, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT, 0 );
				consumers_[consumer_id]->GetCommunicator ( )->Receive ( instance_to_receive, &credit_announcement );
				SetDataConsumerCredit ( consumer_id, credit_announcement );
				ready = true;
			}
		}
	}
	consumers_[consumer_id]->SetCredit ( instance_to_receive, consumers_[consumer_id]->GetCredit ( instance_to_receive ) - 1 );
}

void ProcessingModule::UpdateCreditsForRoundRobinConsumer ( string consumer_id, Message& received_message ) {
	int instance_to_receive;
	bool message_can_be_sent = false;

	/* checks if there is a consumer with credits to receive the message */
	for ( int i = 0; i < consumers_[consumer_id]->GetNumberInstances ( ) && !message_can_be_sent; ++i ) {
		instance_to_receive = consumers_[consumer_id]->GetNextToReceive ( received_message );
		if ( consumers_[consumer_id]->GetCredit ( instance_to_receive ) > 0 ) {
			message_can_be_sent = true;
		}
	}

	/* waits for a credit announcement message from some consumer */
	if ( !message_can_be_sent ) {
		while ( !shutdown_notification_ && !message_can_be_sent ) {
			usleep ( Constants::SLEEP_TIME );
			int source = runtime_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
			if ( source != -1 ) {
				Message m ( NULL, Constants::MESSAGE_OP_ANY, 0 );
				runtime_communicator_->Receive ( source, &m );
				HandleRuntimeMessage ( m );
				if ( shutdown_notification_ || consumers_.find ( consumer_id ) == consumers_.end ( ) ) {
					return;
				}
			}
			source = consumers_[consumer_id]->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT );
			if ( source != -1 ) {
				Message credit_announcement ( NULL, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT, 0 );
				instance_to_receive = source;
				consumers_[consumer_id]->GetCommunicator ( )->Receive ( instance_to_receive, &credit_announcement );
				SetDataConsumerCredit ( consumer_id, credit_announcement );
				message_can_be_sent = true;
			}
		}
	}
	else {
		int source;
		do {
			source = consumers_[consumer_id]->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT );
			if ( source != -1 ) {
				Message credit_announcement ( NULL, Constants::MESSAGE_OP_CREDIT_ANNOUNCEMENT, 0 );
				consumers_[consumer_id]->GetCommunicator ( )->Receive ( source, &credit_announcement );
				SetDataConsumerCredit ( consumer_id, credit_announcement );
			}
		}
		while ( source != -1 );
	}
	consumers_[consumer_id]->SetNextToReceive ( instance_to_receive );
	consumers_[consumer_id]->SetCredit ( instance_to_receive, consumers_[consumer_id]->GetCredit ( instance_to_receive ) - 1 );
}

void ProcessingModule::ValidateLabelFunction ( string policy_function_file ) {
	/* Tries to load the processing module label function library */
	void* policy_lib_ = dlopen ( policy_function_file.c_str ( ), RTLD_NOW );
	if ( !policy_lib_ ) {
		string error = dlerror ( );
		error_on_init_ = true;
		error_message_on_init_ = "cannot load the policy function library. " + error;
		return;
	}
	else {
		/* Loads the symbols from the processing module library*/
		create_function* create_policy_function_ = ( create_function* ) dlsym ( policy_lib_, "GetInstance" );
		if ( !create_policy_function_ ) {
			dlclose ( policy_lib_ );
			string error = dlerror ( );
			error_message_on_init_ = "cannot load symbols. " + error;
			return;
		}
	}
	dlclose ( policy_lib_ );
}

