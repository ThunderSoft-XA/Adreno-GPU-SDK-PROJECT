/*
 * lb_types.h
 *
 *  Created on: Dec 30, 2019
 *      Author: little
 */
#ifndef LIB_INCLUDE_LB_TYPES_H_
#define LIB_INCLUDE_LB_TYPES_H_
#include <assert.h>

#include "lb_common.h"




namespace little
{



uint64_t GetRawTime(); //Seconds
double   GetTime();    //Microseconds



template <class T>
class Matrix
{
public:
	Matrix(uint32_t num_row, uint32_t num_col)	:
		num_of_row(num_row),
		num_of_col(num_col),
		data(0)
	{
		FOOTPRINT();

		if(this->num_of_row > 0 && this->num_of_col > 0)
			data = new T[this->num_of_row * this->num_of_col]();
	}

	~Matrix()
	{
		FOOTPRINT();
		SAFE_DEL_ARRAY(data);
	}

public:
	// Indexing (parenthesis operator)
	const T& operator () (uint32_t nr, uint32_t nc) const
	{
		return this->data[ nr * this->num_of_col + nc ];
	}

	T& operator () (uint32_t nr, uint32_t nc)
	{
		return this->data[ nr * this->num_of_col + nc ];
	}

public:
	// Get dimensions
	uint32_t NumOfRow() const { return this->num_of_row; }
	uint32_t NumOfCol() const { return this->num_of_col; }

	void Reset(void)
	{
		for (int i = 0; i < this->num_of_row * this->num_of_col; i++) this->data[i] = 0;
	}

private:
	uint32_t num_of_row;
	uint32_t num_of_col;

	T* data;

private:
	// To prevent unwanted copying
	Matrix(const Matrix<T> & m);
	Matrix & operator = (const Matrix<T> & m);
};



template <class T>
class Twins
{
public:
	Twins() :
		value(0),
		original(0)
	{

	}

public:
	T operator = (T v)
	{
		this->value = this->original = v;

		return this->value;
	}

	operator T()
	{
		return this->value;
	}

	T operator += (T v)
	{
		this->value += v;

		return this->value;
	}

	T operator -= (T v)
	{
		this->value -= v;

		return this->value;
	}

	T operator *= (T v)
	{
		this->value *= v;

		return this->value;
	}

	T operator /= (T v)
	{
		this->value /= v;

		return this->value;
	}

public:
	void Reset(void)
	{
		this->value = this->original;
	}

protected:
	T value;
	T original;
};



template <class T>
void Swap(T & a, T & b)
{
	T t = a;

	a = b;
	b = t;
}



template <class T>
class Array
{
public:
	Array() :
		data(0),
		length(0),
		releasable(false)
	{
//		FOOTPRINT();
//		LOG_INFO("This:", this);
	}

	Array(T * data, uint32_t length) :
		data(data),
		length(length),
		releasable(false)
	{
//		FOOTPRINT();
//		LOG_INFO("This:", this);
	}

	~Array()
	{
//		FOOTPRINT();
//		LOG_INFO("This:", this);

		this->Release();
	}

	typedef std::function<bool (const T &, const T &)> Compare;

public:
	bool operator == (const Array<T> & other) const;

	operator uint32_t() const { return  this->Length(); }
	operator bool()     const { return  this->data;     }
	operator T&()       const { return *this->data;     }
	operator T*()       const { return  this->data;     }

	/*
	 * DO NOT call operator [] by a pointer of T: this[i] or p[i]!
	 * (*this)[i];
	 * this->operator [](i);
	 * this->data[i];
	 */
	T const & operator[] (uint32_t i) const { return this->data[i]; }
	T       & operator[] (uint32_t i)       { return this->data[i]; }

public:
	uint32_t Length(void) const { return (bool)this ? this->length : 0; }

	bool Create(uint32_t length, T * init_data = 0);
	void Release(void);

	Array<T> SubArray(uint32_t start, uint32_t length);

