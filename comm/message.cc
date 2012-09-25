/**
 * \file comm/message.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "comm/message.h"

Message::Message ( void ) {
	data_size_ = 0;
}

Message::Message ( void* data, int size ) {
	SetData ( data, size );
	SetSequenceNumber ( 0 );
}

Message::Message ( void* data, int operation_code, int size ) {
	SetOperationCode ( operation_code );
	SetData ( data, size );
	SetSequenceNumber ( 0 );
}

Message::~Message ( void ) {

}

void* Message::GetData ( void ) {
	return &data_;
}

int Message::GetDataSize ( void ) {
	return ntohl ( data_size_ );
}

int Message::GetOperationCode ( void ) {
	return ntohl ( operation_code_ );
}

int Message::GetSequenceNumber ( void ) {
	return ntohl ( sequence_number_ );
}

int Message::GetSize ( void ) {
	int effective_size = sizeof(Message) - ( Constants::MAX_DATA_SIZE - GetDataSize ( ) );
	return effective_size;
}

int Message::GetSource ( void ) {
	return ntohl ( source_ );
}

string Message::GetSourceStream(void) {
	return source_stream_;
}

int Message::GetTimestamp ( void ) {
	return ntohl ( timestamp_ );
}

void Message::SetData ( void* data, int size ) {
	if ( size >= 0 ) {
		data_size_ = htonl ( size );
		memcpy ( data_, data, size );
	}
	else {
		data_size_ = htonl ( 0 );
		memcpy ( data_, data, 0 );
	}
}

void Message::SetOperationCode ( int operation_code ) {
	operation_code_ = htonl ( operation_code );
}

void Message::SetSequenceNumber ( int sequence_number ) {
	sequence_number_ = htonl ( sequence_number );
}

void Message::SetSource ( int source ) {
	source_ = htonl ( source );
}

void Message::SetSourceStream(string source_stream) {
	strcpy(source_stream_, source_stream.c_str());
}

void Message::SetTimestamp ( int timestamp ) {
	timestamp_ = htonl ( timestamp );
}
