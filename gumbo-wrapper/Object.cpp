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

#include <cassert>
#include <stdexcept>
#include "Object.h"

CObject::CObject()
{
	mReferences = 1;
}

CObject::~CObject()
{
	assert(mReferences == 1);

	/*if (mReferences != 1)
	{
		throw std::logic_error("something wrong, reference count not zero");
	}*/
}

void CObject::retain()
{
	mReferences++;
}

void CObject::release()
{
	if (mReferences < 0)
	{
		throw std::logic_error("something wrong, reference count is negative");
	}

	if (mReferences == 1)
	{
		delete this;
	}
	else
	{
		mReferences--;
	}
}

unsigned int CObject::references()
{
	return mReferences;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */

