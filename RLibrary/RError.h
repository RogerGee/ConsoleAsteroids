//RError.h
// (RType headers that need RError.h will include it themselves)
#ifndef RERROR_H
#define RERROR_H

namespace rtypes
{
	struct rtype_error
	{
		virtual const char* msg() const
		{ return "An error has occurred."; }
		virtual int number() const
		{ return 0; }
	};
	struct out_of_bounds_error : rtype_error
	{
		const char* msg() const
		{ return "The index was not within the bounds of the array."; }
		int number() const
		{ return 1; }
	};
	struct element_not_found_error : rtype_error
	{
		const char* msg() const
		{ return "The specified element was not found."; }
		int number() const
		{ return 2; }
	};
}

#endif