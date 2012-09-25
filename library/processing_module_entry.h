/**
 * \file library/processing_module_entry.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_PROCESSING_MODULE_ENTRY_H_
#define WATERSHED_LIBRARY_PROCESSING_MODULE_ENTRY_H_

#include "comm/mpi/mpi_communicator.h"
#include "library/configurator.h"

using namespace std;

/**
 * \class ProcessingModuleEntry
 * \brief Implementation of the entry representing a processing module entry.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ProcessingModuleEntry {

	public:

		/**
		 * \brief Constructor. Creates a new instance of a processing entry.
		 * \param communicator The communicator with the PM.
		 * \param configurator The PM configurator.
		 * \return Not applicable.
		 */
		ProcessingModuleEntry ( MpiCommunicator* communicator, ProcessingModuleConfigurator* configurator );

		/**
		 * \brief Destructor.
		 */
		virtual ~ProcessingModuleEntry ( void );

		/**
		 * \brief Locks the class mutex.
		 * \return Not applicable.
		 */
		static void Lock ( void );

		/**
		 * \brief Unlock the class mutex.
		 * \return Not applicable.
		 */
		static void Unlock ( void );

		/**
		 * \brief Retrieves the communicator to the PM entry.
		 * \return The PM communicator.
		 */
		MpiCommunicator* GetCommunicator ( void );

		/**
		 * \brief Retrieves the configurator for this PM entry.
		 * \return A pointer to the PM entry configurator.
		 */
		ProcessingModuleConfigurator* GetConfigurator ( void );

		/**
		 * \brief Sets the communicator.
		 * \param communicator The communicator to be used.
		 * \return Not applicable.
		 */
		void SetCommunicator ( MpiCommunicator* communicator );

		/**
		 * \brief Sets the PM entry configurator.
		 * \param configurator The configurator itself.
		 * \return Not applicable.
		 */
		void SetConfigurator ( ProcessingModuleConfigurator* configurator );

	protected:

	private:

		/** \brief A mutex to control the access to the ProcessingModuleEntry class. */
		static pthread_mutex_t class_mutex_;

		/** \brief The number of instance of this class. */
		static int number_of_instances_;

		/** \brief The PM communicator. */
		MpiCommunicator* communicator_;

		/** \brief The PM entry configurator. */
		ProcessingModuleConfigurator* configurator_;
};

#endif /* WATERSHED_LIBRARY_PROCESSING_MODULE_ENTRY_H_ */
