//RString.h
#ifndef RSTRING_H
#define RSTRING_H
#include "RTypesTypes.h"

namespace rtypes
{
	template<class CharType>
	class rtype_string
	{
	public:
		rtype_string()
		{
			_data = 0;
			_sz = 0;
			_extr = 0;
			_virtAlloc(1);
			_nullTerm();
		}
		rtype_string(const CharType* cStr)
		{
			_data = 0;
			_sz = 0;
			_extr = 0;
			_copy(cStr);
		}
		rtype_string(const rtype_string& sObj)
		{
			_data = 0;
			_sz = 0;
			_extr = 0;
			_copy(sObj.c_str(),sObj.size());
		}
		explicit rtype_string(dword AllocSize/* this is the desired string capacity */)
		{
			// account for null terminator
			//AllocSize++;
			//
			_data = new CharType[AllocSize];
			_sz = AllocSize;
			_extr = 0;
			_nullTerm(); // happy to add null terminator for the user, they just had to account for it
		}
		~rtype_string()
		{
			_dealloc();
		}
		// operators
		CharType& operator [](dword i) { return _data[i]; }
		const CharType& operator [](dword i) const { return _data[i]; }
		rtype_string& operator =(CharType c)
		{
			_virtAlloc(2);
			_data[0] = c;
			_nullTerm();
			return *this;
		}
		rtype_string& operator =(const CharType* cStr)
		{
			_copy(cStr);
			return *this;
		}
		rtype_string& operator =(const rtype_string& sObj)
		{
			_copy(sObj.c_str(),sObj.size());
			return *this;
		}
		rtype_string& operator +=(CharType c)
		{
			push_back(c);
			return *this;
		}
		rtype_string& operator +=(const CharType* cStr)
		{
			dword len = 0, stringEnd = size();
			while (cStr[len])
				len++;
			_virtAlloc(_sz+len);
			for (dword i = 0;i<len;i++)
				_data[i+stringEnd] = cStr[i];
			_nullTerm();
			return *this;
		}
		rtype_string& operator +=(const rtype_string& sObj)
		{
			dword stringEnd = size();
			_virtAlloc(_sz+sObj.length());
			for (dword i = 0;i<sObj.size();i++)
				_data[i+stringEnd] = sObj[i];
			_nullTerm();
			return *this;
		}
		// operations				
		void push_back(CharType t)
		{
			_virtAlloc( _sz + (_sz==0 ? 2 /* add space for null term for user */ : 1) );
			_data[_sz-2] = t;
			_nullTerm();
		}
		void clear() // reduce the logical allocation size
		{
			_virtAlloc(1);
			_nullTerm();
		}
		void reset() // reduce allocation size
		{
			_dealloc();
			_virtAlloc(1); // still make this a null string
			_nullTerm();
		}
		bool truncate(dword Size)
		{
			if (Size>=size())
				return false;
			_virtAlloc(Size+1);
			_nullTerm();
			return true;
		}
		void resize(dword NewSize)
		{
			if (NewSize!=size())
			{
				_virtAlloc(NewSize+1);
				_nullTerm();
			}
		}
		// string info
		const CharType* c_str() const { return _data; }
		dword size() const { return _sz-1; }
		dword length() const { return _sz-1; }
		dword capacity() const { return allocation_size()-1; }
		dword allocation_size() const { return _sz+_extr; }
	protected:
		void _virtAlloc(dword desiredSize)
		{
			if (desiredSize>_sz)
			{
				// an allocation is needed
				if (allocation_size()>=desiredSize)
				{
					// virtual allocation
					while (_extr>0 && _sz<desiredSize)
					{
						_extr--;
						_sz++;
					}
				}
				else
				{
					// reallocate to twice old allocation
					dword allocSize = allocation_size();
					dword newSize = (allocSize==0 ? 2 : allocSize*2);
					CharType* newData = new CharType[newSize];
					for (dword i = 0;i<_sz;i++)
						newData[i] = _data[i];
					delete[] _data;
					_data = newData;
					_extr = newSize-_sz;
					_virtAlloc(desiredSize);
				}
			}
			else if (desiredSize<_sz)
			{
				// virtual deallocation
				dword dif = _sz-desiredSize;
				_extr += dif;
				_sz -= dif;
			}
			// else equal, no action
		}
		void _nullTerm(CharType nullChar = CharType())
		{
			_data[_sz-1] = nullChar; // _sz will always be greater than or equal to 1
		}
	private:
		CharType* _data;
		dword _sz, _extr;
		void _dealloc()
		{
			delete[] _data;
			_data = 0;
			_sz = 0;
			_extr = 0;
		}
		void _copy(const CharType* p)
		{
			dword len = 0;
			while (p[len] != 0)
				len++;
			_virtAlloc(++len); // account for null terminator and copy it as well
			for (dword i = 0;i<len;i++)
				_data[i] = p[i];
		}
		void _copy(const CharType* p,dword len)
		{
			_virtAlloc(len+1);
			for (dword i = 0;i<len;i++)
				_data[i] = p[i];
			_nullTerm();
		}
	};

