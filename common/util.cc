/**
 * \file common/util.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "common/util.h"

vector < string > Util::TokenizeString ( const string &delimiters, const string &word ) {

	vector < string > tokens;

	// Skip delimiters at beginning.
	string::size_type lastPos = word.find_first_not_of ( delimiters, 0 );

	// Find first "non-delimiter".
	string::size_type pos = word.find_first_of ( delimiters, lastPos );

	while ( string::npos != pos || string::npos != lastPos ) {
		// Found a token, add it to the vector.
		tokens.push_back ( word.substr ( lastPos, pos - lastPos ) );
		// Skip delimiters.  Note the "not_of"
		lastPos = word.find_first_not_of ( delimiters, pos );
		// Find next "non-delimiter"
		pos = word.find_first_of ( delimiters, lastPos );
	}
	return tokens;
}

void Util::Error ( MpiCommunicator* communicator, string message ) {
	Message output_message;
	output_message.SetOperationCode ( Constants::MESSAGE_OP_ERROR_LOG );
	output_message.SetData ( ( void* ) message.c_str ( ), message.length ( ) + 1 );
	communicator->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
}

void Util::Information ( MpiCommunicator* communicator, string message ) {
	Message output_message;
	output_message.SetOperationCode ( Constants::MESSAGE_OP_INFO_LOG );
	output_message.SetData ( ( void* ) message.c_str ( ), message.length ( ) + 1 );
	communicator->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
}

void Util::Warning ( MpiCommunicator* communicator, string message ) {
	Message output_message;
	output_message.SetOperationCode ( Constants::MESSAGE_OP_WARNING_LOG );
	output_message.SetData ( ( void* ) message.c_str ( ), message.length ( ) + 1 );
	communicator->Send ( &output_message, Constants::COMM_ROOT_PROCESS );
}

string Util::FloatToString(float number) {
	char number_representation[MAX_NUMBER_REPRESENTATION_SIZE];
	sprintf(number_representation,"%f", number);
	string response = number_representation;
	return response;
}

string Util::IntegerToString(int number) {
	char number_representation[MAX_NUMBER_REPRESENTATION_SIZE];
	sprintf(number_representation,"%d", number);
	string response = number_representation;
	return response;
}
