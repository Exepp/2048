#pragma once
#include "Utility.h"

template<class T>
struct Vec2
{
	Vec2() = default;

	template<class U, class W>
	Vec2(const U& x, const W&y);

	template<class U>
	explicit Vec2(const Vec2<U> other);


	Vec2<T>& clamp(const Vec2<T>& min, const Vec2<T>& max);

	Vec2<T>& normalize();


	Vec2<T> clamped(const Vec2<T>& min, const Vec2<T>& max) const;

	Vec2<T>	normalized() const;

	T length() const;



	T x = 0;
	T y = 0;
};


using Vec2i = Vec2<int32_t>;
using Vec2u = Vec2<uint32_t>;
using Vec2f = Vec2<float_t>;


////////////////////////////////////////////////////////// definitions //////////////////////////////////////////////////////////


template<class T>
template<class U, class W>
inline Vec2<T>::Vec2(const U & x, const W & y) : x(T(x)), y(T(y)){}

template<class T>
template<class U>
inline Vec2<T>::Vec2(const Vec2<U> other) : x(T(other.x)), y(T(other.y)) {}


template<class T>
inline Vec2<T>& Vec2<T>::clamp(const Vec2<T>& min, const Vec2<T>& max)
{
	::clamp(x, min.x, max.x);
	::clamp(y, min.y, max.y);
	return *this;
}

template<class T>
inline Vec2<T>& Vec2<T>::normalize()
{
	T leng = length();
	if (leng)
	{
		x /= leng;
		y /= leng;
	}
	return *this;
}


template<class T>
inline Vec2<T> Vec2<T>::clamped(const Vec2<T>& min, const Vec2<T>& max) const
{
	return Vec2<T>(*this).clamp(min, max);
}

template<class T>
inline Vec2<T> Vec2<T>::normalized() const
{
	return Vec2<T>(*this).normalize();
}

template<class T>
inline T Vec2<T>::length() const
{
	return T(sqrtf(x * x + y * y));
}

////////////////////////////////////////////////////////// operators //////////////////////////////////////////////////////////

template<class T>
inline Vec2<T> operator+(const Vec2<T> & left, const Vec2<T> & right)
{
	return Vec2<T>(left.x + right.x, left.y + right.y);
}

template<class T>
inline Vec2<T> operator-(const Vec2<T> & right)
{
	return Vec2<T>(-right.x, -right.y);
}

template<class T>
inline Vec2<T> operator-(const Vec2<T> & left, const Vec2<T> & right)
{
	return left + (-right);
}

template<class T>
inline Vec2<T> operator*(T left, const Vec2<T> & right)
{
	return Vec2<T>(left * right.x, left * right.y);
}

template<class T>
inline Vec2<T> operator*(const Vec2<T> & left, T right)
{
	return right * left;
}

template<class T>
inline Vec2<T> operator/(const Vec2<T> & left, T right)
{
	if (right)
		return Vec2<T>(left.x / right, left.y / right);
	return Vec2<T>();
}

template<class T>
inline Vec2<T>& operator+=(Vec2<T> & left, const Vec2<T> & right)
{
	return left = (left + right);
}

template<class T>
inline Vec2<T>& operator-=(Vec2<T> & left, const Vec2<T> & right)
{
	return left = (left - right);
}

template<class T>
inline bool operator==(const Vec2<T> & left, const Vec2<T> & right)
{
	return left.x == right.x && left.y == right.y;
}

template<class T>
inline bool operator!=(const Vec2<T> & left, const Vec2<T> & right)
{
	return !(left == right);
}