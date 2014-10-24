//RContainer.h
#ifndef RCONTAINER_H
#define RCONTAINER_H
#include "RError.h"

namespace rtypes
{
	template<class T>
	class container // represents a dynamic container
	{
	public:
		container()
		{
			sz = 0;
			data = 0;
			extra = 0;
			alloc_more();
		}
		explicit container(unsigned int ini_alloc)
		{
			extra = ini_alloc;
			sz = 0;
			if (ini_alloc==0)
				data = 0;
			else
				data = new T[ini_alloc];
		}
		container(const container& c)
		{
			if (c.sz==0)
			{
				data = 0;
				sz = 0;
				extra = 0;
				return;
			}
			extra = 0;
			data = new T[c.sz];
			copy(c.data,data,c.sz);
			sz = c.sz;
		}
		~container()
		{
			delete[] data;
		}
		void alloc_size(unsigned int Size)
		{
			delete[] data;
			extra = Size;
			sz = 0;
			if (Size==0)
				data = 0;
			else
				data = new T[Size];
		}
		void push_back(const T& element)
		{
			if (extra<=0)
				alloc_more();
			data[sz] = element;
			--extra;
			++sz;
		}
		void delete_elements() // deletes, then allocates
		{
			delete[] data;
			sz = 0;
			data = 0; // don't forget this -- if not it causes errors
			extra = 0;
			alloc_more();
		}
		void remove_at(unsigned int index) // this doesn't deallocate
		{
			if (index>=sz) return;
			for (unsigned int i = index;i+1<sz;i++)
				data[i] = data[i+1];
			--sz;
			++extra;
		}
		void remove_all() // this doesn't deallocate
		{
			extra += sz;
			sz = 0;
		}
		unsigned int size() const
		{
			return sz;
		}
		T& operator [](unsigned int i)
		{
			if (sz<=i)
				throw out_of_bounds_error();
			return data[i];
		}
		const T& operator [](unsigned int i) const
		{
			if (sz<=i) 
				throw out_of_bounds_error();
			return data[i];
		}
		container& operator=(const container& c)
		{
			if (this==&c)
				return *this; // same object
			if (c.sz==0)
			{
				delete_elements();
				return *this;
			}
			extra = 0;
			T* _new = new T[c.sz];
			copy(c.data,_new,c.sz);
			delete[] data; // DO NOT call delete elements here, it will LEAK MEMORY!!
			sz = c.sz;
			data = _new;
			return *this;
		}
		T& operator ++()
		{
			push_back(T());
			return data[sz-1];
		}
		void insertion_sort()
		{
			for (unsigned int i = 1;i<sz;i++)
			{
				int pos = i; T val = data[i];
				while (pos>0 && val<data[pos-1])
				{
					data[pos] = data[pos-1];
					pos--;
				}
				data[pos] = val;
			}
		}
		bool binary_search(const T& elem,unsigned int& index) const
		{
			if (!sz)
				// no elements to search
				return false;
			int start = 0, end = (int) (sz-1);
			while (start<=end)
			{
				unsigned int mid = (start+end)/2;
				if (data[mid]==elem)
				{
					index = mid;
					return true;
				}
				if (elem<data[mid])
					end = mid-1;
				else
					start = mid+1;
			}
			index = sz;
			return false;
		}
	private:
		void alloc_more()
		{
			unsigned int nsz = sz*2;
			if (nsz==0) nsz = 1;
			T* _new = new T[nsz];
			extra = nsz-sz; // nsz-sz==sz unless sz==0
			for (unsigned int i = 0;i<sz;i++)
				_new[i] = data[i];
			delete[] data;
			data = _new;
		}
		void copy(T* from,T* to,unsigned int _size)
		{// this method assumes 'from' and 'to' point to '_size' number of elements
			for (unsigned int i = 0;i<_size;i++)
				to[i] = from[i];
		}
		unsigned int sz;
		T* data;
		unsigned int extra;
	};

	// comparision operator overloads
	template<class T>
	bool operator ==(const container<T>& c1,
						const container<T>& c2)
	{
		if (c1.size()==c2.size())
		{
			for (unsigned int i = 0;i<c1.size();i++)
				if ( c1[i] != c2[i] )
					return false;
			return true;
		}
		return false;
	}
	template<class T>
	bool operator !=(const container<T>& c1,
						const container<T>& c2)
	{
		return ! rtypes::operator ==(c1,c2);
	}
	template<class T>
	bool operator <(const container<T>& c1,
						const container<T>& c2)
	{
		unsigned int lesserSize = c1.size()<c2.size() ? c1.size() : c2.size();
		for (unsigned int i = 0;i<lesserSize;i++)
			if ( c1[i]<c2[i] )
				return true;
			else if ( c1[1]>c2[i] )
				return false;
		return c1.size()<c2.size();
	}
	template<class T>
	bool operator >=(const container<T>& c1,
						const container<T>& c2)
	{
		return ! rtypes::operator <(c1,c2);
	}
	template<class T>
	bool operator >(const container<T>& c1,
						const container<T>& c2)
	{
		unsigned int lesserSize = c1.size()<c2.size() ? c1.size() : c2.size();
		for (unsigned int i = 0;i<lesserSize;i++)
			if ( c1[i]>c2[i] )
				return true;
			else if ( c1[i]<c2[i] )
				return false;
		return c1.size()>c2.size();
	}
	template<class T>
	bool operator <=(const container<T>& c1,
						const container<T>& c2)
	{
		return ! rtypes::operator >(c1,c2);
	}
}

#endif