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

// #include <nds.h>
#include "instructions.h"
// #include "sound.h"

#ifndef BB_THREAD
#define BB_THREAD

static const u8 STACK_SIZE=8;

class machine;

class thread
{
public: 
	thread();
	~thread();

	// relative to start
	u8 peek(machine *m, u8 addr) const;
	void poke(machine *m, u8 addr, u8 data);
    void set_start(u8 s) { m_start=s; }
    void set_pc(u8 s) { m_pc=s; }
	u8 get_pc() const { return m_pc+m_start; }
	u8 get_start() const { return m_start; }
	void run(machine *m);
    const u8* get_stack() const { return m_stack; }
	void dump() const;
    const int get_stack_pos() const { return m_stack_pos; }
    bool is_active() const { return m_active; }

	void set_active(bool s) { m_active=s; }

	u8 top() const;
	u8 at(u8 pos) const;

	u8 m_pc;

	u8 m_stack[STACK_SIZE];
	int m_stack_pos;


private:

	void push(u8 data);
	u8 pop();

	bool m_active;
	u8 m_start;
};

#endif
