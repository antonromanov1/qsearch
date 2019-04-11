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

#include "Document.h"
#include <string>
#include <vector>

void search_for_links(GumboNode* node, std::vector<std::string>& vec)
{
	if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }

	GumboAttribute* href;
	if (node->v.element.tag == GUMBO_TAG_A &&
                                  (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        vec.push_back(href->value);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links(static_cast<GumboNode*>(children->data[i]), vec);
    }
}

CDocument::CDocument()
{
	mpOutput = NULL;
}

void CDocument::parse(std::string aInput)
{
	reset();
	mpOutput = gumbo_parse(aInput.c_str());
}

CDocument::~CDocument()
{
	reset();
}

CSelection CDocument::find(std::string aSelector)
{
	if (mpOutput == NULL)
	{
		throw "document not initialized";
	}

	CSelection sel(mpOutput->root);
	return sel.find(aSelector);
}

std::vector<std::string> CDocument::get_links()
{
	if (mpOutput == NULL)
	{
		throw "document not initialized";
	}

	std::vector<std::string> vec;
	search_for_links(mpOutput->root, vec);
	return vec;
}

std::string str_join(std::vector<std::string>& vec) {
    std::string str;

    for (auto x : vec) {
		/*
        std::string var;

        for (auto y : x)
            var += y;

        str += var;
		*/
		str += x;
    }

    return str;
}

std::string CDocument::page_text()
{
	std::string text;

	const char* tags[] = {"h1", "h2", "h3", "h4", "h5", "h6", "p", "span", "figcaption"};
	for (const char* x : tags) {
	    CSelection selec = this->find(x);
        std::vector<std::string> data_h1(selec.results());
        text += str_join(data_h1);
	}
	return text;
}

void CDocument::reset()
{
	if (mpOutput != NULL)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, mpOutput);
		mpOutput = NULL;
	}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */

