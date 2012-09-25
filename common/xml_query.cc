/**
 * \file common/xml_query.cc
 * \author Ana Paula de Carvalho
 */

/* Project's .h */
#include "common/xml_query.h"

XMLQuery::XMLQuery(void) {
}

XMLQuery::~XMLQuery(void) {
}

string XMLQuery::ExecuteQuery(string xml_file, string query_flow_in) {
	XQilla xqilla;
	string value = "";
	try {
		AutoDelete < XQQuery > query(xqilla.parse(X(query_flow_in.c_str())));
		AutoDelete < DynamicContext > context(XQilla::createContext());
		DocumentCache *cache = (DocumentCache*) context->getDocumentCache();
		MemBufInputSource xml((const XMLByte*)  xml_file.c_str(), xml_file.length(), "input");
		Node::Ptr node = cache->parseDocument(xml, context);

		if (node && node->isNode()) {
			context->setContextItem(node);
			context->setContextPosition(1);
			context->setContextSize(1);
		}
		Result result = query->execute(context);
		Item::Ptr item;
		while (item = result->next(context)) {
			value.append(UTF8(item->asString(context)));
			value.append(" ");
		}
	} catch (XMLParseException e) {

	}

	return value;
}

