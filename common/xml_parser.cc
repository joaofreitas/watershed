/**
 * \file common/xml_parser.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "common/xml_parser.h"

XMLParser::XMLParser ( void ) throw ( XercesManipulationException ) {
	// Initialize Xerces infrastructure
	try {
		XMLPlatformUtils::Initialize ();
	}
	catch ( XMLException& e ) {
		string err_message = "XML toolkit initilization error: ";
		err_message.append ( XMLString::transcode ( e.getMessage () ) );
		throw XercesManipulationException ( err_message );
	}

	xml_file_parser_ = new XercesDOMParser ();
}

XMLParser::~XMLParser ( void ) throw ( XercesManipulationException ) {
	/* Free memory */
	delete xml_file_parser_;

	/* Terminate Xerces after release memory */
	try {
		XMLPlatformUtils::Terminate ();
	}
	catch ( XMLException& e ) {
		string err_message = "XML toolkit teardown error: ";
		err_message.append ( XMLString::transcode ( e.getMessage () ) );
		throw XercesManipulationException ( err_message );
	}
}

void XMLParser::CleanChildren ( void ) {
	element_children_.clear ();
}

int XMLParser::DefineCurrentElementByName ( int element_id, string element_name ) {
	const XMLCh* tag_name = XMLString::transcode ( element_name.c_str () );

	/* Get all the elements with the name 'tag_name' */
	elements_list_ = root_element_->getElementsByTagName ( tag_name );

	/* Get the number of elements with the name 'tag_name' */
	int number_elements = elements_list_->getLength ();

	if ( number_elements != 0 ) {
		/* Define the current element based on the 'element_id' */
		current_node_ = elements_list_->item ( element_id );
		if ( !current_node_->getNodeType () && current_node_->getNodeType () != DOMNode::ELEMENT_NODE ) {
			return 0;
		}
		current_element_ = dynamic_cast < DOMElement* > ( current_node_ );
	}

	return number_elements;
}

string XMLParser::GetAttributeByName ( string attribute_name ) {
	const XMLCh* attr_name = XMLString::transcode ( attribute_name.c_str () );
	const XMLCh* attr_value = current_element_->getAttribute ( attr_name );
	string value;
	value.assign ( XMLString::transcode ( attr_value ) );
	return value;
}

int XMLParser::GetChildByName ( int child_id, string child_name ) {
	const XMLCh* tag_name = XMLString::transcode ( child_name.c_str () );

	/* Get the current element children */
	DOMNodeList* children = current_element_->getChildNodes ();

	/* Get the current element children with the name 'child_name' */
	for ( XMLSize_t i = 0; i < children->getLength (); i++ ) {
		DOMNode* current_child = children->item ( i );
		if ( !current_child->getNodeType () ) {
			return 0;
		}
		else if ( current_child->getNodeType () == DOMNode::ELEMENT_NODE ) {
			DOMElement* child_element = dynamic_cast < DOMElement* > ( current_child );

			if ( XMLString::equals ( child_element->getTagName (), tag_name ) ) {
				element_children_.push_back ( child_element );
			}
		}
	}

	/* Get the number of current element children with name 'child_name' */
	int number_childs = element_children_.size ();

	/* Set the current element as one of these children based on the 'child_id' */
	current_element_ = element_children_.at ( child_id );

	return number_childs;
}

void XMLParser::Parse ( string& parse_file ) throw ( FileOperationException, XMLParserException ) {

	/* Test if the parse file is OK */
	struct stat fileStatus;
	int iretStat = stat ( parse_file.c_str (), &fileStatus );

	if ( iretStat == ENOENT ) {
		throw FileOperationException ( "Path file_name does not exist, or path is an empty string." );
	}
	else if ( iretStat == ENOTDIR ) {
		throw FileOperationException ( "A component of the path is not a directory." );
	}
	else if ( iretStat == ELOOP ) {
		throw FileOperationException ( "Too many symbolic links encountered while traversing the path." );
	}
	else if ( iretStat == EACCES ) {
		throw FileOperationException ( "Permission denied." );
	}
	else if ( iretStat == ENAMETOOLONG ) {
		throw FileOperationException ( "File can not be read\n" );
	}

	/* Configure DOM parser and set validation*/
	ParserErrorHandler handler;
	xml_file_parser_->setErrorHandler ( &handler );
	xml_file_parser_->setValidationScheme ( XercesDOMParser::Val_Always );

	try {
		/* Parsing */
		xml_file_parser_->parse ( parse_file.c_str () );

		/* Verify if the parse was succeeded */
		/* No need to free this pointer(owned by the parent parser object) */
		DOMDocument* xmlDoc = xml_file_parser_->getDocument ();
		root_element_ = xmlDoc->getDocumentElement ();
		if ( !root_element_ ) {
			throw XMLParserException ( "empty XML document" );
		}
	}
	catch ( const SAXException& e ) {
		string message;
		message.assign ( XMLString::transcode ( e.getMessage () ) );
		throw XMLParserException ( message );
	}
	catch ( const XMLException& e ) {
		string message;
		message.assign ( XMLString::transcode ( e.getMessage () ) );
		throw XMLParserException ( message );
	}
	catch ( const exception& e ) {
		string message = "error when trying to parse the file " + parse_file + ". One possible reason is that the DTD validation file was not found.";
		throw XMLParserException ( message );
	}
}
