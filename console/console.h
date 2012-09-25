/**
 * \file console/console.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_CONSOLE_CONSOLE_H_
#define WATERSHED_CONSOLE_CONSOLE_H_

/* C libraries */
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>

/* Project's .h */
#include "comm/message.h"
#include "comm/mpi/mpi_communicator.h"
#include "common/logger.h"

using namespace std;

/**
 * \class Console
 * \brief Implementation of the Watershed Console
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Console {

	public:

		/**
		 * \brief Default constructor. Creates a new Console instance.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \return Not applicable.
		 */
		Console ( int argc, char** argv );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Console ( void );

		/**
		 * \brief Connects to a server using the field server_port.
		 * \return Not applicable.
		 */
		void ConnectToServer ( void );

		/**
		 * \brief Disconnects from the server.
		 * \return Not applicable.
		 */
		void DisconnectFromServer ( void );

		/**
		 * \brief Parses a command sent by the user and communicates to the server.
		 * \return Not applicable.
		 */
		void ParseCommand ( void );

		/**
		 * \brief Receives a message from the server.
		 * \param message Pointer to the message buffer.
		 */
		void ReceiveFromServer ( Message* message );

		/**
		 * \brief Sends a message to the server.
		 * \param message Message to be sent.
		 */
		void SendToServer ( Message* message );

	protected:

	private:

		/**
		 * \brief Gets the command line parameters.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \return Not applicable.
		 */
		void GetParameters ( int argc, char* argv[] );

		/**
		 * \brief Reads server information from a file.
		 * \return Not applicable.
		 */
		void GetServerInformation ( void );

		/** \brief Server process identification. */
		int server_identification_;

		/** \brief The console logger. */
		Logger* console_logger_;

		/** \brief Communicator used to connect with a server. */
		MpiCommunicator* self_communicator_;

		/** \brief Communicator used to transfer commands to server. */
		MpiCommunicator* server_communicator_;

		/** \brief Command's arguments.*/
		vector<string> arguments_;

		/** \brief Command to be sent to the server. */
		string command_;

		/** \brief Name of the file containing the server information. */
		string server_info_file_name_;

		/** \brief Name of the port opened by the server. */
		string server_port_;
};

#endif  // WATERSHED_CONSOLE_CONSOLE_H_
