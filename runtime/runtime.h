/**
 * \file runtime/runtime.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_RUNTIME_RUNTIME_H_
#define WATERSHED_RUNTIME_RUNTIME_H_

/* C libraries */
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Other libraries */
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>

/* Project's .h */
#include <comm/mpi/mpi_communicator.h>
#include <common/logger.h>
#include <common/util.h>
#include <library/processing_module_entry.h>
#include <library/xml.h>
#include <runtime/configurator.h>
#include <scheduler/scheduler.h>

using namespace std;

/**
 * \class Runtime
 * \brief Implementation of the runtime.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Runtime {

	public:

		/**
		 * \brief Creates a new Runtime instance.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \return Not applicable.
		 */
		Runtime ( int argc, char** argv );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Runtime ( void );

		/**
		 * \brief Reads the server configuration file.
		 * \return Not applicable.
		 */
		void ReadConfigurationFile ( void ) throw ( FileOperationException, XMLParserException );

		/**
		 * \brief Server execution core.
		 * \return Not applicable.
		 */
		void Run ( void );

	protected:

	private:

		/**
		 * \brief Waits for a console connection using an opened port.
		 * \return A NULL pointer.
		 */
		static void* StartConsoleThread ( void* obj );

		/**
		 * \brief Waits for a communication from other cluster's machines.
		 * \return A NULL pointer.
		 */
		static void* StartServerThread ( void * obj );

		/**
		 * \brief Verifies whether a processing module is already running.
		 * \param processing_module_name The name of a processing module.
		 * \return True if the processing module is already running. False otherwise.
		 */
		bool ProcessingModuleRunning ( string processing_module_name );

		/**
		 * \brief Spawns the instances of a processing module.
		 * \param processing_module_configurator The processing module configurator.
		 * \param scheduler_result The scheduling result.
		 * \return A communicator to the new process group.
		 */
		MpiCommunicator* SpawnProcessingModuleInstances ( ProcessingModuleConfigurator* processing_module_configurator, map < string, int >* scheduler_result ) throw ( ProcessSpawnningException );

		/**
		 * \brief Adds a processing module to the local server.
		 * \param received_message Message received from the console.
		 * \return Not applicable.
		 */
		void AddProcessingModule ( Message& received_message ) throw ( ProcessSpawnningException, XMLParserException );

		/**
		 * \brief Creates a child process and exits the parent process. Makes a daemon process.
		 * \return Not applicable.
		 */
		void Daemonize ( void );

		/**
		 * todo
		 */
		void DoProcessingModuleInstanceRemoval ( Message& received_message );

		/**
		 * \brief Does the processing module removal from Watershed.
		 * \param received_message Message received from the console to remove a processing module.
		 * \return Not applicable.
		 */
		void DoProcessingModuleRemoval ( Message& received_message );

		/**
		 * \brief Exchange the initial information with server replicas.
		 * \return Not applicable.
		 */
		void ExchangeInitialInformation ( void );

		/**
		 * \brief Gets the command line parameters.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \return Not applicable.
		 */
		void GetParameters ( int argc, char* argv[] );

		/**
		 * todo
		 */
		void HandleDatabaseMessage ( Message& received_message );

		/**
		 * todo
		 */
		void HandleProcessingModuleMessage ( string processing_module_name, Message& received_message );

		/**
		 * todo
		 */
		void HandleRuntimeMessage ( Message& received_message );

		/**
		 * \brief Handles the signals received from the operating system.
		 * \param signal Signal code.
		 * \return Not applicable.
		 */
		void HandleSignal ( int signal );

		/**
		 * \brief Checks whether this manager is the owner of a specified module.
		 * \param received_message The message asking for the existence of a module.
		 * \param requester The requester manager.
		 * \return Not applicable.
		 */
		void IsOwnerOfProcessingModule ( Message received_message, int requester );

		/**
		 * \brief Locks a file to allow just a single instance of the server.
		 * \return Not applicable.
		 */
		void LockLocalResources ( void ) throw ( FileOperationException );

		/**
		 * todo
		 */
		void MigrateProcessingModuleInstance ( int instance, string module_name, string target_host );

		/**
		 * \brief Builds a list of the requested ports.
		 * \param is_query_manager Flag to identify the operation manager.
		 * \param processing_module_name The requester processing module.
		 * \param received_message The message containing the query.
		 * \return Not applicable.
		 */
		void QueryProcessingModulePorts ( bool is_query_manager, string processing_module_name, Message& received_message );

		/**
		 * \brief Removes a processing module from the runtime.
		 * \param is_removal_manager Flag to identify the operation manager.
		 * \param received_message Message received from the console to remove a processing module.
		 * \return Not applicable.
		 */
		void RemoveProcessingModule ( bool is_removal_manager, Message& received_message );

		/**
		 * todo
		 */
		void RemoveProcessingModuleInstance ( bool removal_manager, Message& received_message );

		/**
		 * \brief Do all the tasks needed for the shutdown step.
		 * \param is_shutdown_manager Flag to identify the operation manager.
		 * \param received_message Message received asking for shutdown.
		 * \return Not applicable.
		 */
		void Shutdown ( bool is_shutdown_manager, Message& received_message );

		/**
		 * \brief Spawns the database daemon whether there is a database running on the local host.
		 * \return Not applicable.
		 */
		void SpawnDatabaseDaemon ( void );

		/**
		 * \brief Waits connection from the opened port.
		 * \return Not applicable.
		 */
		void WaitConnections ( void );

		/**
		 * \brief Waits messages from the other processes.
		 * \return Not applicable.
		 */
		void WaitMessages ( void );

		/** \brief The status of the last operation. */
		bool last_operation_status_;

		/** \brief Notification of system shutdown. */
		bool shutdown_notification_;

		/** \brief Database process identification. */
		int database_identification_;

		/** \brief System logger. */
		Logger* runtime_logger_;

		/** \brief Active processing modules in this host. */
		map < string, ProcessingModuleEntry* > active_processing_modules_;

		/** \brief Communicator used to transfer data between servers. */
		MpiCommunicator* cluster_communicator_;

		/** \brief Communicator used to exchange data with a console. */
		MpiCommunicator* console_communicator_;

		/** \brief Communicator to send/receive data to/from database daemons. */
		MpiCommunicator* database_communicator_;

		/** \brief Communicator including only the local runtime. */
		MpiCommunicator* self_communicator_;

		/** Mutex used to control access to the shutdown notification. */
		pthread_mutex_t shutdown_notification_mutex_;

		/** \brief Thread to wait connection from console. */
		pthread_t console_thread_;

		/** \brief Thread used to exchange messages with other servers. */
		pthread_t server_thread_;

		/** \brief Runtime configurator. */
		RuntimeConfigurator* runtime_configurator_;

		/** \brief General scheduler used to allocate process. */
		Scheduler runtime_scheduler_;

		/** \brief Name of the configuration file. */
		string configuration_file_name_;

		/** \brief Database group port name. */
		string database_port_name_;

		/** \brief Name of the file to contain the server information. */
		string server_information_file_name_;

		/** \brief Name of the port opened by server. */
		string server_port_;
};

#endif  // WATERSHED_RUNTIME_RUNTIME_H_
