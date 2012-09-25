/**
 * \file common/exceptions.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_EXCEPTIONS_H_
#define WATERSHED_COMMON_EXCEPTIONS_H_

//#define DEBUG

/* C++ libraries */
#include <iostream>
#include <exception>
#include <string>

using namespace std;

/**
 * \class BadParameterException
 * \brief Implementation of an exception related to parameter not correct.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class BadParameterException: public exception {

	public:

		BadParameterException(string message) {message_="BadParameterException: "; message_.append(message);}
		virtual ~BadParameterException() throw() {};
		string ToString() {return message_;}
		virtual const char* what() const throw() {return "";}

	protected:

	private:

		string message_;
};

/**
 * \class FileOperationException
 * \brief Implementation of an exception related to file operation.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class FileOperationException: public exception {

	public:

		FileOperationException(string message) {message_="FileOperationException: "; message_.append(message);}
		virtual ~FileOperationException() throw() {};
		string ToString() {return message_;}
		virtual const char* what(void) const throw() {return "";}

	protected:

	private:

		string message_;
};

/**
 * \class MamoryOperationException
 * \brief Implementation of an exception related to memory allocation.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class MemoryOperationException: public exception {

	virtual const char* what() const throw() {
		return "";
	}

	private:
		string message_;
	public:
		MemoryOperationException(string message) {message_="MemoryOperationException: "; message_.append(message);}
		virtual ~MemoryOperationException() throw() {};
		string ToString() {return message_;}
};

/**
 * \class ProcessRemovingException
 * \brief Implementation of an exception related to process removing.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ProcessRemovingException: public exception {

	public:
		ProcessRemovingException(string message) {message_="ProcessRemovingException: "; message_.append(message);}
		virtual ~ProcessRemovingException() throw() {};
		string ToString() {return message_;}
		virtual const char* what() const throw() {return "";}

	protected:

	private:
		string message_;
};

/**
 * \class ProcessSpawnningException
 * \brief Implementation of an exception related to process spawning.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ProcessSpawnningException: public exception {

	public:
		ProcessSpawnningException(string message) {message_="ProcessSpawnningException: "; message_.append(message);}
		virtual ~ProcessSpawnningException() throw() {};
		string ToString() {return message_;}
		virtual const char* what() const throw() {return "";}

	protected:

	private:
		string message_;
};

/**
 * \class XercesManipulationException
 * \brief Implementation of an exception related to the Xerces manipulation.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class XercesManipulationException: public exception {

	virtual const char* what() const throw() {
		return "";
	}

	private:
		string message_;
	public:
		XercesManipulationException(string message) {message_="Xerces Manipulation Error: "; message_.append(message);}
		virtual ~XercesManipulationException() throw() {};
		string ToString() {return message_;}
};

/**
 * \class XMLParserException
 * \brief Implementation of an exception related to the XML parser.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class XMLParserException: public exception {

	virtual const char* what() const throw() {
		return "";
	}

	private:
		string message_;
	public:
		XMLParserException(string message) {message_="XMLParserException: "; message_.append(message);}
		virtual ~XMLParserException() throw() {};
		string ToString() {return message_;}
};

/**
 * \class SymbolNotFoundException
 * \brief Implementation of an exception related to the XML parser.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class SymbolNotFoundException: public exception {

	virtual const char* what() const throw() {
		return "";
	}

	private:
		string message_;
	public:
		SymbolNotFoundException(string message) {message_="SymbolNotFoundException: "; message_.append(message);}
		virtual ~SymbolNotFoundException() throw() {};
		string ToString() {return message_;}
};

#endif /* WATERSHED_COMMON_EXCEPTIONS_H_ */