	// comparison operator overloads for rtype_string<CharType>
	template<class CharType>
	bool operator ==(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		if (s1.size()==s2.size())
		{
			for (dword i = 0;i<s1.size();i++)
				if (s1[i]!=s2[i])
					return false;
			return true;
		}
		return false;
	}
	template<class CharType>
	bool operator ==(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		dword len = 0;
		while (s2[len])
			len++;
		if (s1.size()==len)
		{
			for (dword i = 0;i<s1.size();i++)
				if (s1[i]!=s2[i])
					return false;
			return true;
		}
		return false;
	}
	template<class CharType>
	bool operator ==(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		dword len = 0;
		while (s1[len])
			len++;
		if (len==s2.size())
		{
			for (dword i = 0;i<len;i++)
				if (s1[i]!=s2[i])
					return false;
			return true;
		}
		return false;
	}
	template<class CharType>
	bool operator !=(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator ==(s1,s2);
	}

	template<class CharType>
	bool operator !=(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		return ! rtypes::operator ==(s1,s2);
	}

	template<class CharType>
	bool operator !=(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator ==(s1,s2);
	}

	template<class CharType>
	bool operator <(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		for (dword i = 0;i<s1.size() && i<s2.size();i++)
			if ( s1[i]<s2[i] )
				return true;
			else if ( s1[i]>s2[i] )
				return false;
		return s1.size()<s2.size();
	}

	template<class CharType>
	bool operator <(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		dword len = 0;
		while (s2[len])
			len++;
		for (dword i = 0;i<s1.size() && i<len;i++)
			if ( s1[i]<s2[i] )
				return true;
			else if ( s1[i]>s2[i] )
				return false;
		return s1.size()<len;
	}

	template<class CharType>
	bool operator <(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		dword len = 0;
		while (s1[len])
			len++;
		for (dword i = 0;i<len && i<s2.size();i++)
			if ( s1[i]<s2[i] )
				return true;
			else if ( s1[i]>s2[i] )
				return false;
		return len<s2.size();
	}

	template<class CharType>
	bool operator >=(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator <(s1,s2);
	}

	template<class CharType>
	bool operator >=(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		return ! rtypes::operator <(s1,s2);
	}

	template<class CharType>
	bool operator >=(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator <(s1,s2);
	}

	template<class CharType>
	bool operator >(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		for (dword i = 0;i<s1.size() && i<s2.size();i++)
			if ( s1[i]>s2[i] )
				return true;
			else if ( s1[i]<s2[i] )
				return false;
		return s1.size()>s2.size();
	}

	template<class CharType>
	bool operator >(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		dword len = 0;
		while (s2[len])
			len++;
		for (dword i = 0;i<len && i<s1.size();i++)
			if ( s1[i]>s2[i] )
				return true;
			else if ( s1[i]<s2[i] )
				return false;
		return s1.size()>len;
	}

	template<class CharType>
	bool operator >(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		dword len = 0;
		while (s1[len])
			len++;
		for (dword i = 0;i<len && i<s2.size();i++)
			if ( s1[i]>s2[i] )
				return true;
			else if ( s1[i]<s2[i] )
				return false;
		return len>s2.size();
	}

	template<class CharType>
	bool operator <=(const rtype_string<CharType>& s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator >(s1,s2);
	}

	template<class CharType>
	bool operator <=(const rtype_string<CharType>& s1,
						const CharType* s2)
	{
		return ! rtypes::operator >(s1,s2);
	}

	template<class CharType>
	bool operator <=(const CharType* s1,
						const rtype_string<CharType>& s2)
	{
		return ! rtypes::operator >(s1,s2);
	}

	//concatenation operators
	template<class CharType>
	const rtype_string<CharType> operator +(rtype_string<CharType> left,
												const rtype_string<CharType>& right)
	{
		left += right;
		return left;
	}

	template<class CharType>
	const rtype_string<CharType> operator +(rtype_string<CharType> left,
												const CharType* right)
	{
		left += right;
		return left;
	}

	template<class CharType>
	const rtype_string<CharType> operator +(const CharType* left,
												const rtype_string<CharType>& right)
	{
		rtype_string<CharType> r(left);
		r += right;
		return r;
	}

	// typedefs for common string types
	typedef rtype_string<char> str;
	typedef rtype_string<wchar_t> wstr;
}

#endif