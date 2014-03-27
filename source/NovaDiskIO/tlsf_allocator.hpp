//  tlsf allocator class
//  Copyright (C) 2013 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef TLSF_ALLOCATOR_HPP
#define TLSF_ALLOCATOR_HPP

extern "C"
{
#include "TLSF-2.4.6/src/tlsf.h"
}

#include <array>

namespace nova {

/** constant-sized, pooled memory allocator based on TLSF allocator
 *
 *  \todo this violates the allocator requirement in 20.1.5, paragraph 4
 *        objects have to be freed using the same instance that was used
 *        to allocate them.
 *
 *  */
template <class T,
		  std::size_t count>
class tlsf_allocator
{
	static const std::size_t bytes = 2 * count * sizeof(T) + 4096 * 2;

	static_assert(bytes % sizeof(long) == 0,
				  "tlsf_allocator: bytes not an integer mutiple of sizeof(long)");

	static const std::size_t poolsize = bytes/sizeof(long);

public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	template <class U> struct rebind
	{
		typedef tlsf_allocator<U, count> other;
	};

	tlsf_allocator() noexcept
	{
		pool.fill(0);
		init_memory_pool(bytes, pool.begin());
	}

	tlsf_allocator(tlsf_allocator const & ) = delete;

	~tlsf_allocator() noexcept
	{
		destroy_memory_pool(pool.begin());
	}

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	pointer allocate(size_type n, const_pointer hint = 0)
	{
		pointer ret = static_cast<pointer>(malloc_ex(n * sizeof(T), pool.begin()));

		if (ret == 0)
			throw std::bad_alloc();

		return ret;
	}

	void deallocate(pointer p, size_type n)
	{
		free_ex(p, pool.begin());
	}

	size_type max_size() const throw()
	{
		return count;
	}

	void construct(pointer p, const T& val)
	{
		::new(p) T(val);
	}

	void destroy(pointer p)
	{
		p->~T();
	}

private:
	std::array<long, poolsize> pool;
};


template<typename T, std::size_t ts, typename U, std::size_t us>
bool operator==( tlsf_allocator<T, ts> const& left, tlsf_allocator<U, us> const& right )
{
	return !(left != right);
}

template<typename T, std::size_t ts, typename U, std::size_t us>
bool operator!=( tlsf_allocator<T, ts> const& left, tlsf_allocator<U, us> const& right )
{
	return true;
}

} /* namespace nova */

#endif
