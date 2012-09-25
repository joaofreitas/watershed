/**
 * \file library/input_flow.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#include "input_flow.h"

InputFlow::InputFlow ( void ) {
}

InputFlow::~InputFlow ( void ) {
}

string InputFlow::GetName ( void ) {
	return name_;
}

string InputFlow::GetPolicy ( void ) {
	return policy_;
}

string InputFlow::GetPolicyFunctionFile ( void ) {
	return policy_function_file_;
}

string InputFlow::GetQuery ( void ) {
	return query_;
}

void InputFlow::SetName ( string name ) {
	name_ = name;
}

void InputFlow::SetPolicy ( string policy ) {
	policy_ = policy;
}

void InputFlow::SetPolicyFunctionFile ( string policy_function_file ) {
	policy_function_file_ = policy_function_file;
}

void InputFlow::SetQuery ( string query ) {
	query_ = query;
}
