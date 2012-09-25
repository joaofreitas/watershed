/**
 * \file common/logger.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_LOGGER_H_
#define WATERSHED_COMMON_LOGGER_H_

/* C libraries */
#include <string.h>

/* C++ libraries */
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/**
 * \class Logger
 * \brief Implementation of a logger.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Logger {

	public:

		/**
		 * \brief Constructor. Creates a new instance of Logger.
		 * \param log_file_name Name of the file to use in log operations.
		 * \param screen_as_output Sets the screen to be an output of log messages.
		 * \param screen_separator Text to be printed as section separator on the screen.
		 * \return Not applicable.
		 */
		Logger ( string log_file_name, bool screen_as_output, string screen_separator );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Logger ( void );

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
		 * \brief Prints an information message.
		 * \param message The message to be printed.
		 * \return Not applicable.
		 */
		void PrintInfo ( string message );

		/**
		 * \brief Prints an error message.
		 * \param message The message to be printed.
		 * \return Not applicable.
		 */
		void PrintError ( string message );

		/**
		 * \brief Prints an warning message.
		 * \param message The message to be printed.
		 * \return Not applicable.
		 */
		void PrintWarning ( string message );

		/**
		 * \brief Print a screen separator.
		 * \return Not applicable.
		 */
		void PrintScreenSeparator ( void );

	protected:

	private:

		/**
		 * \brief Print a message in the log file.
		 * \param message Message to be printed.
		 * \return Not applicable.
		 */
		void PrintFile ( string message );

		/**
		 * \brief Print a message in the screen and in the log file.
		 * \param message Message to be printed.
		 * \return Not applicable.
		 */
		void PrintScreen ( string message );

		/** \brief A mutex to control the access to the Logger class. */
		static pthread_mutex_t class_mutex_;

		/** \brief The number of instances of this class. */
		static int number_of_instances_;

		/** \brief Flag used to allow printing to screen. */
		bool screen_as_output_;

		/**\brief System log file. */
		ofstream log_file_;

		/** \brief Error message notification. */
		static const string ERROR_TEXT;

		/** \brief Information message notification. */
		static const string INFO_TEXT;

		/** \brief Warning message notification. */
		static const string WARNING_TEXT;

		/** \brief Section separator to print information. */
		string screen_separator_;
};

#endif /* WATERSHED_COMMON_LOGGER_H_ */
