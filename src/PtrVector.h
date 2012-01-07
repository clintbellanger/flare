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

#ifndef PTR_VECTOR_H
#define PTR_VECTOR_H

#include "Assert.h"

#include <vector>
#include <iterator>

/**
 * class PtrVector
 *
 * Wraps a vector of pointers automatically deleting them when it goes out of
 * scope.
 */
 
template<typename T>
class PtrVector {
private:
	std::vector<T*> vec_;
	typedef typename std::vector<T*>::iterator iter_t;
	typedef typename std::vector<T*>::const_iterator citer_t;

	PtrVector(PtrVector const&);
	PtrVector& operator=(PtrVector const&);

	template<typename IMPL>
	class base_iter {
	protected:
		friend class PtrVector;
		T** p_;
		base_iter(T** p) : p_(p) {}
		base_iter() : p_() {}

	public:
		IMPL& operator++() {
			IMPL::increment(p_);
			return static_cast<IMPL&>(*this);
		}
		
		friend bool operator==(base_iter lhs, base_iter rhs) {
			return lhs.p_ == rhs.p_;
		}

		friend bool operator!=(base_iter lhs, base_iter rhs) {
			return lhs.p_ != rhs.p_;
		}
		T const& operator*() const {
			FlareAssert(p_);
			FlareAssert(*p_);
			return **p_;
		}

		T const* operator->() const {
			FlareAssert(p_);
			return *p_;
		}
	};

public:
	class iterator : public base_iter<iterator> {
	private:
		friend class PtrVector;
		friend class base_iter<iterator>;
		typedef base_iter<iterator> base;
		iterator(T** p) : base_iter<iterator>(p) {}
		static void increment(T**& p) {
			++p;
		}
		
	public:
		iterator() {}

		T& operator*() {
			FlareAssert(base::p_);
			FlareAssert(*base::p_);
			return **base::p_;
		}
		
		T* operator->() {
			FlareAssert(base::p_);
			return *base::p_;
		}
	};
	
	class const_iterator : public base_iter<const_iterator> {
	private:
		friend class PtrVector;
		friend class base_iter<const_iterator>;
		const_iterator(T** p) : base_iter<const_iterator>(p) {}
		static void increment(T**& p) {
			++p;
		}
	public:
		const_iterator() {}
	};

	PtrVector() {}

	~PtrVector() {
		clear();
	}
	
	std::size_t size() const {
		return vec_.size();
	}

	std::size_t capacity() const {
		return vec_.capacity();
	}

	bool empty() const {
		return vec_.empty();
	}

	void reserve(std::size_t num) {
		vec_.reserve(num);
	}

	void clear() {
		const iter_t vec_end = vec_.end();
		for (iter_t it = vec_.begin(); it != vec_end; ++it)
			delete *it;
		vec_.clear();
	}

	void push_back(T* p) {
		vec_.push_back(p);
	}

	void pop_back() {
		vec_.pop_back();
	}

	T const& front() const {
		return *vec_.front();
	}

	T& front() {
		return *vec_.front();
	}

	T const& back() const {
		return *vec_.back();
	}

	T& back() {
		return *vec_.back();
	}

	const_iterator begin() const {
		return const_iterator(&vec_.front());
	}

	const_iterator end() const {
		return const_iterator(&vec_.back() + 1);
	}

	iterator begin() {
		return iterator(&vec_.front());
	}

	iterator end() {
		return iterator(&vec_.back() + 1);
	}

	T& operator[](std::size_t i) {
		FlareAssert(i < size());
		return *vec_[i];
	}
	
	T const& operator[](std::size_t i) const {
		FlareAssert(i < size());
		return *vec_[i];
	}
};

#endif 
