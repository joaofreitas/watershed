/**
 * \file common/xml_query.h
 * \author Ana Paula de Carvalho
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef XML_QUERY_H_
#define XML_QUERY_H_

#include <iostream>
#include <xqilla/xqilla-simple.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>

using namespace std;
using namespace xercesc;


/**
 * \class XMLQuery
 * \brief Query XML file parser using Xqilla
 * \author Ana Paula de Carvalho
 * \version 1.0
 * \date 2011
 */
class XMLQuery {
	public:

		/**
		 * \brief XMLQuery constructor.
	     * \return Not applicable.
	 	 */
	    XMLQuery();

	    /**
	     * \brief Destructor.
	     * \return Not applicable.
	     */
	   ~XMLQuery(void);

	   /**
	   	* \brief Query a XML file.
	   	* \param xml_file
	   	* \param query_flow_in
	   	* \return
	   	*/
	   string ExecuteQuery(string xml_file, string query_flow_in);

	private:


};

#endif  // CAHPETA_COMMON_XML_QUERY_H_
