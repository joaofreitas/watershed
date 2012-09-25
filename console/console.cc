/**
 * \file console/console.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "console/console.h"

Console::Console ( int argc, char** argv ) {
	GetParameters ( argc, argv );
	self_communicator_ = new MpiCommunicator ( argc, argv, Constants::COMM_SCOPE_SELF );
	console_logger_ = new Logger ( "/dev/null", true, " " );
	GetServerInformation ( );
}

Console::~Console ( void ) {
	delete (self_communicator_);
	delete (server_communicator_);
}

void Console::ConnectToServer ( void ) {
	server_communicator_ = self_communicator_->Connect ( server_port_ );
	server_identification_ = 0;
}

void Console::DisconnectFromServer ( void ) {
	server_communicator_->Disconnect ( );
}

void Console::GetParameters ( int argc, char* argv[] ) {
#define OPTSTR "a:c:i:"
	char option;

	int index = 1;
	while ((option = getopt ( argc, argv, OPTSTR )) > 0) {
		switch (option) {
			case 'a':
				for (++index; index < argc; ++index) {
					arguments_.push_back ( argv[index] );
				}
				break;
			case 'c':
				command_ = optarg;
				index += 2;
				break;
			case 'i':
				server_info_file_name_ = optarg;
				index += 2;
				break;
		}
	}
}

void Console::GetServerInformation ( void ) {
	char line[Constants::MAX_LINE_SIZE];
	FILE* server_info_file = fopen ( server_info_file_name_.c_str ( ), "r" );
	fgets ( line, Constants::MAX_LINE_SIZE, server_info_file );
	server_port_ = line;
	fclose ( server_info_file );
}

void Console::ParseCommand ( void ) {
	Message* message_to_server;
	Message message_from_server;
	string log_message;

	if (command_ == Constants::COMMAND_ADD_PROCESSING_MODULE) {
		message_to_server = new Message ( (void*) arguments_[0].c_str ( ), Constants::MESSAGE_OP_ADD_PROCESSING_MODULE, arguments_[0].length ( ) + 1 );
		SendToServer ( message_to_server );
		message_from_server.SetOperationCode ( Constants::MESSAGE_OP_ANY );
		ReceiveFromServer ( &message_from_server );
		if (message_from_server.GetOperationCode ( ) == Constants::MESSAGE_OP_ADD_PROCESSING_MODULE_ACK) {
			log_message = arguments_[0] + " successfully added to " + Constants::SYSTEM_NAME;
			console_logger_->PrintInfo ( log_message );
		}
		else {
			console_logger_->PrintError ( (char*) message_from_server.GetData ( ) );
		}
	}
	else if (command_ == Constants::COMMAND_REMOVE_PROCESSING_MODULE) {
		message_to_server = new Message ( (void*) arguments_[0].c_str ( ), Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE, arguments_[0].length ( ) + 1 );
		SendToServer ( message_to_server );
		message_from_server.SetOperationCode ( Constants::MESSAGE_OP_ANY );
		ReceiveFromServer ( &message_from_server );
		if (message_from_server.GetOperationCode ( ) == Constants::MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK) {
			log_message = arguments_[0] + " successfully removed from " + Constants::SYSTEM_NAME;
			console_logger_->PrintInfo ( log_message );
		}
		else {
			console_logger_->PrintError ( (char*) message_from_server.GetData ( ) );
		}
	}
	else if (command_ == Constants::COMMAND_REMOVE_INSTANCE) {
		RemoveInstanceMessage message_data ( atoi ( arguments_[1].c_str ( ) ), arguments_[0] );
		message_to_server = new Message ( (void*) &message_data, Constants::MESSAGE_OP_REMOVE_INSTANCE, sizeof(message_data) );
		SendToServer ( message_to_server );
		message_from_server.SetOperationCode ( Constants::MESSAGE_OP_ANY );
		ReceiveFromServer ( &message_from_server );
		if (message_from_server.GetOperationCode ( ) == Constants::MESSAGE_OP_REMOVE_INSTANCE_ACK) {
			log_message = arguments_[0] + ", instance " + arguments_[1] + " successfully removed from " + Constants::SYSTEM_NAME;
			console_logger_->PrintInfo ( log_message );
		}
		else {
			console_logger_->PrintError ( (char*) message_from_server.GetData ( ) );
		}

	}
	else if (command_ == Constants::COMMAND_SHUTDOWN) {
		log_message = Constants::SYSTEM_NAME + " is going down";
		console_logger_->PrintInfo ( log_message );
		message_to_server = new Message ( NULL, Constants::MESSAGE_OP_SHUTDOWN, 0 );
		SendToServer ( message_to_server );
		log_message = Constants::SYSTEM_NAME + " stopped";
		console_logger_->PrintInfo ( log_message );
	}
	else {
		log_message = "command " + command_ + " not found";
		console_logger_->PrintError ( log_message );
		return;
	}
	delete (message_to_server);
}

void Console::ReceiveFromServer ( Message* message ) {
	server_communicator_->Poll ( server_identification_, message->GetOperationCode ( ) );
	server_communicator_->Receive ( server_identification_, message );
}

void Console::SendToServer ( Message* message ) {
	server_communicator_->Send ( message, server_identification_ );
}
