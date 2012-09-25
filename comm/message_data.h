/**
 * \file comm/message_data.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMM_MESSAGE_DATA_H_
#define WATERSHED_COMM_MESSAGE_DATA_H_

/* C Libraries */
#include <arpa/inet.h>
#include <string.h>

/* C++ Libraries */
#include <vector>

/* Project's .h */
#include "common/constants.h"

using namespace std;

/**
 * \class IdentificationMessage
 * \brief Message containing a process identification.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class IdentificationMessage {

	public:

		/**
		 * \brief Identification message constructor. Creates a new instance.
		 * \return Not applicable.
		 */
		IdentificationMessage ( void ) {

		}

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~IdentificationMessage ( void ) {

		}

		/**
		 * \brief Retrieve the process identification number.
		 * \return The process identification number.
		 */
		int GetProcessIdentification ( void ) {
			return ntohl ( process_identification_ );
		}

		/**
		 * \brief Retrieve the host name where the process is being executed.
		 * \return The host name.
		 */
		string GetHostName ( void ) {
			return host_name_;
		}

		/**
		 * \brief Set the host name where the process is being executed.
		 * \param host_name The host name.
		 * \return Not applicable.
		 */
		void SetHostName ( string host_name ) {
			strcpy ( host_name_, host_name.c_str ( ) );
		}

		/**
		 * \brief Set the process identification number.
		 * \param process_identification The process identification number.
		 * \return Not applicable.
		 */
		void SetProcessIdentification ( int process_identification ) {
			process_identification_ = htonl ( process_identification );
		}

	protected:

	private:

		/** \brief Host name where the process is being executed. */
		char host_name_[Constants::MAX_LINE_SIZE];

		/** \brief Process identification number. */
		int process_identification_;
};

/**
 * \class RemoveInstanceMessage
 * \brief Message to remove a processing module instance.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2012
 */
class RemoveInstanceMessage {

	public:

		/**
		 * \brief Message constructor. Creates a new instance.
		 * \return Not applicable.
		 */
		RemoveInstanceMessage ( int instance_identification, string module_name ) {
			SetInstanceIdentification ( instance_identification );
			SetModuleName ( module_name );
		}

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~RemoveInstanceMessage ( void ) {

		}

		/** todo */
		int GetInstanceIdentification ( void ) {
			return ntohl ( instance_identification_ );
		}

		/**
		 * \brief Retrieves the processing module name.
		 * \return The processing module name;
		 */
		string GetModuleName ( void ) {
			return module_name_;
		}

		/** todo */
		void SetInstanceIdentification ( int instance_identification ) {
			instance_identification_ = htonl ( instance_identification );
		}

		/**
		 * \brief Set the processing module name.
		 * \param module_name The module name.
		 * \return Not applicable.
		 */
		void SetModuleName ( string module_name ) {
			strcpy ( module_name_, module_name.c_str ( ) );
		}

	protected:

	private:

		/** \brief The processing module name. */
		char module_name_[Constants::MAX_LINE_SIZE];

		/** \brief The instance identification. */
		int instance_identification_;
};

#endif
