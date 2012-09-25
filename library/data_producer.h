/**
 * \file library/data_producer.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_DATA_PRODUCER_H_
#define WATERSHED_LIBRARY_DATA_PRODUCER_H_

/* C libraries */
#include <pthread.h>

/* Project libraries */
#include <comm/mpi/mpi_communicator.h>
#include <library/label_function.h>

using namespace std;

/**
 * \class DataProducer
 * \brief Implementation of Watershed data producer
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class DataProducer {

	public:

		/**
		 * \brief Creates a new DataProducer instance.
		 * \return Not applicable.
		 */
		DataProducer ( void );

		/**
		 * \brief Creates a new DataProducer instance.
		 * \param processing_module_name Name of the processing module.
		 * \return Not applicable.
		 */
		DataProducer ( string processing_module_name );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~DataProducer ( void );

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
		 * \brief Retrieves the number of instances of this data producer.
		 * \return The number of instances of this data producer.
		 */
		int GetNumberInstances ( void );

		/**
		 * \brief Retrieves a pointer to the data consumer module communicator.
		 * \return A pointer to the data consumer module communicator.
		 */
		MpiCommunicator* GetCommunicator ( void );

		/**
		 * \brief Returns the credit for this data producer.
		 * \param rank The rank of target instance.
		 * \return The credit for this data producer instance.
		 */
		int GetCredit ( int rank );

		/**
		 * \brief Retrieves the output stream from this data producer.
		 * \return The name of the output stream.
		 */
		string GetFlowOut ( void );

		/**
		 * \brief Retrieves the data consumer module name.
		 * \return The data consumer module name.
		 */
		string GetName ( void );

		/**
		 * todo
		 */
		void RemoveInstance ( int instance_rank );

		/**
		 * \brief Sets the data consumer module communicator.
		 * \param communicator A pointer to the module communicator.
		 * \return Not applicable.
		 */
		void SetCommunicator ( MpiCommunicator* communicator );

		/**
		 * \brief Sets the name of the output stream.
		 * \param flow_out The name of the output stream.
		 * \return Not applicable.
		 */
		void SetFlowOut ( string flow_out );

		/**
		 * \brief Sets the credit for the data producer.
		 * \param rank The producer rank.
		 * \param new_credit The value of the new credit.
		 * \return Not applicable.
		 */
		void SetCredit ( int rank, int new_credit );

		/**
		 * \brief Sets the data consumer module name.
		 * \param processing_module_name The data consumer module name.
		 * \return Not applicable.
		 */
		void SetProcessingModuleName ( string processing_module_name );

	protected:

	private:

		/** \brief Number of instances of this class. */
		static int number_of_class_instances_;

		/** \brief A mutex to control the access to the DataProducer class. */
		static pthread_mutex_t class_mutex_;

		/** \brief Communicator. */
		MpiCommunicator* communicator_;

		/** \brief Data consumer module name. */
		string name_;

		/** \brief Name of the output stream. */
		string flow_out_;

		/** \brief Credits for all instances. */
		vector < int > credits_;
};

#endif /* WATERSHED_LIBRARY_DATA_PRODUCER_H_ */
