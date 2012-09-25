/**
 * \file stream/stream_manager.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "stream_manager.h"

StreamManager::StreamManager ( int argc, char** argv ) {
	shutdown_notification_ = false;
	group_communicator_ = new MpiCommunicator ( argc, argv, Constants::COMM_SCOPE_WORLD );
	runtime_communicator_ = new MpiCommunicator ( argc, argv );
}

StreamManager::~StreamManager ( void ) {

	if (group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS) {
		group_communicator_->ClosePort ( port_name_ );
	}

	for (map<string, ProcessingModuleEntry*>::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it) {
		it->second->GetCommunicator ( )->Disconnect ( );
		delete (it->second);
	}
	active_processing_modules_.clear ( );

	delete (group_communicator_);
	delete (runtime_communicator_);
}

void StreamManager::AcceptConnection ( void ) {

	Message message_from_module;
	int source;

	try {
		group_communicator_->Synchronize ( );
		MpiCommunicator* new_processing_module_communicator = group_communicator_->Accept ( port_name_ );
		group_communicator_->Synchronize ( );

		source = new_processing_module_communicator->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
		message_from_module.SetOperationCode ( Constants::MESSAGE_OP_ANY );
		new_processing_module_communicator->Receive ( source, &message_from_module );

		switch (message_from_module.GetOperationCode ( )) {

			case Constants::MESSAGE_OP_ADD_PROCESSING_MODULE: {
				AddProcessingModule ( new_processing_module_communicator, message_from_module );
				break;
			}

			default: {
				break;
			}
		}

	}
	catch (exception e) {

	}
}

void StreamManager::AddProcessingModule ( MpiCommunicator* communicator, Message& received_message ) {

	/* Receives the processing module's configurator file name */
	string processing_module_configurator_file_name = (char*) received_message.GetData ( );
	ProcessingModuleConfigurator* new_processing_module_configurator = new ProcessingModuleConfigurator ( processing_module_configurator_file_name );
	new_processing_module_configurator->SetConfiguratorFileName ( processing_module_configurator_file_name );

	/* Creates the new entry for the new processing module. */
	ProcessingModuleEntry* new_entry = new ProcessingModuleEntry ( communicator, new_processing_module_configurator );
	active_processing_modules_[new_processing_module_configurator->GetName ( )] = new_entry;
}

void StreamManager::ExchangeInitialInformation ( void ) {
	IdentificationMessage my_information;
	int source;
	my_information.SetProcessIdentification ( group_communicator_->GetProcessRank ( ) );
	my_information.SetHostName ( group_communicator_->GetHostName ( ) );

	Message output_message ( &my_information, Constants::MESSAGE_OP_PRESENTATION, sizeof(my_information) );
	Message input_messages[group_communicator_->GetNumberProcesses ( )];
	Message input_external_messages[runtime_communicator_->GetNumberProcesses ( )];
	group_communicator_->AllGather ( &output_message, input_messages );
	runtime_communicator_->AllGather ( &output_message, input_external_messages );

	/* Receives the path where the database will stay. */
	Message db_environment_dir_received;
	db_environment_dir_received.SetOperationCode ( Constants::MESSAGE_OP_DB_ENVIRONMENT_DIR );
	source = runtime_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_DB_ENVIRONMENT_DIR );
	runtime_communicator_->Receive ( source, &db_environment_dir_received );
	db_environment_dir_ = (char*) db_environment_dir_received.GetData ( );

	/* If I am the master database daemon, I open a port for the group. */
	if (group_communicator_->GetProcessRank ( ) == Constants::COMM_ROOT_PROCESS) {
		port_name_ = group_communicator_->OpenPort ( );
		Message port_name_message ( (void*) port_name_.c_str ( ), Constants::MESSAGE_OP_PORT_NAME, port_name_.length ( ) );
		group_communicator_->BroadCast ( &port_name_message );
		runtime_communicator_->BroadCast ( &port_name_message );
	}

	Message port_name_received;
	port_name_received.SetOperationCode ( Constants::MESSAGE_OP_PORT_NAME );
	source = group_communicator_->Poll ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_PORT_NAME );
	group_communicator_->Receive ( source, &port_name_received );
	port_name_ = (char*) port_name_received.GetData ( );
	group_communicator_->Synchronize ( );

	StartDatabase ( );
}

void StreamManager::MainLoop ( void ) {

	Message received_message;
	int source;
	string processing_module_name;

	/* Waits message from the other cluster daemons */
	do {
		try {
			received_message.SetOperationCode ( Constants::MESSAGE_OP_ANY );

			/* Checks whether there is a message from runtime */
			source = runtime_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );

			if (source != -1) { /* Receives the message from runtime. */
				runtime_communicator_->Receive ( source, &received_message );
			}
			else { /* If there is no message from runtime, checks whether there is a message from other database daemons */
				source = group_communicator_->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
				if (source != -1) { /* Receives the message from database daemon. */
					group_communicator_->Receive ( source, &received_message );
				}
			}

			/* If there is no message from runtime neither other database daemons, checks whether there is a message from processing module */
			if (source == -1) {
				for (map<string, ProcessingModuleEntry*>::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it) {
					source = it->second->GetCommunicator ( )->Probe ( Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_ANY );
					if (source != -1) {
						it->second->GetCommunicator ( )->Receive ( source, &received_message );
						processing_module_name = it->first;
						break;
					}
				}
			}

			/* Process the  received message */
			if (!shutdown_notification_ && source != -1) {

				switch (received_message.GetOperationCode ( )) {

					case Constants::MESSAGE_OP_ACCEPT_CONNECT: {
						AcceptConnection ( );
						break;
					}

					case Constants::MESSAGE_OP_SHUTDOWN: {
						Shutdown ( );
						break;
					}

					case Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE: {
						RemoveProcessingModule ( received_message );
						break;
					}

					case Constants::MESSAGE_OP_REMOVE_INSTANCE: {
						RemoveProcessingModuleInstance ( received_message );
						break;
					}

					case Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS: {
						QueryConsumers ( processing_module_name, received_message );
						break;
					}

					case Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS: {
						QueryProducers ( processing_module_name, received_message );
						break;
					}

					default: {
						break;
					}
				}
			}
		}
		catch (exception e) {

		}
		if (source == -1) {
			usleep ( Constants::SLEEP_TIME );
		}
	} while (!shutdown_notification_);
}

