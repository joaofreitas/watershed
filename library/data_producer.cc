/**
 * \file library/data_producer.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include <library/data_producer.h>

int DataProducer::number_of_class_instances_ = 0;
pthread_mutex_t DataProducer::class_mutex_;

DataProducer::DataProducer ( void ) {
}

DataProducer::DataProducer ( string processing_module_name ) {
	++number_of_class_instances_;
	if ( number_of_class_instances_ == 1 ) {
		pthread_mutex_init ( &class_mutex_, NULL );
	}
	SetProcessingModuleName ( processing_module_name );
}

DataProducer::~DataProducer ( void ) {
	--number_of_class_instances_;
	if ( number_of_class_instances_ == 0 ) {
		pthread_mutex_destroy ( &class_mutex_ );
	}
	credits_.clear ();
	delete ( communicator_ );
}

MpiCommunicator* DataProducer::GetCommunicator ( void ) {
	return communicator_;
}

int DataProducer::GetCredit ( int rank ) {
	return credits_[rank];
}

string DataProducer::GetFlowOut ( void ) {
	return flow_out_;
}

string DataProducer::GetName ( void ) {
	return name_;
}

int DataProducer::GetNumberInstances ( void ) {
	return communicator_->GetNumberProcesses ();
}

void DataProducer::Lock ( void ) {
	pthread_mutex_lock ( &class_mutex_ );
}

void DataProducer::RemoveInstance ( int instance_rank ) {
	credits_.erase ( credits_.begin ( ) + instance_rank );
	credits_.assign(credits_.size(), 0);
	GetCommunicator ( )->RemoveProcess ( Constants::PROCESSING_MODULE_INVALID_INSTANCE );
}

void DataProducer::SetCommunicator ( MpiCommunicator* communicator ) {
	communicator_ = communicator;
	credits_.assign ( GetNumberInstances (), 0 );
}

void DataProducer::SetCredit ( int rank, int new_credit ) {
	credits_[rank] = new_credit;
}

void DataProducer::SetFlowOut ( string flow_out ) {
	flow_out_ = flow_out;
}

void DataProducer::SetProcessingModuleName ( string processing_module_name ) {
	name_ = processing_module_name;
}

void DataProducer::Unlock ( void ) {
	pthread_mutex_unlock ( &class_mutex_ );
}
