/**
 * \file library/configurator.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_CONFIGURATOR_H_
#define WATERSHED_LIBRARY_CONFIGURATOR_H_

/* Project's .h */
#include <comm/mpi/mpi_communicator.h>
#include <common/constants.h>
#include <common/util.h>
#include <common/xml_parser.h>
#include <library/input_flow.h>

/* Other libraries */
#include <sys/prctl.h>

using namespace std;

/**
 * \class ProcessingModuleConfigurator
 * \brief Implementation of the utilitarian class to configure a processing module.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ProcessingModuleConfigurator {

	public:

		/**
		 * \brief Default constructor. Creates a new ProcessingModuleConfigurator instance.
		 * \return Not applicable.
		 */
		ProcessingModuleConfigurator ( void );

		/**
		 * \brief Constructor. Creates a new ProcessingModuleConfigurator instance based on a XML file descriptor.
		 * \param parse_file ProcessingModule descriptor XML file name.
		 * \return Not applicable.
		 */
		ProcessingModuleConfigurator ( string parse_file ) throw ( XMLParserException );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~ProcessingModuleConfigurator ( void );

		/**
		 * \brief Retrieves the database daemon identification.
		 * \return The database daemon identification.
		 */
		int GetDatabasePeerIdentification ( void );

		/**
		 * \brief Retrieves the number of instances of a processing module.
		 * \return The number of instances of a processing module.
		 */
		int GetNumberInstances ( void );

		/**
		 * \brief Retrieves the number of messages received to terminate the module.
		 * \return The number of termination messages received.
		 */
		int GetNumberTerminationMessages ( void );

		/**
		 * \brief Retrieves the processing module's arguments.
		 * \return The arguments.
		 */
		string GetArguments ( void );

		/**
		 * \brief Returns the name of the configurator file.
		 * \return The configurator file name.
		 */
		string GetConfiguratorFileName ( void );

		/**
		 * \brief Retrieves the processing module's database port name.
		 * \return The processing module's database port name.
		 */
		string GetDatabasePortName ( void );

		/**
		 * \brief Retrieves the description of the processing module's output flow.
		 * \return The description of the processing module's output flow.
		 */
		string GetFlowOut ( void );

		/**
		 * \brief Retrieves the structure of an output flow.
		 * \return The structure of an output flow.
		 */
		string GetFlowOutStructure ( void );

		/**
		 * \brief Retrieves the name of the processing module's shared library file.
		 * \return The name of the processing module's shared library file.
		 */
		string GetLibraryFile ( void );

		/**
		 * \brief Retrieves the processing module's name.
		 * \return The processing module's name.
		 */
		string GetName ( void );

		/**
		 * \brief Retrieves the processing module port name.
		 * \return The processing module port name.
		 */
		string GetPortName ( void );

		/**
		 * \brief Retrieves the query of the filter's input flow.
		 * \return The query of the filter's input flow.
		 */
		string GetQueryFlowIn ( void );

		/**
		 * \brief Retrieves the processing module's running directory.
		 * \return The processing module's running directory.
		 */
		string GetRunningDirectory(void);

		/**
		 * \brief Retrieves the structure of an output flow.
		 * \return The structure of an output flow.
		 */
		string GetStructureFlowOut ( void );

		/**
		 * \brief Retrieves the processing module's demands.
		 * \return A list of demands.
		 */
		vector < string >* GetDemands ( void );

		/**
		 * \brief Retrieves the list of input streams.
		 * \return A vector containing all the input streams in a PM configurator.
		 */
		vector < InputFlow >* GetInputs ( void );

		/**
		 * \brief Adds a demand to a processing module configurator.
		 * \param demand Demand's name.
		 * \return Not applicable.
		 */
		void AddDemand ( string demand );

		/**
		 * \brief Adds an input stream to a PM configurator.
		 * \param input The new input flow to be added.
		 * \return Not applicable.
		 */
		void AddInput ( InputFlow input );

		/**
		 * \brief Prints all the information about the configurator.
		 * \return Not applicable.
		 */
		void Print ( void );

		/**
		 * \brief Set the processing module arguments.
		 * \param arguments List of processing module arguments.
		 * \return Not applicable.
		 */
		void SetArguments ( string arguments );

		/**
		 * \brief Sets the configurator file name.
		 * \param configurator_file_name The configurator file name.
		 * \return Not applicable.
		 */
		void SetConfiguratorFileName ( string configurator_file_name );

		/**
		 * \brief Set the database peer identification.
		 * \param database_peer_identification The database daemon identification.
		 * \return Not applicable.
		 */
		void SetDatabasePeerIdentification ( int database_peer_identification );

		/**
		 * \brief Sets the database port name to be used by the processing module.
		 * \return Not applicable.
		 */
		void SetDatabasePortName ( string database_port_name );

		/**
		 * \brief Sets the output flow.
		 * \param flow_out Description of an output flow.
		 * \return Not applicable.
		 */
		void SetFlowOut ( string flow_out );

		/**
		 * \brief Sets the structure of an output flow.
		 * \param flow_out_structure Structure of an output flow.
		 * \return Not applicable.
		 */
		void SetFlowOutStructure ( string flow_out_structure );

		/**
		 * \brief Sets the library file name to be used by a processing module.
		 * \param library_file Library's name.
		 * \return Not applicable.
		 */
		void SetLibraryFile ( string library_file );

		/**
		 * \brief Sets the processing module's name.
		 * \param name Name to be used.
		 * \return Not applicable.
		 */
		void SetName ( string name );

		/**
		 * \brief Sets the number of instances of a processing module.
		 * \param number_instances Number of instances.
		 * \return Not applicable.
		 */
		void SetNumberInstances ( int number_instances );

		/**
		 * \brief Set the number of messages received to terminate the processing module.
		 * \param number_termination_messages The number of received messages to terminate the processing module.
		 * \return Not applicable
		 */
		void SetNumberTerminationMessages ( int number_termination_messages );

		/**
		 * \brief Set the processing module port name.
		 * \param port_name Name of the opened port.
		 * \return Not applicable.
		 */
		void SetPortName ( string port_name );

	protected:

	private:

		/**
		 * \brief Fill the configurator items based on a XML file.
		 * \return Not applicable.
		 */
		void FillItems ( void ) throw ( XMLParserException );

		/** \brief Database with which the processing module will communicate. */
		int database_peer_identification_;

		/** \brief Number of instances. */
		int number_instances_;

		/** \brief Number of a batch processing module termination messages. */
		int number_termination_messages_;

		/** \brief Processing module's arguments. */
		string arguments_;

		/** \brief The configurator file name. */
		string configurator_file_name_;

		/** \brief Processing module's database port name. */
		string database_port_name_;

		/** \brief Description of an output flow. */
		string flow_out_;

		/** \brief Structure of an output flow. */
		string flow_out_structure_;

		/** \brief Processing module library's name. */
		string library_file_;

		/** \brief Processing module's name. */
		string name_;

		/** \brief Processing module port name. */
		string port_name_;

		/** \brief Query of an input flow. */
		string query_flow_in_;

		/** \brief The running directory. */
		string running_directory_;

		/** \brief Structure of an output flow. */
		string structure_flow_out_;

		/** \brief Processing module's demands. */
		vector < string > demands_;

		/** \brief Description of the input flows. */
		vector < InputFlow > inputs_;

		/**  \brief XML parser. */
		XMLParser processing_module_parser_;
};

#endif /* WATERSHED_LIBRARY_CONFIGURATOR_H_ */
