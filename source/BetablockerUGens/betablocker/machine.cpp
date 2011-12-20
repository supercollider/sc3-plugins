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
#include <cstdlib>

#include "machine.h"

machine::machine()
{
	//printf("machine constructor\n");
	for (u32 n=0; n<HEAP_SIZE; n++)
	{
		m_heap[n]=0;
	}

}

machine::~machine()
{
}

u8 machine::peek(u32 addr) const
{
	return m_heap[addr%HEAP_SIZE];
}

void machine::poke(u32 addr, u8 data)
{	
	m_heap[addr%HEAP_SIZE]=data;
}


void machine::run()
{
	m_thread.run(this);
}

void machine::dump() const
{
	for (u32 n=0; n<HEAP_SIZE; n++)
	{
		printf("%d ",m_heap[n]);
	}
	printf("\n");
}


void machine::print_instr(u8 instr) const
{
	switch(instr)
	{
		case NOP:  printf("nop"); break;
		case ORG:  printf("org"); break;
		case EQU:  printf("equ"); break;
		case JMP:  printf("jmp"); break;
		case JMPZ: printf("jmpz"); break;
		case PSHL: printf("pshl"); break;
		case PSH:  printf("psh"); break;
		case PSHI: printf("pshi"); break;
		case POP:  printf("pop"); break;
		case POPI: printf("popi"); break;
		case ADD:  printf("add"); break;
	    case SUB:  printf("sub"); break;
		case INC:  printf("inc"); break;
		case DEC:  printf("dec"); break;
		case AND:  printf("and"); break;
		case OR:   printf("or"); break;
		case XOR:  printf("xor"); break;
		case NOT:  printf("not"); break;
		case ROR:  printf("ror"); break;
		case ROL:  printf("rol"); break;
		case PIP:  printf("pip"); break;
		case PDP:  printf("pdp"); break;
		case DUP: printf("dup"); break;
		case NOTE: printf("note"); break;
		case VOX:  printf("vox"); break;
		case STOP: printf("stop"); break;
		default: printf("%d", instr); break;
	};
}

u32 machine::find_previous_org(u32 addr) const
{
    // find previous org to be relative to
    u32 org=0;
    bool found=false;
    u32 i=0;
    while(i<256 && !found) 
    {
        if (peek(addr-i)==ORG)
        {
            org=addr-i;
            found=true;
        }
        i++;
    }
    return org;
}

void machine::init_thread(u32 addr)
{
	m_thread.set_active(true);
    m_thread.set_start(addr);
    m_thread.set_pc(0);
}
