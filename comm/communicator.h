/**
 * \file comm/communicator.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMM_COMMUNICATOR_H_
#define WATERSHED_COMM_COMMUNICATOR_H_

/* C libraries */
#include <stdlib.h>

/* C++ libraries */
#include <string>
#include <iostream>
#include <vector>
#include <string>

/* Project's .h */
#include "comm/message.h"
#include "common/exceptions.h"

using namespace std;

/**
 * \class Communicator
 * \brief Communicator interface.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Communicator {

	public:

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Communicator ( void ) {
		}
		;

		/**
		 * \brief Accept a connection in a given port.
		 * \param port_name Name of the opened port.
		 * \return A new communicator linking the processes.
		 */
		virtual Communicator* Accept ( string port_name ) = 0;

		/**
		 * \brief Connect to a server using an MPI port.
		 * \param port Name of the port to be used.
		 * \return A new communicator linking the processes.
		 */
		virtual Communicator* Connect ( string port ) = 0;

		/**
		 * \brief Spawn processes at a group of hosts.
		 * \param argv Command line arguments.
		 * \param commands List of programs to be spawned.
		 * \param hosts List of hosts.
		 * \param number_process Number of copies of each process.
		 * \param work_directory Process work directory.
		 * \return New communicator between the two groups.
		 */
		virtual Communicator* Spawn ( vector < vector < string > >& argv, vector < string >& commands, vector < string >& hosts, int* number_process, string& work_directory ) throw ( ProcessSpawnningException ) = 0;

		/**
		 * \brief Retrieve the number of processes in the communicator.
		 * \return Number of processes in the communicator.
		 */
		virtual int GetNumberProcesses ( void ) = 0;

		/**
		 * \brief Retrieve the process rank in the communicator.
		 * \return Process rank in the communicator.
		 */
		virtual int GetProcessRank ( void ) = 0;

		/**
		 * \brief Waits until there is a message ready to be received.
		 * \param source Receives the process id of message source.
		 * \param tag You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag.
		 * \return The source of a message to be received.
		 */
		virtual int Poll ( int source, int tag ) = 0;

		/**
		 * \brief Tells if there is a message ready to be received.
		 * \param source Receives the process id of message source.
		 * \param tag You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag.
		 * \return The source of a message to be received.
		 */
		virtual int Probe ( int source, int tag ) = 0;

		/**
		 * \brief Receive data from a source.
		 * \param source Could be the id of specific process or COMM_ANY_SOURCE to receive from any process.
		 * \param data Buffer where received data will be stored.
		 * \return The source of the message.
		 */
		virtual int Receive ( int source, Message* data ) = 0;

		/**
		 * \brief Retrieve the host name in which the process is executing.
		 * \return Name of the host.
		 */
		virtual string GetHostName ( void ) = 0;

		/**
		 * \brief Open a communication port.
		 * \return Name of the opened port.
		 */
		virtual string OpenPort ( void ) = 0;

		/**
		 * \brief Gather data from all processes in the communicator.
		 * \param output_message Data to be sent to the other processes.
		 * \param input_messages Pointer to receive data.
		 * \return Not applicable.
		 */
		virtual void AllGather ( Message* output_message, Message* input_messages ) = 0;

		/**
		 * \brief Send data to all processes in the communicator.
		 * \param data Message to be sent.
		 * \return Not applicable.
		 */
		virtual void BroadCast ( Message* data ) = 0;

		/**
		 * \brief Close an opened port.
		 * \param port_name Port to be closed.
		 * \return Not applicable.
		 */
		virtual void ClosePort ( string port_name ) = 0;

		/**
		 * \brief Disconnect from a communicator.
		 * \return Not applicable.
		 */
		virtual void Disconnect ( void ) = 0;

		/**
		 * todo
		 */
		virtual void RemoveProcess(int process_rank) = 0;

		/**
		 * todo
		 */
		virtual void SBroadCast ( Message* data ) = 0;

		/**
		 * \brief Send a message to a specified destination.
		 * \param data Pointer to the message to be sent.
		 * \param destination Id of process that data will be sent.
		 * \return Not applicable.
		 */
		virtual void Send ( Message* data, int destination ) = 0;

		/**
		 * \brief Send a synchronized message. Remains blocked until the destination starts receiving.
		 * \param data Pointer to the message to be sent.
		 * \param destination Id of process that data will be sent.
		 * \return Not applicable.
		 */
		virtual void SSend ( Message* data, int destination ) = 0;

	protected:

	private:

};

#endif /*WATERSHED_COMM_COMMUNICATOR_H_*/
