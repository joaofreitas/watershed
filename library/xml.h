/*
 * XML.h
 *
 *  Created on: Aug 29, 2011
 *      Author: anapc
 */

#ifndef WATERSHED_LIBRARY_XML_H_
#define WATERSHED_LIBRARY_XML_H_

#include "common/constants.h"
#include "common/xml_query.h"

using namespace std;

class XML {

public:

	/**
	 * todo
	 */
	XML(void);

	/**
	 * todo
	 */
	~XML(void);
	/**
	 * todo
	 */
	void CreateTag(string name);

	/**
	 * todo
	 */
	void CloseTag(string name);

	/**
	 * todo
	 */
	void CreateAttr(string name, string value);

	/**
	 * todo
	 */
	void CreateCharData(string value);

	/**
	 * todo
	 */
	string GetValue(string docXml, string XPath);

	/**
	 * todo
	 */
	string GetDocXML(void);

protected:

private:
	/** todo */
	string doc_XML_;

	/** todo */
	int last_element_;

};

#endif // WATERSHED_LIBRARY_XML_H
