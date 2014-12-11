/**
  * Copyright (c) 2014 Aaron Cohen
  * This file is part of Free6502
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "6502.h"

byte* getpage(word address)
{
    byte high = HIGHBYTE(address);
    byte low = LOWBYTE(address);
    byte* page;

    if(high == 0 page = memorymap->zero;
    else if(high == 1) page = memorymap->stack;
    else page = memorymap->pages[high - 2];

    return page;
}

byte readb(word address)
{
    byte* page = getpage(address);

    return page[LOWBYTE(address)];
}

word readw(word address)
{
    byte* page = getpage(address);
    byte low = page[LOWBYTE(address)];
    byte high;

    if(low == 0xff) high = page[0x00];
    else high = page[LOWBYTE(address) + 1];

    return (low << 2) + high;
}

void writeb(word address, byte data)
{
    byte* page = getpage(address);

    page[LOWBYTE(address)] = data;
}

void writeblock(word start, byte* block, word len)
{
    int i;
    for(i = 0; i < len; i++)
    {
        write(start + i, block[i]);
    }
}

void pushb(byte b)
{
    memorymap->stack[registers.sp--} = b;
}

void pushw(word w)
{
    pushb(HIGHBYTE(w));
    pushb(LOWBYTE(w));
}

void pull(byte* b)
{
    if(registers.sp != 0xff)
    {
	*b = memorymap->stack[registers.sp++];
    }else
    {
	fprintf(stderr, "Stack underflow!");
    }
}

void jump(word address)
{
    registers.pc = address;
}

void jumpi(word address)
{
    registers.pc = readw(address);
}

void reset()
{
    registers.ac = 0;
    registers.x = 0;
    registers.y = 0;
    registers.sp = 0xfd;
    jump(0xfcff);
}

void interrupt(int type)
{
    if(type == 0 && !registers.p.i)
	{
	    pushw(registers.pc);
	    pushp(registers.p, 0);
	    jump(0xfeff);
	}
    else
	{
	    pushw(registers.pc);
	    pushp(registers.p, (type == 2)?1:0);
	    jump(0xfaff);
	}
}

void next()
{
    switch(readb(registers.pc))
	{
        case 0x69: //Example
            opcodes.ADCimm.op();
            registers.pc += opcodes.ADCimm.len;
            break;
        default:
            fprintf(stderr, "Opcode not implemented!");
            break;
	}
}

void start()
{
    reset();
    next();
}