void StreamManager::QueryConsumers ( string processing_module_name, Message& received_message ) {
	string flow_out = active_processing_modules_[processing_module_name]->GetConfigurator ( )->GetFlowOut ( );
	string message_data = "";

	if (flow_out.compare ( Constants::EMPTY_ATTRIBUTE ) != 0) {
		for (map<string, ProcessingModuleEntry*>::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it) {
			vector<InputFlow>* inputs = it->second->GetConfigurator ( )->GetInputs ( );
			for (uint i = 0; i < inputs->size ( ); ++i) {
				if (inputs->at ( i ).GetName ( ).compare ( flow_out ) == 0) {
					message_data.append ( it->first );
					message_data.append ( " " );
				}
			}
		}
	}

	Message output_message;
	output_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS );
	output_message.SetData ( (void*) message_data.c_str ( ), message_data.length ( ) + 1 );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->Send ( &output_message, received_message.GetSource ( ) );
}

void StreamManager::QueryProducers ( string processing_module_name, Message& received_message ) {
	vector<InputFlow>* inputs = active_processing_modules_[processing_module_name]->GetConfigurator ( )->GetInputs ( );
	string message_data = "";

	for (uint i = 0; i < inputs->size ( ); ++i) {
		for (map<string, ProcessingModuleEntry*>::iterator it = active_processing_modules_.begin ( ); it != active_processing_modules_.end ( ); ++it) {
			if (it->second->GetConfigurator ( )->GetFlowOut ( ).compare ( inputs->at ( i ).GetName ( ) ) == 0) {
				message_data.append ( it->first );
				message_data.append ( " " );
			}
		}
	}

	Message output_message;
	output_message.SetOperationCode ( Constants::MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS );
	output_message.SetData ( (void*) message_data.c_str ( ), message_data.length ( ) + 1 );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->Send ( &output_message, received_message.GetSource ( ) );
}

void StreamManager::RemoveProcessingModule ( Message& received_message ) {
	string processing_module_name = (char*) received_message.GetData ( );
	if (active_processing_modules_.find ( processing_module_name ) != active_processing_modules_.end ( )) {
		active_processing_modules_[processing_module_name]->GetCommunicator ( )->Disconnect ( );
		delete (active_processing_modules_[processing_module_name]);
		active_processing_modules_.erase ( processing_module_name );
	}
}

void StreamManager::RemoveProcessingModuleInstance ( Message& received_message ) {
	string processing_module_name = ((RemoveInstanceMessage*) received_message.GetData ( ))->GetModuleName ( );
	active_processing_modules_[processing_module_name]->GetCommunicator ( )->RemoveProcess ( Constants::PROCESSING_MODULE_INVALID_INSTANCE );
}

void StreamManager::Run ( void ) {
	MainLoop ( );
}

void StreamManager::Shutdown ( void ) {
	runtime_communicator_->Synchronize ( );
	shutdown_notification_ = true;
}

void StreamManager::StartDatabase ( void ) {
	u_int32_t envFlags = DB_RECOVER | DB_CREATE | DB_INIT_MPOOL | DB_INIT_LOCK | DB_INIT_TXN | DB_INIT_LOG;
	int dberr;
	DB_ENV *dbEnv = 0;
	dberr = db_env_create ( &dbEnv, 0 );
	if (dberr == 0) {
		dbEnv->set_cachesize ( dbEnv, 0, Constants::ENV_CACHE_SIZE, 1 );
		dbEnv->set_lk_detect ( dbEnv, DB_LOCK_DEFAULT ); // handle deadlocks
		dberr = dbEnv->open ( dbEnv, db_environment_dir_.c_str ( ), envFlags, 0 );
	}
	if (dberr) {
		std::cout << "Unable to create environment handle due to the following error: " << db_strerror ( dberr ) << std::endl;
		if (dbEnv)
			dbEnv->close ( dbEnv, 0 );
	}
	try {
		XmlManager mgr ( dbEnv, DBXML_ADOPT_DBENV );
		if (!mgr.existsContainer ( Constants::CONTAINER_NAME )) {
			XmlContainerConfig config;
			config.setTransactional ( true );
			XmlContainer cont = mgr.createContainer ( Constants::CONTAINER_NAME, config );
		}
	}
	catch (XmlException &xe) {
		// Error handling goes here
	}
	catch (std::exception &e) {
		// Error handling goes here
	}
}

