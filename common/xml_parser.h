/**
 * \file common/xml_parser.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMMON_XML_PARSER_H_
#define WATERSHED_COMMON_XML_PARSER_H_

/* C libraries */
#include <sys/stat.h>
#include <errno.h>

/* C++ libraries */
#include <sstream>
#include <stdexcept>
#include <vector>

/* Other libraries */
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

/* Project's .h */
#include "common/parser_error_handler.h"

using namespace xercesc;
using namespace std;

/**
 * \class XMLParser
 * \brief Implementation of an XML file parser using Xerces-C++
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class XMLParser {
	public:

		/**
		 * \brief XMLParser constructor. Creates a new instance of a parser.
		 * \return Not applicable.
		 */
		XMLParser(void) throw(XercesManipulationException);

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		~XMLParser(void) throw(XercesManipulationException);

		/**
		 * \brief Defines the current element based on its name and its ID between all the elements with this name.
		 * \param element_id Identification number of the current element between all the elements with the same name.
		 * \param element_name Name of the current element.
		 * \return The number of elements with the given name.
		 */
		int DefineCurrentElementByName(int element_id, string element_name);

		/**
		 * \brief Gets the current element child based on its name and its ID between all the children with this name.
		 * \param child_id Identification number of the child between all the children with the same name.
		 * \param child_name Name of the child.
		 * \return The number of current element children with the given name.
		 */
		int GetChildByName(int child_id, string child_name);

		/**
		 * \brief Gets the value of the current element attribute with the given name.
		 * \param attribute_name Name of the attribute.
		 * \return The value of the current element attribute with the given name.
		 */
		string GetAttributeByName(string attribute_name);

		/**
		 * \brief Clean the vector of the current element children.
		 */
		void CleanChildren(void);

		/**
		 * \brief Parses the XML file.
		 * \param parse_file XML file name.
		 * \return Not Applicable
		 */
		void Parse(string& parse_file) throw(FileOperationException, XMLParserException);

	protected:

	private:

		/** \brief The current element of the parser. */
		DOMElement* current_element_;

		/** \brief The root element of the parser. */
		DOMElement* root_element_;

		/** \brief The current node of the Parser DOM tree. */
		DOMNode* current_node_;

		/** \brief A list of elements with the same name. */
		DOMNodeList* elements_list_;

		/** \brief A list of some element children. */
		vector< DOMElement* > element_children_;

		/** \brief The Xerces XML Parser. */
		XercesDOMParser *xml_file_parser_;
};

#endif  // WATERSHED_COMMON_XML_PARSER_H_
