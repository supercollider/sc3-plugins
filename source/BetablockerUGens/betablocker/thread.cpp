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

#include <stdio.h>
#include "thread.h"
#include "machine.h"

thread::thread() :
m_active(false),
m_start(0),
m_pc(0),
m_stack_pos(-1)
{
	for (int n=0; n<STACK_SIZE; n++)
	{
		m_stack[n]=0;
	}
}

thread::~thread()
{
}
	
u8 thread::peek(machine *m, u8 addr) const
{
	return m->peek(m_start+addr);
}

void thread::poke(machine *m, u8 addr, u8 data)
{
	m->poke(m_start+addr,data);
}

void thread::run(machine *m)
{
	//printf("in thread run %d %d\n", this, m_active);
	if (!m_active) return;

	u8 instr=peek(m,m_pc);
	m_pc++;

 	switch(instr)
	{
    case NOP: break;
    case ORG: m_start=m_start+m_pc-1; m_pc=1; break;
    case EQU: push(pop()==pop()); break;
    case JMP: m_pc=peek(m,m_pc++); break;
    //case JMPZ: m_pc++; if (pop()==0) m_pc=peek(m,m_pc); break;
	case JMPZ: if (pop()==0) m_pc=peek(m,m_pc); else m_pc++; break;
    case PSHL: push(peek(m,m_pc++)); break;
    case PSH: push(peek(m,peek(m,m_pc++))); break;
    case PSHI: push(peek(m,peek(m,peek(m,m_pc++)))); break;
    case POP: poke(m,peek(m,m_pc++),pop()); break;
    case POPI: poke(m,peek(m,peek(m,m_pc++)),pop()); break;
    case ADD: push(pop()+pop()); break;
    case SUB: push(pop()-pop()); break;
	case INC: push(pop()+1); break;
    case DEC: push(pop()-1); break;
    case AND: push(pop()&pop()); break;
    case OR: push(pop()|pop()); break;
    case XOR: push(pop()^pop()); break;
    case NOT: push(~pop()); break;
    case ROR: push(pop()>>peek(m,m_pc++)); break;
    case ROL: push(pop()<<peek(m,m_pc++)); break;
    case PIP: 
    {
        u8 d=peek(m,m_pc++); 
        poke(m,d,peek(m,d)+1); 
    } break;
    case PDP: 
    {
        u8 d=peek(m,m_pc++); 
        poke(m,d,peek(m,d)-1); 
    } break;
    case DUP: push(top()); break;
    case NOTE: 
    {
        // m_pitch=pop();
        // m_played_sound=m_pitch;
        // m_sound->play(m_instrument,m_pitch); 
    } break;
    case VOX: 
    {
        // m_instrument=pop();
        // m_played_sound=m_pitch;
        // m_sound->play(m_instrument,m_pitch); 
    } break;
    case STOP: /*m_active=false;*/ break;
    default : break;
	};
// printf("after=%d\n",m_pc);

}
	
void thread::push(u8 data)
{
	//printf("data %d \n", data);

	if (m_stack_pos >= STACK_SIZE-1)
	{
	 	for(size_t n = 1; n < STACK_SIZE; n++)
		{
			m_stack[n-1] = m_stack[n];
		}
		m_stack[STACK_SIZE-1] = data;
	} else {
		m_stack[++m_stack_pos] = data;
	}
}

u8 thread::pop()
{
	if (m_stack_pos>=0)
	{
		u8 ret=m_stack[m_stack_pos];
		m_stack_pos--;
		return ret;
	}
	return 0;
}

u8 thread::top() const
{
	//printf("m_stack_pos %d\n", m_stack_pos);
	if (m_stack_pos>=0)
	{
		return m_stack[m_stack_pos];
	}
	return 0;
}

u8 thread::at(u8 pos) const
{
	if (m_stack_pos>=0 && (pos <= m_stack_pos))
	{
		return m_stack[m_stack_pos - pos];
//		return m_stack[pos];
	}
	return 0;
}


void thread::dump() const
{
	printf("%d \n",m_start+m_pc);

	for (s32 n=0; n<STACK_SIZE; n++)
	// for (s32 n=0; n<(m_stack_pos+1); n++)
	{
		printf("%d ",m_stack[n]);
	}
	printf("\n");
}