	int Sort(Compare compare);

protected:
	T        * data;
	uint32_t   length;
	bool       releasable;
};

/*
 * C++03 standard, § 14.7.2.4:
 * The definition of a non-exported function template, a non-exported member function template,
 * or a non-exported member function or static data member of a class template shall be present
 * in every translation unit in which it is explicitly instantiated.
 */

template <class T>
bool Array<T>::operator == (const Array<T> & other) const
{
	if (this->Length() != other.Length()) return false;

	for (int i = 0; i < this->length; i++)
	{
		if (this[i] != other[i]) return false;
	}

	return true;
}

template <class T>
bool Array<T>::Create(uint32_t length, T * init_data)
{
	this->Release();
	this->length = length;

	this->data = this->length > 0 ? new T[this->length]() : 0;
	if (this->length > 0 && 0 == this->data)
	{
		LOG_ERROR("Out of memory!", "");

		this->length = 0;

		return false;
	}

	this->releasable = true;

	if (!init_data) return true;

	for (uint32_t i = 0; i < this->length; i++)
	{
		this->data[i] = init_data[i];
	}

	return true;
}

template <class T>
void Array<T>::Release()
{
	if (this->releasable)
	{
		FOOTPRINT();
		SAFE_DEL_ARRAY(this->data);
		this->length = 0;
	}
}

template <class T>
Array<T> Array<T>::SubArray(uint32_t start, uint32_t num)
{
	T * tmp = 0;

	if (start + num > this->Length())
	{
		LOG_ERROR("Invalid Parameters!", "");
		LOG_ERROR("Start:", start);
		LOG_ERROR("Length:", num);
		LOG_ERROR("ArrayLength:", this->Length());
	}
	else
	{
		tmp = this->data + start;
	}

	if (!tmp)
	{
		num = 0;
	}

	return Array<T>(tmp, num);
}

template <class T>
int Array<T>::Sort(Compare compare)
{
	if (!compare) return 0;

	int swap_count = 0;

	auto Partition = [&](int left, int right)
	{
		if (left >= right)
		{
			PRINT_SEPARATOR();
			LOG_INFO("InvalidIndex", "");
			LOG_INFO("Start:", left);
			LOG_INFO("End:", right);
			PRINT_SEPARATOR();

			return 0;
		}

//		PRINT_SEPARATOR();
//		LOG_INFO("Start:", left);
//		LOG_INFO("End:", right);
//		PRINT_SEPARATOR();

		T   & pivot = (*this)[right];
		int   index = left;

		for (int i = left; i < right; i++)
		{
			if (compare((*this)[i], pivot))
			{
				if (index != i)
				{
					Swap<T>((*this)[i], (*this)[index]);
					swap_count++;
				}

				index++;
			}
		}

		if (index != right)
		{
			Swap((*this)[index], pivot);
			swap_count++;
		}

		return index;
	};

	std::function<void (int, int)> QuickSort = [&](int left, int right) -> void
	{
		if (left < right)
		{
			int pivot_index = Partition(left, right);

			QuickSort(left,            pivot_index - 1);
			QuickSort(pivot_index + 1, right);
		}
	};

	double start = GetTime(), end;

	QuickSort(0, this->length - 1);

	end = GetTime() - start;

#ifndef NDEBUG
	static uint32_t _count = 0;

	if (++_count % 60 == 0)
	{
		_count = 0;

		PRINT_SEPARATOR();
		LOG_INFO("ArrayLength:", this->length);
		LOG_INFO("QuickSortElapsedTime:", end);
		LOG_INFO("SwapCount:", swap_count);
		PRINT_SEPARATOR();
	}
#endif //NDEBUG

	return swap_count;
}



template <class T>
class IndexArray : public Array<T>
{

};



class IntIndexArray : public IndexArray<int>
{
public:
	bool Create(uint32_t length, int * init_data = 0);
	void Reset(bool fill_series = false);
};

inline bool IntIndexArray::Create(uint32_t length, int * init_data)
{
	bool status = IndexArray<int>::Create(length, init_data);

	if (!status)   return false;
	if (init_data) return true;

	this->Reset();

	return true;
}

inline void IntIndexArray::Reset(bool fill_series)
{
	if (this->Length() == 0 || !this->data)
	{
		LOG_ERROR("Invalid IntIndexArray Object:", this);

		return;
	}

	for (uint32_t i = 0; i < this->Length(); i++)
	{
		this->data[i] = fill_series ? i : 0;
	}
}



template <class T>
#if 1
class PointerArray : public Array<T*>
{
public:
	PointerArray()
	{

	}
};
#else
class PointerArray
{
public:
	PointerArray() :
		data(0),
		length(0)
	{

	}

	~PointerArray()
	{
		this->Release();
	}

public:
	operator uint32_t() const { return  this->length; }
	operator bool()     const { return  this->data;  }
	operator T*&()      const { return *this->data;  }

	T* const & operator[] (uint32_t i) const { return this->data[i]; }
	T*       & operator[] (uint32_t i)       { return this->data[i]; }

public:
	bool Create(uint32_t length)
	{
		this->Release();
		this->length = length;

		return this->data = this->length > 0 ? new T*[this->length]() : 0;
	}

	void Release(void)
	{
		SAFE_DEL_ARRAY(this->data);
		this->length = 0;
	}

protected:
	T        ** data;
	uint32_t    length;
};
#endif



} // namespace little



#endif /* LIB_INCLUDE_LB_TYPES_H_ */
