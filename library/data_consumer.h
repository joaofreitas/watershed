/**
 * \file library/data_consumer.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_DATA_CONSUMER_H_
#define WATERSHED_LIBRARY_DATA_CONSUMER_H_

/* C libraries */
#include <pthread.h>

/* Project libraries */
#include "comm/mpi/mpi_communicator.h"
#include "library/label_function.h"

using namespace std;

/**
 * \class DataConsumer
 * \brief Implementation of Watershed data consumer
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class DataConsumer {

	public:

		/**
		 * \brief Creates a new DataConsumer instance.
		 * \return Not applicable.
		 */
		DataConsumer ( void ) throw (FileOperationException);

		/**
		 * \brief Creates a new DataConsumer instance.
		 * \param policy_function_file_name Policy function file name.
		 * \param processing_module_name Name of the processing module.
		 * \param receive_policy Processing module policy to receive messages.
		 * \param query_flow_in The query to be aaplied on the input stream.
		 * \return Not applicable.
		 */
		DataConsumer ( string policy_function_file_name, string processing_module_name, string receive_policy, string query_flow_in ) throw (FileOperationException);

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~DataConsumer ( void );

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
		 * \brief Return the current credit for a consumer.
		 * \return the value of the credit.
		 */
		int GetCredit ( int rank );

		/**
		 * \brief Retrieves the next instance to receive the message based on the policy.
		 * \param message Message to be received by the data consumer module.
		 * \return The rank of the instance to receive the message.
		 */
		int GetNextToReceive ( Message& message );

		/**
		 * \brief Retrieves the number of instances in this consumer.
		 * \return The number of instances in this consumer.
		 */
		int GetNumberInstances ( void );

		/**
		 * \brief Retrieves a pointer to the policy function.
		 * \return A pointer to the policy function.
		 */
		LabelFunction* GetPolicyFunction ( void );

		/**
		 * \brief Retrieves a pointer to the data consumer module communicator.
		 * \return A pointer to the data consumer module communicator.
		 */
		MpiCommunicator* GetCommunicator ( void );

		/**
		 * \brief Retrieves the data consumer module name.
		 * \return The data consumer module name.
		 */
		string GetName ( void );

		/**
		 * \brief Retrieves the data consumer module policy to receive messages.
		 * \return The policy to receive messages.
		 */
		string GetPolicy ( void );

		/**
		 * \brief Retrieves the query of the consumer's input flow.
		 * \return The query of the consumer's input flow.
		 */
		string GetQueryFlowIn ( void );

		/**
		 * \brief Retrieves a pointer to the library to the policy function.
		 * \return A pointer to the function library.
		 */
		void* GetPolicyLib ( void );

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
		 * \brief Sets the credit for the data consumer.
		 * \param rank The consumer rank.
		 * \param new_credit The value of the new credit.
		 * \return Not applicable.
		 */
		void SetCredit ( int rank, int new_credit );

		/**
		 * \brief Forces the next instance to receive message.
		 * \param next_to_receive The next instance to receive message.
		 * \return Not applicable.
		 */
		void SetNextToReceive ( int next_to_receive );

		/**
		 * \brief Sets the data consumer policy.
		 * \param policy Data consumer policy to receive messages.
		 * \return Not applicable.
		 */
		void SetPolicy ( string policy );

		/**
		 * \brief Sets the data consumer policy function.
		 * \param policy_function A pointer to the data consumer policy function.
		 * \return Not applicable.
		 */
		void SetPolicyFunction ( LabelFunction* policy_function );

		/**
		 * \brief Sets the library to the policy function.
		 * \param policy_lib A pointer to the library of the policy function.
		 * \return Not applicable.
		 */
		void SetPolicyLib ( void* policy_lib );

		/**
		 * \brief Sets the data consumer module name.
		 * \param processsing_module_name The data consumer module name.
		 * \return Not applicable.
		 */
		void SetProcessingModuleName ( string processsing_module_name );

		/**
		 * \brief Sets the input flow query.
		 * \param query_flow_in Query of an input flow.
		 * \return Not applicable.
		 */
		void SetQueryFlowIn ( string query_flow_in );

	protected:

	private:

		/**
		 * \brief Opens the dynamic library with the policy function.
		 * \param policy_function_file_name Policy function file name.
		 * \return Not applicable.
		 */
		void OpenLibrary ( string policy_function_file_name ) throw (FileOperationException);

		/** \brief The number of instances of this class. */
		static int number_of_class_instances_;

		/** \brief A mutex to control the access to the DataConsumer class. */
		static pthread_mutex_t class_mutex_;

		/** \brief Pointer to the create instance function. */
		create_function* create_policy_function_;

		/** \brief Rank of the data consumer instance that will receive the next message. */
		int instance_to_receive_;

		/** \brief Pointer to the policy function. */
		LabelFunction* policy_object_;

		/** \brief Policy to the data consumer module communicator. */
		MpiCommunicator* communicator_;

		/** \brief Data consumer module name. */
		string name_;

		/** \brief Data consumer module policy to receive messages. */
		string policy_;

		/** \brief Credits for all the instances. */
		vector<int> credits_;

		/** \brief Pointer to the library of the policy function. */
		void* policy_lib_;

		/** \brief Query of an input flow. */
		string query_flow_in_;
};

#endif /* WATERSHED_LIBRARY_DATA_CONSUMER_H_ */
