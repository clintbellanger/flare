/*
Copyright © 2011-2012 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * Utils
 *
 * Various utility structures, enums, function
 */

#ifndef UTILS_H
#define UTILS_H

#include "Settings.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <cmath>

using std::string;

/**
 * A generic 2-diminsional vector
 *
 * @param T the data type of vector items
 * @param F a real data type for functions that that must deal with a floating
 *			point value.  Only float, double and long double are supported.
 * @param promiscuous true to enable arithmetic operators that liberally accept
 * 			other vector types with members x, y of compatible types, false for
 * 			stricter type safety.
 */
template<typename T, typename F = T, bool promiscuous = true> struct Vec2 {
	T x;
	T y;

	/** Default constructor. */
	Vec2() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

	/** Standard constructor */
	explicit Vec2(const T &_x, const T &_y) : x(_x), y(_y) {}

	/** Copy operator plus conversion from compatible Vec2 types. */
	template<typename U, typename V> explicit Vec2(const Vec2<U, V> &v) : x(v.x), y(v.y) {}

	/* arithmetic assignment operators */
	Vec2 &operator +=(const Vec2 &v)	{x += v.x; y += v.y; return *this;}
	Vec2 &operator -=(const Vec2 &v)	{x -= v.x; y -= v.y; return *this;}
	Vec2 &operator *=(const Vec2 &v)	{x *= v.x; y *= v.y; return *this;}
	Vec2 &operator /=(const Vec2 &v)	{x /= v.x; y /= v.y; return *this;}

	/* arithmetic assignment operators to accept a single value and operate on all members */
	Vec2 &operator +=(F v)				{x += v;   y += v;   return *this;}
	Vec2 &operator -=(F v)				{x -= v;   y -= v;   return *this;}
	Vec2 &operator *=(F v)				{x *= v;   y *= v;   return *this;}
	Vec2 &operator /=(F v)				{x /= v;   y /= v;   return *this;}


	/* generic binary arithmetic operators (will accept any type that it can @= to, where @ is the operator) */
	template<typename U> Vec2 &operator +(const U &v) const {return Vec2(*this) += v;}
	template<typename U> Vec2 &operator -(const U &v) const {return Vec2(*this) -= v;}
	template<typename U> Vec2 &operator *(const U &v) const {return Vec2(*this) *= v;}
	template<typename U> Vec2 &operator /(const U &v) const {return Vec2(*this) /= v;}

	Vec2 lerp(F t, const Vec2 &v) const			{return *this + (v - *this) * t;}
	T dot(const Vec2 &v) const					{return x * v.x + y * v.y;}
	F dist(const Vec2 &v) const					{Vec2 ret(v); return (ret -= *this).length();}

	F length() const {
		F fx = static_cast<F>(x);
		F fy = static_cast<F>(y);
		return std::sqrt(fx * fx + fy * fy);
	}

	void normalize() {
		F l = length();
		x /= l;
		y /= l;
	}
};

/* promiscuous arithmetic operators plus, minus, multiply and divide. */
template<typename T, typename F, typename U> inline Vec2<T, F, true> &operator +=(Vec2<T, F, true> &a, const U &b)	{a.x += b.x; a.y += b.y; return a;}
template<typename T, typename F, typename U> inline Vec2<T, F, true> &operator -=(Vec2<T, F, true> &a, const U &b)	{a.x -= b.x; a.y -= b.y; return a;}
template<typename T, typename F, typename U> inline Vec2<T, F, true> &operator *=(Vec2<T, F, true> &a, const U &b)	{a.x *= b.x; a.y *= b.y; return a;}
template<typename T, typename F, typename U> inline Vec2<T, F, true> &operator /=(Vec2<T, F, true> &a, const U &b)	{a.x /= b.x; a.y /= b.y; return a;}

typedef Vec2<int, float> Point;
typedef Vec2<float> FPoint;
typedef Vec2<double> DPoint;


/**
 * <p>An Uncopyable has no default copy constructor or operator=().  Deriving from Uncopyable at
 * any level of visibility (private, etc.) will prevent the compiler from generating a default
 * copy constructor and assignment operator.  This is analogous to @{see boost::noncopyable}
 * and has no run-time costs when optimized.
 *
 * @see http://www.boost.org/libs/utility/utility.htm#Class_noncopyable
 */
class Uncopyable {
protected:
	Uncopyable() {}
	~Uncopyable() {}

private:
	Uncopyable(const Uncopyable&);
	const Uncopyable &operator=(const Uncopyable&);
};

// message passing struct for various sprites rendered map inline
struct Renderable /*: private Uncopyable */{ // WARNING: copied in Animation::getCurrentFrame()
	Point map_pos;
	SDL_Surface *sprite;
	SDL_Rect src;
	Point offset;
	bool object_layer;
	Point tile;
};

struct Event_Component {
	std::string type;
	std::string s;
	int x;
	int y;
	int z;
	
	Event_Component() : type(), s(), x(0), y(0), z(0) {}
};

// Utility Functions
int round(float f);
Point round(FPoint fp);
Point screen_to_map(int x, int y, int camx, int camy);
Point map_to_screen(int x, int y, int camx, int camy);
Point map_to_collision(Point p);
Point collision_to_map(Point p);
FPoint calcVector(Point pos, int direction, int dist);
bool isWithin(const SDL_Rect &r, const Point &target);
void zsort(Renderable r[], int rnum);
void sort_by_tile(Renderable r[], int rnum);
void drawPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x0, int y0, int x1, int y1, Uint32 color);
void drawLine(SDL_Surface *screen, Point pos0, Point pos1, Uint32 color);
SDL_Surface* createSurface(int width, int height);

/** Convert an SDL_Rect into a Vec2<int, float, true> (a.k.a. Point), omitting w & h */
inline Point toPoint(const SDL_Rect &r) {
	return Point(r.x, r.y);
}

/**
 * is target within the area defined by center and radius?
 */
inline bool isWithin(const Point &center, int radius, const Point &target) {
	return center.dist(target) < radius;
}

/** Generate a random number bertween 0 and 1. */
inline float frand() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

#if 0 /* replaced with std::sort(), but leaving here in case we still want it */
/**
 * As implemented here:
 * http://www.algolist.net/Algorithms/Sorting/Bubble_sort
 */
template <typename T>
void bubbleSort(T arr[], int n) {
      bool swapped = true;
      int j = 0;
      T tmp;
      while (swapped) {
            swapped = false;
            j++;
            for (int i = 0; i < n - j; i++) {
                  if (arr[i] > arr[i + 1]) {
                        tmp = arr[i];
                        arr[i] = arr[i + 1];
                        arr[i + 1] = tmp;
                        swapped = true;
                  }
            }
      }
}
#endif // 0

template <typename T>
void remove(T arr[], int &n, int index) {
	for (int i=index; i<n-1; i++) {
		arr[i] = arr[i+1];
	}
	n--;
}

template <typename T>
void removeDupes(T arr[], int &n) {
	int i = n;
	while (i>0) {
		if (arr[i] == arr[i-1]) {
			remove(arr, n, i);
		}
		i--;
	}
}

#endif
