/**
 * \file common/util.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_UTIL_H_
#define WATERSHED_COMMON_UTIL_H_

/* C++ libraries */
#include <string>
#include <vector>

/** Project's .h */
#include "comm/mpi/mpi_communicator.h"

using namespace std;

/**
 * \class Util
 * \brief Implementation of a utility class.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Util {

	public:

		/**
		 * \brief Default constructor.
		 * \return Not applicable.
		 */
		Util ( void );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Util ();

		/**
		 * \brief Converts a float number to a string.
		 * \param number The number to be converted.
		 * \return The string representing the number.
		 */
		static string FloatToString ( float number );

		/**
		 * \brief Converts an integer number to a string.
		 * \param number The number to be converted.
		 * \return The string representing the number.
		 */
		static string IntegerToString ( int number );

		/**
		 * \brief Transforms a string in a vector of tokens according to delimiters.
		 * \param delimiters Delimiters used to split the string.
		 * \param word Word to be split.
		 * \return A vector containing the tokens.
		 */
		static vector < string > TokenizeString ( const string &delimiters, const string &word );

		/**
		 * \brief Sends an error message to the root process in a communicator.
		 * \param communicator Communicator to be used.
		 * \param message The message to be sent.
		 * \return Not applicable.
		 */
		static void Error ( MpiCommunicator* communicator, string message );

		/**
		 * \brief Sends an information message to the root process in a communicator.
		 * \param communicator Communicator to be used.
		 * \param message The message to be sent.
		 * \return Not applicable.
		 */
		static void Information ( MpiCommunicator* communicator, string message );

		/**
		 * \brief Sends an warning message to the root process in a communicator.
		 * \param communicator Communicator to be used.
		 * \param message The message to be sent.
		 * \return Not applicable.
		 */
		static void Warning ( MpiCommunicator* communicator, string message );

	protected:

	private:

		/** \brief The maximun size for a number representation. */
		static const int MAX_NUMBER_REPRESENTATION_SIZE = 100;
};

#endif /* WATERSHED_COMMON_UTIL_H_ */
