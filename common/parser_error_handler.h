/**
 * \file common/parser_error_handler.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_PARSER_ERROR_HANDLER_H_
#define WATERSHED_COMMON_PARSER_ERROR_HANDLER_H_

/* Other libraries */
#include <xercesc/sax2/DefaultHandler.hpp>

/* Project's .h */
#include "common/exceptions.h"

using namespace xercesc;

/**
 * \class ParserErrorHandler
 * \brief Implementation of a handler for a xerces parser.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class ParserErrorHandler : public DefaultHandler {

	public:

		/**
		 * \brief Default constructor.
		 * \return Not applicable.
		 */
		ParserErrorHandler();

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		~ParserErrorHandler();

		/**
		 * \brief Handles a parser error.
		 * \param e A parser exception.
		 * \return Not applicable.
		 */
		void error(const SAXParseException& e) throw(XMLParserException);

		/**
		 * \brief Handles a parser fatal error.
		 * \param e A parser exception.
		 * \return Not applicable.
		 */
		void fatalError(const SAXParseException& e);

		/**
		 * \brief Handles a parser warning.
		 * \param e A parser exception.
		 * \return Not applicable.
		 */
		void warning(const SAXParseException& e);

	protected:

	private:
};

#endif /* WATERSHED_COMMON_PARSER_ERROR_HANDLER_H_ */
