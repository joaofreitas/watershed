/**
 * \file library/data_consumer.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "library/data_consumer.h"

int DataConsumer::number_of_class_instances_ = 0;
pthread_mutex_t DataConsumer::class_mutex_;

DataConsumer::DataConsumer ( void ) throw (FileOperationException) {
	try {
		DataConsumer ( Constants::EMPTY_ATTRIBUTE, Constants::EMPTY_ATTRIBUTE, Constants::POLICY_ROUND_ROBIN, Constants::EMPTY_ATTRIBUTE );
	}
	catch (FileOperationException& e) {
		throw e;
	}
}

DataConsumer::DataConsumer ( string policy_function_file_name, string processing_module_name, string receive_policy, string query_flow_in ) throw (FileOperationException) {
	try {
		++number_of_class_instances_;
		if (number_of_class_instances_ == 1) {
			pthread_mutex_init ( &class_mutex_, NULL );
		}
		instance_to_receive_ = 0;
		SetProcessingModuleName ( processing_module_name );
		SetPolicy ( receive_policy );
		OpenLibrary ( policy_function_file_name );
		SetQueryFlowIn ( query_flow_in );
	}
	catch (FileOperationException& e) {
		throw e;
	}
}

DataConsumer::~DataConsumer ( void ) {
	--number_of_class_instances_;
	if (number_of_class_instances_ == 0) {
		pthread_mutex_destroy ( &class_mutex_ );
	}
	if (policy_.compare ( Constants::POLICY_LABELED ) == 0) {
		delete (policy_object_);
		dlclose ( policy_lib_ );
	}
	credits_.clear ( );
	delete (communicator_);
}

MpiCommunicator* DataConsumer::GetCommunicator ( void ) {
	return communicator_;
}

int DataConsumer::GetCredit ( int rank ) {
	return credits_[rank];
}

string DataConsumer::GetName ( void ) {
	return name_;
}

int DataConsumer::GetNextToReceive ( Message& message ) {
	int response;
	if (policy_.compare ( Constants::POLICY_ROUND_ROBIN ) == 0) {
		response = instance_to_receive_ % communicator_->GetNumberProcesses ( );
		instance_to_receive_ = (instance_to_receive_ + 1) % communicator_->GetNumberProcesses ( );
	}
	else if (policy_.compare ( Constants::POLICY_LABELED ) == 0) {
		response = policy_object_->GetLabel ( message, communicator_->GetNumberProcesses ( ) );
	}

	return response;
}

int DataConsumer::GetNumberInstances ( void ) {
	return communicator_->GetNumberProcesses ( );
}

string DataConsumer::GetPolicy ( void ) {
	return policy_;
}

LabelFunction* DataConsumer::GetPolicyFunction ( void ) {
	return policy_object_;
}

void* DataConsumer::GetPolicyLib ( void ) {
	return policy_lib_;
}

string DataConsumer::GetQueryFlowIn ( void ) {
	return query_flow_in_;
}

void DataConsumer::Lock ( void ) {
	pthread_mutex_lock ( &class_mutex_ );
}

void DataConsumer::OpenLibrary ( string policy_function_file_name ) throw (FileOperationException) {
	if (policy_.compare ( Constants::POLICY_LABELED ) == 0) {
		policy_lib_ = dlopen ( policy_function_file_name.c_str ( ), RTLD_NOW );
		if (!policy_lib_) {
			string error = dlerror ( );
			string message = "cannot load label function library. " + error;
			throw FileOperationException ( message );
		}

		/* Load the symbols */
		create_policy_function_ = (create_function*) dlsym ( policy_lib_, "GetInstance" );
		if (!create_policy_function_) {
			dlclose ( policy_lib_ );
			string error = dlerror ( );
			string message = "cannot load symbols. " + error;
			throw FileOperationException ( message );
		}
		policy_object_ = create_policy_function_ ( );
	}
	else {
		policy_object_ = NULL;
	}
}

void DataConsumer::RemoveInstance ( int instance_rank ) {
	credits_.erase ( credits_.begin ( ) + instance_rank );
	credits_.assign ( credits_.size ( ), 0 );
	GetCommunicator ( )->RemoveProcess ( Constants::PROCESSING_MODULE_INVALID_INSTANCE );
}

void DataConsumer::SetCommunicator ( MpiCommunicator* communicator ) {
	communicator_ = communicator;
	credits_.assign ( GetNumberInstances ( ), 0 );
}

void DataConsumer::SetCredit ( int rank, int new_credit ) {
	credits_[rank] = new_credit;
}

void DataConsumer::SetNextToReceive ( int next_to_receive ) {
	instance_to_receive_ = next_to_receive;
}

void DataConsumer::SetPolicy ( string policy ) {
	policy_ = policy;
}

void DataConsumer::SetPolicyFunction ( LabelFunction* policy_function ) {
	policy_object_ = policy_function;
}

void DataConsumer::SetPolicyLib ( void* policy_lib ) {
	policy_lib_ = policy_lib;
}

void DataConsumer::SetProcessingModuleName ( string processing_module_name ) {
	name_ = processing_module_name;
}

void DataConsumer::SetQueryFlowIn ( string query_flow_in ) {
	query_flow_in_ = query_flow_in;
}

void DataConsumer::Unlock ( void ) {
	pthread_mutex_unlock ( &class_mutex_ );
}
