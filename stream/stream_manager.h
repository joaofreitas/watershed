/**
 * \file stream/stream_manager.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_STREAM_STREAM_MANAGER_H_
#define WATERSHED_STREAM_STREAM_MANAGER_H_

/* Project's .h */
#include "comm/message.h"
#include "comm/mpi/mpi_communicator.h"
#include "common/logger.h"
#include "library/configurator.h"
#include "library/processing_module_entry.h"

/* Other libraries */
#include <dbxml/DbXml.hpp>

using namespace DbXml;
using namespace std;

/**
 * \class StreamManager
 * \brief Implementation of a stream manager.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class StreamManager {

	public:

		/**
		 * \brief Constructor. Creates a new instance of StreamManager.
		 * \param argc Number of process arguments.
		 * \param argv Process arguments.
		 * \return Not applicable.
		 */
		StreamManager ( int argc, char** argv );

		/**
		 * \brief Default destructor.
		 * \return Not applicable.
		 */
		virtual ~StreamManager ( void );

		/**
		 * \brief Adds a new procesing module to the database daemon.
		 * \param communicator Processing module communicator.
		 * \param received_message Processing module configuration message.
		 * \return Not applicable.
		 */
		void AddProcessingModule ( MpiCommunicator* communicator, Message& received_message );

		/**
		 * \brief Exchange the initial information with the runtime daemons.
		 * \return Not applicable.
		 */
		void ExchangeInitialInformation ( void );

		/**
		 * \brief Runs the main loops of the database daemon.
		 * \return Not applicable.
		 */
		void Run ( void );

		/**
		 * \brief Do all the tasks needed for the shutdown step.
		 * \return Not applicable.
		 */
		void Shutdown ( void );

	protected:

	private:

		/**
		 * \brief Waits for connections from processing modules.
		 * \return Not applicable.
		 */
		void AcceptConnection ( void );

		/**
		 * \brief Waits for messages from other processes.
		 * \return Not applicable.
		 */
		void MainLoop ( void );

		/**
		 * \brief Queries for a processing module consumers.
		 * \param processing_module_name The name of the requester.
		 * \param received_message Message received from a module querying its consumers.
		 * \return Not applicable.
		 */
		void QueryConsumers ( string processing_module_name, Message& received_message );

		/**
		 * \brief Queries for a processing module producers.
		 * \param processing_module_name The name of the requester.
		 * \param received_message Message received from a module querying its producers.
		 * \return Not applicable.
		 */
		void QueryProducers ( string processing_module_name, Message& received_message );

		/**
		 * \brief Removes a processing module from the internal structures and disconnects from that module.
		 * \param received_message Message received asking for a module removal.
		 * \return Not applicable.
		 */
		void RemoveProcessingModule ( Message& received_message );

		/**
		 * todo
		 */
		void RemoveProcessingModuleInstance ( Message& received_message );

		/**
		 * todo
		 */
		void StartDatabase ( void );

		/** \brief Flag used to notify the process termination. */
		bool shutdown_notification_;

		/** \brief Active processing modules supported by the database daemons. */
		map<string, ProcessingModuleEntry*> active_processing_modules_;

		/** \brief Communicator used to exchange data with the other database daemons. */
		MpiCommunicator* group_communicator_;

		/** \brief Communicator used to exchange data with the runtime daemons. */
		MpiCommunicator* runtime_communicator_;

		/** todo*/
		string db_environment_dir_;

		/** \brief Opened port to accept connection from processing modules. */
		string port_name_;
};

#endif /* WATERSHED_STREAM_STREAM_MANAGER_H_ */
