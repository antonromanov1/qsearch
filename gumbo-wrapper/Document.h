/***************************************************************************
 * 
 * $Id$
 * 
 **************************************************************************/

/**
 * @file $HeadURL$
 * @author $Author$(hoping@baimashi.com)
 * @date $Date$
 * @version $Revision$
 * @brief 
 *  
 **/

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <gumbo.h>
#include <string>
#include <vector>
#include "Selection.h"

void search_for_links(GumboNode*, std::vector<std::string>&);

class CDocument: public CObject
{
	public:

		CDocument();

		void parse(std::string aInput);

		virtual ~CDocument();

		CSelection find(std::string aSelector);

		std::vector<std::string> get_links() const;

	private:

		void reset();

	private:

		GumboOutput* mpOutput;
};

#endif /* DOCUMENT_H_ */

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
