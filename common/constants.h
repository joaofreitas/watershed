/**
 * \file common/constants.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_CONSTANTS_H_
#define WATERSHED_COMMON_CONSTANTS_H_

/** C++ libraries */
#include <string>

/* Other libraries */
#include <dbxml/DbXml.hpp>

using namespace std;
using namespace DbXml;

/**
 * \class Constants
 * \brief Utilitary class including a set of constants
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Constants {

	public:

		/* ----- Limits ---------------------------------------------------------------------------------------------- */

		/** \brief Maximum size of a message data. */
		static const int MAX_DATA_SIZE = 15000;

		/** \brief Maximum number of a processing module demands. */
		static const int MAX_DEMANDS = 10;

		/** \brief Maximum size of a string line. */
		static const int MAX_LINE_SIZE = 300;

		/** \brief Constant of sleeping to consume less CPU time. */
		static const int SLEEP_TIME = 20;

		/** \brief Maximum size of an integer transformed in a string. */
		static const int MAX_INT_TO_STRING_LENGTH = 5;

		/** \brief Maximum deadlock retries.  */
		static const int MAX_DEADLOCK_RETRIES = 5;

		/** \brief Berkeley DB cache size (25 MB).  */
		static const u_int32_t ENV_CACHE_SIZE = 25 * 1024 * 1024;

		/* ----- System name ----------------------------------------------------------------------------------------- */
		static const string SYSTEM_NAME;

		/* ----- Message opcodes ------------------------------------------------------------------------------------- */

		/** \brief Code of any type of message. */
		static const int MESSAGE_OP_ANY = -1;

		/** \brief Code of a presentation message. */
		static const int MESSAGE_OP_PRESENTATION = 0;

		/** \brief Code of an add-module message. */
		static const int MESSAGE_OP_ADD_PROCESSING_MODULE = 1;

		/** \brief Code of a remove-module message. */
		static const int MESSAGE_OP_REMOVE_PROCESSING_MODULE = 2;

		/** \brief Code of an add-module confirmation message. */
		static const int MESSAGE_OP_ADD_PROCESSING_MODULE_ACK = 3;

		/** \brief Code of a remove-module confirmation message. */
		static const int MESSAGE_OP_REMOVE_PROCESSING_MODULE_ACK = 4;

		/** \brief Code of an add-module error message. */
		static const int MESSAGE_OP_ADD_PROCESSING_MODULE_ERROR = 5;

		/** \brief Code of an init processing module message. */
		static const int MESSAGE_OP_INIT_PROCESSING_MODULE = 6;

		/** \brief Code of an open-port message. */
		static const int MESSAGE_OP_PORT_NAME = 7;

		/** \brief Code of a shutdown message. */
		static const int MESSAGE_OP_SHUTDOWN = 8;

		/** \brief Code of a shutdown ack message. */
		static const int MESSAGE_OP_SHUTDOWN_ACK = 9;

		/** \brief Code of a remove-module error message. */
		static const int MESSAGE_OP_REMOVE_PROCESSING_MODULE_ERROR = 10;

		/** \brief Code of a processing module query message. */
		static const int MESSAGE_OP_PROCESSING_MODULE_RUNNING_QUERY = 11;

		/**\brief Code of a processing module query ack message. */
		static const int MESSAGE_OP_PROCESSING_MODULE_RUNNING_ACK = 12;

		/** \brief Code of a query for a processing module consumers. */
		static const int MESSAGE_OP_QUERY_PROCESSING_MODULE_CONSUMERS = 13;

		/** \brief Code of a query for a processing module producers. */
		static const int MESSAGE_OP_QUERY_PROCESSING_MODULE_PRODUCERS = 14;

		/** \brief Code of a query (made by another processing module) for processing module ports. */
		static const int MESSAGE_OP_PROCESSING_MODULE_PORTS_QUERY = 15;

		/** \brief Coed of processing module ports query ack message. */
		static const int MESSAGE_OP_RUNTIME_QUERY_PROCESSING_MODULE_PORTS_ACK = 17;

		/** \brief Code of a producer processing module presentation. */
		static const int MESSAGE_OP_PRODUCER_PROCESSING_MODULE_PRESENTATION = 18;

		/** \brief Code of a consumer processing module presentation. */
		static const int MESSAGE_OP_CONSUMER_PROCESSING_MODULE_PRESENTATION = 19;

		/** \brief Code of a data message. */
		static const int MESSAGE_OP_PROCESSING_MODULE_DATA = 20;

		/** \brief Code of disconnect operation. */
		static const int MESSAGE_OP_DISCONNECT = 21;

		/** \brief Code of module termination. */
		static const int MESSAGE_OP_TERMINATION = 22;

		/** \brief Code of a xml parser error. */
		static const int MESSAGE_OP_PARSER_ERROR = 23;

		/** \brief Code of an information log message. */
		static const int MESSAGE_OP_INFO_LOG = 24;

		/** \brief Code of an error log message. */
		static const int MESSAGE_OP_ERROR_LOG = 25;

		/** \brief Code of an warning log message. */
		static const int MESSAGE_OP_WARNING_LOG = 26;

		/** \brief Code of a credit announcement. */
		static const int MESSAGE_OP_CREDIT_ANNOUNCEMENT = 27;

		/** \brief Code of a message containing information about the database environment. */
		static const int MESSAGE_OP_DB_ENVIRONMENT_DIR = 28;

		/** \brief Code of a message asking for remove some processing module instance. */
		static const int MESSAGE_OP_REMOVE_INSTANCE = 29;

		/** todo */
		static const int MESSAGE_OP_REMOVE_INSTANCE_ACK = 30;

		/** todo */
		static const int MESSAGE_OP_REMOVE_PEER_INSTANCE = 31;

		/** todo */
		static const int MESSAGE_OP_ACCEPT_CONNECT = 32;

		/* ---- XML  ------------------------------------------------------------------------------------------------- */
		static const int START = 0;
		static const int CREATE_TAG = 1;
		static const int CLOSE_TAG = 2;
		static const int CREATE_ATTR = 3;
		static const int CREATE_CHAR_DATA = 4;

		/* ----- Console/runtime commands ---------------------------------------------------------------------------- */

		/** \brief Command add-module. */
		static const string COMMAND_ADD_PROCESSING_MODULE;

		/** \brief Command remove-module. */
		static const string COMMAND_REMOVE_PROCESSING_MODULE;

		/** \brief Command to remove a processing module instance. */
		static const string COMMAND_REMOVE_INSTANCE;

		/** \brief Command system shutdown. */
		static const string COMMAND_SHUTDOWN;

		/* ----- Special message identifications --------------------------------------------------------------------- */

		/** \brief Code of an any source. */
		static const int COMM_ANY_SOURCE = -1;

		/** \brief Identification of the root process. */
		static const int COMM_ROOT_PROCESS = 0;

		/* ----- Communicators' scopes ------------------------------------------------------------------------------- */

		/** \brief Code of an world scope of communication. */
		static const int COMM_SCOPE_WORLD = 0;

		/** \brief Code of a self scope of communication. */
		static const int COMM_SCOPE_SELF = 1;

		/* ----- Runtime files --------------------------------------------------------------------------------------- */

		/** \brief Runtime information file name. */
		static const string FILE_INFO;

		/** \brief Runtime lock file name. */
		static const string FILE_LOCK;

		/** \brief Runtime log file name. */
		static const string FILE_LOG;

		/* ---- Runtime directories ---------------------------------------------------------------------------------- */

		/** \brief Runtime executable directory. */
		static const string DIR_BIN;

		/* ---- Processing Module constants -------------------------------------------------------------------------- */

		/** \brief Automatic number of instances' code. */
		static const int PROCESSING_MODULE_AUTOMATIC_NUMBER_INSTANCES = -1;

		/** todo */
		static const int PROCESSING_MODULE_INVALID_INSTANCE = -1;

		/** \brief Consumer processing module identification. */
		static const string PROCESSING_MODULE_CONSUMER;

		/** \brief Producer processing module identification. */
		static const string PROCESSING_MODULE_PRODUCER;

		/** \brief Batch processing module identification. */
		static const string PROCESSING_MODULE_BATCH;

		/** \brief Stream processing module identification. */
		static const string PROCESSING_MODULE_STREAM;

		/** \brief Default value of an empty attribute. */
		static const string EMPTY_ATTRIBUTE;

		/** \brief Broadcast policy identification. */
		static const string POLICY_BROADCAST;

		/** \brief Round Robin policy identification. */
		static const string POLICY_ROUND_ROBIN;

		/** \brief Labeled stream policy identification. */
		static const string POLICY_LABELED;

		/** \brief Credit shared by all producer instances */
		static const int SHARED_CREDIT = 100;

		/* ---- Persistence module ----------------------------------------------------------------------------------- */

		/** todo */
		static const string CONTAINER_NAME;

		/** todo */
		static const string URI;

		/** todo */
		static const string ATTR_NAME;

		/* ---- Schedule module ----------------------------------------------------------------------------------- */

		static const int SCHED_OP_MIGRATION = 0;

	protected:

	private:
};

#endif /* WATERSHED_COMMON_CONSTANTS_H_ */
