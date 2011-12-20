// Copyright (C) 2010 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "thread.h"

#ifndef BB_MACHINE
#define BB_MACHINE

//static const u8 MAX_THREADS=2;
static const u32 HEAP_SIZE=256;

class machine
{
public:
	//machine(sound* s);
	machine();
	~machine();

	// global
	u8   peek(u32 addr) const;
	void poke(u32 addr, u8 data);
	void print_instr(u8 instr) const;

	void run();
	void dump() const;

    u32 find_previous_org(u32 addr) const;
	
    void init_thread(u32 addr);
	thread get_thread() { return m_thread; } 

	// make this public such that I can fill it with junk from a buffer
	u8 m_heap[HEAP_SIZE];
		
private:
	u32 get_free_thread();

    //u32 m_recycle_thread;

	thread m_thread;

};

#endif
