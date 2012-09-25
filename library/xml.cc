/*
 * XML.cpp
 *
 *  Created on: Aug 29, 2011
 *      Author: anapc
 */

#include "library/xml.h"

XML::XML ( void ) {
	doc_XML_ = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	last_element_ = Constants::START;
}

XML::~XML ( void ) {
}

string XML::GetDocXML () {
	return doc_XML_;
}

void XML::CreateTag ( string name ) {
	if ( last_element_ == Constants::CREATE_ATTR || last_element_ == Constants::CREATE_TAG ) {
		doc_XML_.append ( ">" );
	}
	doc_XML_.append ( " <" + name );
	last_element_ = Constants::CREATE_TAG;
}

void XML::CloseTag ( string name ) {
	if ( last_element_ == Constants::CREATE_ATTR ) {
		doc_XML_.append ( "/>" );
	}
	else {
		doc_XML_.append ( " </" + name + ">" );
	}
	last_element_ = Constants::CLOSE_TAG;
}

void XML::CreateAttr ( string name, string value ) {
	doc_XML_.append ( " " + name + "=\"" + value + "\"" );
	last_element_ = Constants::CREATE_ATTR;
}

void XML::CreateCharData ( string value ) {
	if ( last_element_ == Constants::CREATE_ATTR || last_element_ == Constants::CREATE_TAG ) {
		doc_XML_.append ( ">" );
	}
	doc_XML_.append ( " " + value );
	last_element_ = Constants::CREATE_CHAR_DATA;
}

string XML::GetValue ( string doc_xml, string XPath ) {
	XMLQuery xml_query;
	string result = xml_query.ExecuteQuery ( doc_xml, XPath );
	return result;
}
