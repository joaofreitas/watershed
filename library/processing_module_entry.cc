/**
 * \file library/processing_module_entry.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#include "processing_module_entry.h"

int ProcessingModuleEntry::number_of_instances_ = 0;
pthread_mutex_t ProcessingModuleEntry::class_mutex_;

ProcessingModuleEntry::ProcessingModuleEntry ( MpiCommunicator* communicator, ProcessingModuleConfigurator* configurator ) {
	++number_of_instances_;
	if ( number_of_instances_ == 1 ) {
		pthread_mutex_init ( &class_mutex_, NULL );
	}
	SetCommunicator ( communicator );
	SetConfigurator ( configurator );
}

ProcessingModuleEntry::~ProcessingModuleEntry ( void ) {
	--number_of_instances_;
	if ( number_of_instances_ == 0 ) {
		pthread_mutex_destroy ( &class_mutex_ );
	}
	delete ( configurator_ );
	delete ( communicator_ );
}

MpiCommunicator* ProcessingModuleEntry::GetCommunicator ( void ) {
	return communicator_;
}

ProcessingModuleConfigurator* ProcessingModuleEntry::GetConfigurator ( void ) {
	return configurator_;
}

void ProcessingModuleEntry::Lock ( void ) {
	pthread_mutex_lock ( &class_mutex_ );
}

void ProcessingModuleEntry::SetCommunicator ( MpiCommunicator* communicator ) {
	communicator_ = communicator;
}

void ProcessingModuleEntry::SetConfigurator ( ProcessingModuleConfigurator* configurator ) {
	configurator_ = configurator;
}

void ProcessingModuleEntry::Unlock ( void ) {
	pthread_mutex_unlock ( &class_mutex_ );
}
