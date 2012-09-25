/**
 * \file common/parser_error_handler.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "common/parser_error_handler.h"

ParserErrorHandler::ParserErrorHandler(void){

}

ParserErrorHandler::~ParserErrorHandler(void){

}

void ParserErrorHandler::error(const SAXParseException& e)
		throw(XMLParserException){
	string message;
	message.assign(XMLString::transcode(e.getMessage()));
	throw XMLParserException(message);
}

void ParserErrorHandler::fatalError(const SAXParseException& e){
	error(e);
}

void ParserErrorHandler::warning(const SAXParseException& e){

}
