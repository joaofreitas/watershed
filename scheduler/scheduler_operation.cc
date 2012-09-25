/**
 * \file common/scheduler_operation.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#include <scheduler/scheduler_operation.h>

SchedulerOperation::SchedulerOperation ( void ) {

}

SchedulerOperation::~SchedulerOperation ( void ) {
}

int SchedulerOperation::GetInstanceRank ( void ) {
	return instance_rank_;
}

string SchedulerOperation::GetModuleName ( void ) {
	return module_name_;
}

int SchedulerOperation::GetOperationDescription ( void ) {
	return operation_description_;
}

string SchedulerOperation::GetTargetHost ( void ) {
	return target_host_;
}

void SchedulerOperation::SetInstanceRank ( int instance_rank_ ) {
	this->instance_rank_ = instance_rank_;
}

void SchedulerOperation::SetModuleName ( string module_name_ ) {
	this->module_name_ = module_name_;
}

void SchedulerOperation::SetOperationDescription ( int operation_description_ ) {
	this->operation_description_ = operation_description_;
}

void SchedulerOperation::SetTargetHost ( string target_host_ ) {
	this->target_host_ = target_host_;
}

