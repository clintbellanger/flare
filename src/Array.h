/*
Copyright 2011 Clint Bellanger

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

#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>

/**
 * class Array
 *
 * Wraps a C array, giving it value semantics.
 */

template<typename T, std::size_t N>
class Array {
private:
	T arr_[N];

public:
	typedef T* iterator;
	typedef T const* const_iterator;
	Array() {}

	Array(T const& t) {
		reset(t);
	}

	std::size_t size() const {
		return N;
	}

	void reset(T const& t = T()) {
		for (std::size_t i = 0; i < N; ++i)
			arr_[i] = t;
	}

	T const& operator[](std::size_t s) const {
		return arr_[s];
	}

	T& operator[](std::size_t s) {
		return arr_[s];
	}

	iterator begin() {
		return arr_;
	}

	const_iterator begin() const {
		return arr_;
	}

	iterator end() {
		return arr_ + N;
	}

	const_iterator end() const {
		return arr_ + N;
	}
};

#endif
