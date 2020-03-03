#pragma once
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

struct Color
{
	uint8_t r = 255, g = 255, b = 255;

	int sdlLike(struct SDL_Surface const* screen) const;

	static Color const Red;
	static Color const Green;
	static Color const Blue;
	static Color const Black;
	static Color const White;
	static Color const Grey;
};


enum class Direction : uint8_t { Left, Right, Up, Down, Count };



template<class T>
constexpr inline T maxVal(const T & val1, const T & val2)
{
	return val1 > val2 ? val1 : val2;
}

template<class T>
constexpr inline T minVal(const T & val1, const T & val2)
{
	return val1 < val2 ? val1 : val2;
}

template<class T>
inline T clamped(const T & value, const T& min, const T& max)
{
	return minVal(maxVal(value, min), max);
}

template<class T>
inline T& clamp(T& value, const T& min, const T& max)
{
	return value = clamped(value, min, max);
}

template<class T>
inline void swap(T& value1, T& value2)
{
	T temp = value1;
	value1 = value2;
	value2 = temp;
}



template<class T>
class Vector
{
public:

	Vector();

	Vector(Vector const& other);

	Vector(Vector &&) = default;

	Vector<T>& operator=(Vector const& other);

	Vector<T>& operator=(Vector && other) = default;

	~Vector();


	void pushBack(T const& obj);

	void clear(bool memory = false);


	T* getData() const;

	size_t size() const;


	T& operator[](size_t idx);

	T const& operator[](size_t idx) const;


	template<class Cmp_t>
	bool find(T const& other, Cmp_t cmp);


	T& front();

	T const& front() const;


	T& back();

	T const& back() const;


	bool reserve(size_t n);

	void resize(size_t n);

private:

	T* data = nullptr;

	size_t count = 0;

	size_t reserved = 0;

	static size_t const growth = 2;
};

template<class T>
inline Vector<T>::Vector()
{
	reserve(16);
}

template<class T>
inline Vector<T>::Vector(Vector const & other)
{
	*this = other;
}

template<class T>
inline Vector<T>& Vector<T>::operator=(Vector const & other)
{
	resize(other.count);
	for (size_t i = 0; i < other.size(); ++i)
		data[i] = other[i];
	return *this;
}

template<class T>
inline Vector<T>::~Vector()
{
	clear(true);
}

template<class T>
inline void Vector<T>::pushBack(T const & obj)
{
	if (count < reserved || reserve((reserved > 0 ? reserved : 8) * growth))
		data[count++] = obj;
}

template<class T>
inline void Vector<T>::clear(bool memory)
{
	if (memory && data)
	{
		delete[] data;
		data = nullptr;
		reserved = 0;
	}
	count = 0;
}

template<class T>
inline T & Vector<T>::front()
{
	return (*this)[0];
}

template<class T>
inline T const & Vector<T>::front() const
{
	return (*this)[0];
}

template<class T>
inline T & Vector<T>::back()
{
	return (*this)[count - 1];
}

template<class T>
inline T const & Vector<T>::back() const
{
	return (*this)[count - 1];
}

template<class T>
inline bool Vector<T>::reserve(size_t n)
{
	reserved = n;
	T* newData = new T[n];
	n = count < n ? count : n;
	if (newData)
	{
		if (data)
		{
			for (size_t i = 0; i < n; ++i)
				newData[i] = data[i];
			delete[] data;
		}
		data = newData;
		return true;
	}

	printf("Out of memory!");
	exit(EXIT_FAILURE);

	return false;
}

template<class T>
inline void Vector<T>::resize(size_t n)
{
	reserve(n);
	count = n;
}

template<class T>
inline T* Vector<T>::getData() const
{
	return data;
}

template<class T>
inline size_t Vector<T>::size() const
{
	return count;
}

template<class T>
inline T & Vector<T>::operator[](size_t idx)
{
	if (idx >= count)
		printf("out of range idx: %u\n", idx);
	return data[idx];
}

template<class T>
inline T const & Vector<T>::operator[](size_t idx) const
{
	if (idx >= count)
		printf("out of range idx: %u\n", idx);
	return data[idx];
}


//static void merge(char const ** arr, int32_t l, int32_t m, int32_t r)
//{
//	int32_t lLen = m - l + 1;
//	int32_t rLen = r - m;
//	int32_t lIdx = 0, rIdx = 0, oIdx = l; // oIdx - index of arr
//	char const ** lCpy = (char const **)malloc(lLen * sizeof(char*));
//	char const ** rCpy = (char const **)malloc(rLen * sizeof(char*));
//
//
//	for (lIdx = 0; lIdx < lLen; lIdx++)
//		lCpy[lIdx] = arr[l + lIdx];
//	for (rIdx = 0; rIdx < rLen; rIdx++)
//		rCpy[rIdx] = arr[m + rIdx + 1];
//
//	for (oIdx = l, lIdx = 0, rIdx = 0; lIdx < lLen && rIdx < rLen; ++oIdx)
//		if (strcmp(lCpy[lIdx], rCpy[rIdx]) <= 0)
//			arr[oIdx] = lCpy[lIdx++];
//		else
//			arr[oIdx] = rCpy[rIdx++];
//
//	while (lIdx < lLen)
//		arr[oIdx++] = lCpy[lIdx++];
//	while (rIdx < rLen)
//		arr[oIdx++] = rCpy[rIdx++];
//
//	free(lCpy);
//	free(rCpy);
//}
//
//static void mergeSort(char const** arr, int32_t l, int32_t r)
//{
//	if (l < r)
//	{
//		int m = l + (r - l) / 2;
//
//		mergeSort(arr, l, m);
//		mergeSort(arr, m + 1, r);
//
//		merge(arr, l, m, r);
//	}
//}
//
//
//void Vector::sort()
//{
//	if (data && count)
//		mergeSort((char const**)data, 0, count - 1);
//}

template<class T>
template<class Cmp_t>
inline bool Vector<T>::find(T const & other, Cmp_t cmp)
{
	for (size_t i = 0; i < count; ++i)
		if (cmp((*this)[i], other))
			return true;
	return false;
}
