/**
 * \file comm/mpi/mpi_communicator.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMM_MPI_COMMUNICATOR_H_
#define WATERSHED_COMM_MPI_COMMUNICATOR_H_

/* C libraries */
#include <mpi.h>

/* Project's .h */
#include "comm/communicator.h"

using namespace std;

/**
 * \class MpiCommunicator
 * \brief Implementation of a communicator using Open MPI.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class MpiCommunicator : public Communicator {

	public:

		/**
		 * \brief MpiCommunicator constructor. Creates a new instance with an intercommunicator linking the process group and the parent group.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \return Not applicable.
		 */
		MpiCommunicator ( int argc, char** argv );

		/**
		 * \brief MpiCommunicator constructor. Creates a new instance with an intracommunicator linking the process group or only the process.
		 * \param argc Command line arguments counter.
		 * \param argv Command line arguments.
		 * \param scope Scope of the communicator (WORLD, SELF)
		 * \return Not applicable.
		 */
		MpiCommunicator ( int argc, char** argv, int scope );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~MpiCommunicator ( void );

		/**
		 * \brief Retrieve the number of process in the communicator.
		 * \return Number of process in the communicator.
		 */
		int GetNumberProcesses ( void );

		/**
		 * \brief Retrieve the process rank in the communicator.
		 * \return Process rank in the communicator.
		 */
		int GetProcessRank ( void );

		/**
		 * \brief Waits until there is a message ready to be received.
		 * \param source Receives the process id of message source.
		 * \param tag You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag.
		 * \return The source of a message to be received.
		 */
		int Poll ( int source, int tag ) throw ( BadParameterException );

		/**
		 * \brief Tells if there is a message ready to be received.
		 * \param source Receives the process id of message source.
		 * \param tag You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag.
		 * \return The source of a message to be received.
		 */
		int Probe ( int source, int tag ) throw ( BadParameterException );

		/**
		 * \brief Receive data from a source.
		 * \param source Could be the id of specific process or COMM_ANY_SOURCE to receive from any process.
		 * \param data Buffer where received data will be stored.
		 * \return The source of the message.
		 */
		int Receive ( int source, Message* data ) throw ( BadParameterException );

		/**
		 * \brief Accept a connection in a given port.
		 * \param port_name Name of the opened port.
		 * \return A new instance of MpiCommunicator with an intercommunicator linking two groups of processes.
		 */
		MpiCommunicator* Accept ( string port_name );

		/**
		 * \brief Connect to a server using an MPI port.
		 * \param port Name of the port to be used.
		 * \return A new instance of MpiCommunicator with an intercommunicator linking two groups of processes.
		 */
		MpiCommunicator* Connect ( string port );

		/**
		 * \brief Spawn processes at a group of hosts.
		 * \param argv Command line arguments.
		 * \param commands List of programs to be spawned.
		 * \param hosts List of hosts.
		 * \param number_process Number of copies of each process.
		 * \param work_directory Process work directory.
		 * \return A new instance of MpiCommunicator with an intercommunicator linking two groups of processes.
		 */
		MpiCommunicator* Spawn ( vector < vector < string > >& argv, vector < string >& commands, vector < string >& hosts, int* number_process, string& work_directory ) throw ( ProcessSpawnningException );

		/**
		 * \brief Retrieve the host name in which the process is executing.
		 * \return Name of the host.
		 */
		string GetHostName ( void );

		/**
		 * \brief Open a communication port.
		 * \return Name of the opened port.
		 */
		string OpenPort ( void );

		/**
		 * \brief Gather data from all process in the communicator.
		 * \param output_message Data to be sent to the other process.
		 * \param input_messages Pointer to receive data.
		 * \return Not applicable.
		 */
		void AllGather ( Message* output_message, Message* input_messages );

		/**
		 * \brief Send data to all process in the communicator.
		 * \param data Message to be sent.GetDataSize
		 * \return Not applicable.
		 */
		void BroadCast ( Message* data ) throw ( BadParameterException );

		/**
		 * \brief Close an opened port.
		 * \param port_name Port to be closed.
		 * \return Not applicable.
		 */
		void ClosePort ( string port_name );

		/**
		 * \brief Disconnect a process from a communicator.
		 * \return Not applicable.
		 */
		void Disconnect ( void );

		/**
		 * \brief Locks the class mutex, allowing parallel access.
		 * \return Not applicable.
		 */
		void Lock ( void );

		/**
		 * todo
		 */
		void RemoveProcess(int process_rank);

		/**
		 * \brief Performs a synchronized broadcast. Waits until all the destinations start receiving the message.
		 * \param data The message to be sent.
		 * \return Not applicable.
		 */
		void SBroadCast ( Message* data ) throw ( BadParameterException );

		/**
		 * \brief Send a message to a specified destination.
		 * \param data Pointer to the data that will be sent.
		 * \param destination Id of process that data will be sent.
		 * \return Not applicable.
		 */
		void Send ( Message* data, int destination ) throw ( BadParameterException );

		/**
		 * \brief Send a message to a specified destination and waits to its receipt.
		 * \param data Pointer to the data that will be sent.
		 * \param destination Id of process that data will be sent.
		 * \return Not applicable.
		 */
		void SSend ( Message* data, int destination ) throw ( BadParameterException );

		/**
		 * \brief Synchronize all processes in the same communicator.
		 * \return Not applicable.
		 */
		void Synchronize ( void );

		/**
		 * \brief Unlock the class mutex.
		 * \return Not applicable.
		 */
		void Unlock ( void );

	protected:

	private:

		/**
		 * \brief MpiCommunicator constructor. Creates a new instance defining an intercommunicator or an intracommunicator.
		 * \param intracomm Intracommunicator.
		 * \param intercomm Intercommunicator.
		 * \return Not applicable.
		 */
		MpiCommunicator ( MPI::Intracomm intracomm, MPI::Intercomm intercomm );

		/** \brief  Number of active class instances. */
		static int number_instances_;

		/** \brief Intercommunicator linking two groups of processes. */
		MPI::Intercomm inter_communicator_;

		/** \brief Intracommunicator including all the processes. */
		MPI::Intracomm intra_communicator_;

		/** \brief A mutex to control the access to the MpiCommunicator class. */
		pthread_mutex_t class_mutex_;
};

#endif /* WATERSHED_COMM_MPI_COMMUNICATOR_H_ */
