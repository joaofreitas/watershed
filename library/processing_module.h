/**
 * \file library/processing_module.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_PROCESSING_MODULE_H_
#define WATERSHED_LIBRARY_PROCESSING_MODULE_H_

/* C libraries */
#include <dlfcn.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>

/* Project's .h */
#include <comm/message.h>
#include <comm/communicator.h>
#include <comm/mpi/mpi_communicator.h>
#include <common/util.h>
#include <common/xml_query.h>
#include <library/configurator.h>
#include <library/data_consumer.h>
#include <library/data_producer.h>
#include <library/label_function.h>
#include <library/xml.h>

/* Other libraries */
#include <dbxml/DbXml.hpp>

using namespace std;
using namespace DbXml;

/**
 * \class ProcessingModule
 * \brief Implementation of a generic ProcessingModule.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ProcessingModule {

	public:

		/**
		 * \brief Default constructor. Creates a new ProcessingModule instance.
		 * \return Not applicable.
		 */
		ProcessingModule ( void );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~ProcessingModule ( void );

		/**
		 * \brief Retrieves the elapsed time until the call moment.
		 * \return A value representing the time in seconds.
		 */
		double GetClockTime ( void );

		/**
		 * \brief Retrieves the CPU consumption until the call moment.
		 * \return A value representing the time in seconds.
		 */
		double GetCPUTime ( void );

		/**
		 * \brief Retrieves the CPU System consumption until the call moment.
		 * \return A value representing the time in seconds.
		 */
		double GetSystemTime ( void );

		/**
		 * \brief Retrieves the CPU User consumption until the call moment.
		 * \return A value representing the time in seconds.
		 */
		double GetUserTime ( void );

		/**
		 * \brief Checks if an error occurred on the startup moment.
		 * \return The error condition.
		 */
		bool ErrorOnInit ( void );

		/**
		 * \brief Retrieves the number of instances of this processing module.
		 * \return The number of instances of this processing module.
		 */
		int GetNumberInstances ( void );

		/**
		 * \brief Retrieves the number of instances producing a stream.
		 * \param stream_name The stream name.
		 * \return The number of producers.
		 */
		int GetNumberOfProducers ( string stream_name );

		/**
		 * \brief Retrieves the number of instances consuming the output stream.
		 * \return The number of consumers.
		 */

		int GetNumberOfConsumers ( void );

		/**
		 * \brief Retrieves the instance rank.
		 * \return The instance rank.
		 */
		int GetRank ( void );

		/**
		 * \brief Retrieves an argument by its name.
		 * \param argument_name Argument name.
		 * \return Argument value.
		 */
		string GetArgument ( string argument_name );

		/**
		 * \brief Retrieves an error message occurred on the module startup if it has occurred.
		 * \return The error message.
		 */
		string GetErrorMessageOnInit ( void );

		/**
		 * \brief Retrieves the processing module name. It is useful in the task of print and file creation, for example.
		 * \return The processing module name.
		 */
		string GetModuleName ( void );

		/**
		 * \brief Receive a message and do some computation.
		 * \param message Message received.
		 * \return Not applicable.
		 */
		virtual void Process ( Message& message ) = 0;

		/**
		 * todo
		 */
		void DisconnectConsumers ( void );

		/**
		 * todo
		 */
		void DisconnectProducers ( void );

		/**
		 * \brief Executes the main loop of the processing module.
		 * \return Not applicable.
		 */
		void Run ( void );

		/**
		 * \brief Send a message to the processing module's consumers.
		 * \param output_message Message to be sent.
		 * \return Not applicable.
		 */
		void Send ( Message& output_message );

		/**
		 * \brief Sets the processing module configurator.
		 * \param configurator The processing module configurator.
		 * \return Not applicable.
		 */
		void SetConfigurator ( ProcessingModuleConfigurator* configurator );

		/**
		 * \brief Sets the initial time for this PM instance.
		 * \return Not applicable.
		 */
		void SetInitialTime ( void );

		/**
		 * \brief Sets the external communicator.
		 * \param runtime_communicator Communicator with the runtime.
		 * \return Not applicable.
		 */
		void SetRuntimeCommunicator ( MpiCommunicator* runtime_communicator );

		/**
		 * \brief Sends a synchronized broadcast message to all instances consuming the output stream.
		 * \param message The message to be sent.
		 * \return Not applicable.
		 */
		void SynchronizeConsumers ( Message& message );

		/**
		 * \brief Terminates a module.
		 * \return Not applicable.
		 */
		void TerminateModule ( void );

	protected:

	private:

		/**
		 * \brief Handles the signals received from the operating system.
		 * \param signal Signal code.
		 * \return Not applicable.
		 */
		static void HandleSignal ( int signal );

		/**
		 * todo
		 */
		static void* ThreadDisconnectConsumers ( void* obj );

		/**
		 * todo
		 */
		static void* ThreadDisconnectProducers ( void* obj );

		/**
		 * \brief Computes the credit value for the producer instances.
		 * \return The number of messages in the credit announcement.
		 */
		int ComputeProducerCredit ( void );

		/**
		 * \brief Retrieves the number of instances which send data to this module.
		 * \return The number of producer instances.
		 */
		int GetNumberProducerInstances ( void );

		/**
		 * todo
		 */
		void AcceptConnection ( void );

		/**
		 * \brief Adds a consumer module to this one.
		 * \param new_communicator Communicator between this module and the new consumer.
		 * \param received_message The first message received from the consumer.
		 * \return The log message to be sent to the runtime.
		 */
		string AddConsumer ( MpiCommunicator* new_communicator, Message& received_message ) throw ( FileOperationException );

		/**
		 * \brief Adds a producer module to this one.
		 * \param new_communicator Communicator between this module and the new producer.
		 * \param received_message The first message received from the producer.
		 * \return The log message to be sent to the runtime.
		 */
		string AddProducer ( MpiCommunicator* new_communicator, Message& received_message );

		/**
		 * \brief Computes the resource usage for the PM instance.
		 * \return Not applicable.
		 */
		void ComputeResourcesUsage ( void );

		/**
		 * todo
		 */
		void ConfigureProcess ( void );

		/**
		 * \brief Connects to all consumers.
		 * \return Not applicable.
		 */
		void ConnectToConsumers ( void ) throw ( FileOperationException );

		/**
		 * \brief Connects to a database daemon.
		 * \return Not applicable.
		 */
		void ConnectToDatabaseDaemon ( void );

		/**
		 * \brief Connects to all producers.
		 * \return Not applicable.
		 */
		void ConnectToProducers ( void );

		/**
		 * \brief Creates the processing module arguments table.
		 * \return Not applicable.
		 */
		void CreateArguments ( void );

		/**
		 * \brief Disconnects from a processing module.
		 * \param received_message Received message with the information to proceed with the disconnection.
		 * \return Not applicable.
		 */
		void DisconnectFromProcessingModule ( Message* received_message );

		/**
		 * todo
		 */
		void HandleDatabaseMessage ( Message& received_message );

		/**
		 * todo
		 */
		void HandleProcessingModuleMessage ( string processing_module_id, int source, Message& received_message );

		/**
		 * todo
		 */
		void HandleRuntimeMessage ( Message& received_message );

		/**
		 * \brief Initializes the processing module.
		 * \return Not applicable.
		 */
		void InitProcessingModule ( void );

		/**
		 * \brief Waits for messages from other processes.
		 * \return Not applicable.
		 */
		void MainLoop ( void );

		/**
		 * todo
		 */
		void ReceiveLastMessages ( string module_name );

		/**
		 * todo
		 */
		void RemoveConsumerInstance ( Message& received_message );

		/**
		 * todo
		 */
		void RemoveInstance ( Message& received_message );

		/**
		 * todo
		 */
		void RemovePeerInstance ( Message& received_message );

		/**
		 * todo
		 */
		void RemoveProducerInstance ( Message& received_message );

		/**
		 * \brief Sends a credit message to a producer.
		 * \param instance The instance to receive the credit announcement.
		 * \param producer_id The identification of the producer in the internal data structure.
		 * \return Not applicable.
		 */
		void SendCreditToProducer ( int instance, string producer_id );

		/**
		 * \brief Sets the credit this instance has to send to a consumer.
		 * \param consumer_id The internal identification for the consumer.
		 * \param credit_message The message containing the credit announcement.
		 * \return Not applicable.
		 */
		void SetDataConsumerCredit ( string consumer_id, Message& credit_message );

		/**
		 * \brief Do all the tasks needed for the shutdown step.
		 * \return Not applicable.
		 */
		void Shutdown ( void );

		/**
		 * \brief Adjusts the credits for all consumers' instances in a send operation.
		 * \param consumer_id The internal identification for the consumer.
		 * \param received_message the message to be sent.
		 * \return Not applicable.
		 */
		void UpdateConsumerCredits ( string consumer_id, Message& received_message );

		/**
		 * \brief Adjusts the credits for all broadcast consumers' instances in a send operation.
		 * \param consumer_id The internal identification for the consumer.
		 * \return Not applicable.
		 */
		void UpdateCreditsForBroadcastConsumer ( string consumer_id );

		/**
		 * \brief Adjusts the credits for all labeled stream consumers' instances in a send operation.
		 * \param consumer_id The internal identification for the consumer.
		 * \param received_message The message to be sent.
		 * \return Not applicable.
		 */
		void UpdateCreditsForLabeledStreamConsumer ( string consumer_id, Message& received_message );

		/**
		 * \brief Adjusts the credits for round robin all consumers' instances in a send operation.
		 * \param consumer_id The internal identification for the consumer.
		 * \param received_message The message to be sent.
		 * \return Not applicable.
		 */
		void UpdateCreditsForRoundRobinConsumer ( string consumer_id, Message& received_message );

		/**
		 * \brief Validates the labeled stream function file.
		 * \param policy_function_file The name of the file to be validated.
		 * \return Not applicable.
		 */
		void ValidateLabelFunction ( string policy_function_file );

		/** \brief ProcessingModule arguments. */
		static map < string, string > arguments_;

		/** todo */
		static ProcessingModule* this_instance_;

		/** \brief Flag used to indicates an error event on the startup step. */
		bool error_on_init_;

		/** \brief Flag used to stop threads when stopping the process. */
		bool shutdown_notification_;

		/** \brief Flag used to indicates that the PM module has asked to terminate. */
		bool termination_requested_;

		/** \brief The CPU user time. */
		double user_time_;

		/** \brief The CPU system time. */
		double system_time_;

		/** \brief Sequence number of a message. */
		int message_sequence_number_;

		/** \brief Communicator with the database group. */
		MpiCommunicator* database_communicator_;

		/** \brief Communicator including all the processing module instances. */
		MpiCommunicator* group_communicator_;

		/** \brief Communicator with the runtime. */
		MpiCommunicator* runtime_communicator_;

		/** \brief The processing module configurator. */
		ProcessingModuleConfigurator* processing_module_configurator_;

		/** \brief The configurator file name. */
		string configurator_file_name_;

		/** \brief The database environment directory. */
		string db_environment_dir_;

		/** \brief The error occured on the startup step, when applicable. */
		string error_message_on_init_;

		/** \brief The initial time of execution. */
		struct timeval initial_time_;

		/** \brief Module consumers. */
		map < string, DataConsumer* > consumers_;

		/** \brief Module producers. */
		map < string, DataProducer* > producers_;

		/** \brief The queris for all consumers. */
		vector < string > queries_flows_consumers_;
};

/** \brief The types of the class factories. */
typedef ProcessingModule* create_t ( );

/** \brief Macro used to create a new instance of a specialized processing module. */
#define RegisterModule( class_name ) extern "C" ProcessingModule* GetInstance() { return new class_name; }

#endif  // WATERSHED_LIBRARY_PROCESSING_MODULE_H_
