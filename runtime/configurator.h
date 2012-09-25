/**
 * \file runtime/configurator.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_RUNTIME_CONFIGURATOR_H_
#define WATERSHED_RUNTIME_RUNTIME_CONFIGURATOR_H_

/* C++ libraries */
#include <map>

/* Project's .h */
#include <runtime/host.h>
#include <common/xml_parser.h>

using namespace std;

/**
 * \class RuntimeConfigurator
 * \brief Implementation of Watershed configurator
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class RuntimeConfigurator {

	public:
		/**
		 * \brief Creates a new RuntimeConfigurator instance.
		 * \param parse_file Name of the XML startup file.
		 * \return Not applicable.
		 */
		RuntimeConfigurator ( string parse_file ) throw ( XMLParserException );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		~RuntimeConfigurator ( void );

		/**
		 * \brief Retrieves the hosts included in the cluster.
		 * \return The hosts included in the cluster.
		 */
		map < string, Host >* GetHosts ( void );

		/**
		 * \brief Retrieves the database daemon arguments.
		 * \return The database daemon arguments.
		 */
		string GetDBArguments ( void );

		/**
		 * \brief Retrieves the database executable name.
		 * \return The database executable name.
		 */
		string GetDBExeName ( void );

		/**
		 * \brief Retrieves the runtime executable directory.
		 * \return The runtime executable directory.
		 */
		string GetExeDir ( void );

		/**
		 * \brief Retrieves the runtime information file name.
		 * \return The runtime information file name.
		 */
		string GetInfoFile ( void );

		/**
		 * \brief Retrieves the runtime lock file name.
		 * \return The runtime lock file name.
		 */
		string GetLockFile ( void );

		/**
		 * \brief Retrieves the runtime log file name.
		 * \return The runtime log file name.
		 */
		string GetLogFile ( void );

		/**
		 * \brief Retrieves the Open MPI installation directory.
		 * \return The Open MPI installation directory.
		 */
		string GetOMPIPrefix ( void );

		/**
		 * \brief Retrieves the processing module executable name.
		 * \return The processing module executable name.
		 */
		string GetProcessingModuleExeName ( void );

		/**
		 * \brief Retrieves the runtime running directory.
		 * \return The runtime running directory.
		 */
		string GetRunningDir ( void );

		/**
		 * \brief Retrieves the name of the console executable.
		 * \return The name of the console executable.
		 */
		string GetServerConsole ( void );

		/**
		 * \brief Retrieves the runtime home directory.
		 * \return The runtime home directory.
		 */
		string GetServerHome ( void );

		/**
		 * \brief Retrieves the server name.
		 * \return The server name.
		 */
		string GetServerName ( void );

		/**
		 * \brief Fills the configurator items.
		 * \return Not applicable.
		 */
		void FillConfiguratorItems ( void ) throw ( XMLParserException );

		/**
		 * \brief Print all the information in a configurator.
		 * \return Not applicable.
		 */
		void Print(void);

		/**
		 * \brief Sets the runtime information file name.
		 * \param info_file Information file name.
		 */
		void SetInfoFile ( string info_file );

		/**
		 * \brief Sets the runtime lock file name.
		 * \param lock_file Lock file name.
		 */
		void SetLockFile ( string lock_file );

		/**
		 * \brief Sets the runtime log file name.
		 * \param log_file Log file name.
		 */
		void SetLogFile ( string log_file );

	protected:

	private:

		/** \brief Cluster's hosts. */
		map < string, Host > hosts_;

		/** \brief Database arguments. */
		string database_arguments_;

		/** \brief Database executable name. */
		string db_executable_name_;

		/** \brief Runtime executable directory. */
		string exe_dir_;

		/** \brief Runtime information file name. */
		string info_file_;

		/** \brief Runtime lock file name. */
		string lock_file_;

		/** \brief Runtime log file name. */
		string log_file_;

		/** \brief Open MPI installation directory. */
		string ompi_prefix_;

		/** \brief ProcessingModule executable name. */
		string processing_module_executable_name_;

		/** \brief Runtime running directory. */
		string running_dir_;

		/** \brief Runtime home directory. */
		string server_home_;

		/** \brief Runtime executable name. */
		string server_name_;

		/** \brief XML parser. */
		XMLParser configuration_parser_;
};

#endif /* WATERSHED_RUNTIME_CONFIGURATOR_H_ */
