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
        writeb(start + i, block[i]);
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
    *b = memorymap->stack[registers.sp++];
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
    opcode o;
    
    o.op();
    registers.pc += o.len();
}

void start()
{
    reset();
    next();
}

void ADC(byte src, byte* dest)
{
    if((src | *dest < 0x80 && src + *dest >= 0x80) || //If src + *dest > 127
       (src & *dest >= 0x80 && src + *dest < 0x80)) //If src + *dest < -128
	registers.p.v = true;
    
    if(registers.p.d)
	{
	    byte low = LOWNIBBLE(src) + LOWNIBBLE(*dest);
	    byte high = HIGHNIBBLE(src) + HIGHNIBBLE(*dest);

	    if(low >= 10)
		{
		    high++;
		    low %= 10;
		}
	    if(high >= 10)
		{
		    registers.p.c = true;
		    high %= 10;
		}

	    *dest = (high << 4) + low;
	}
    else
	{
	    word result_safe = src + *dest;

	    if(result_safe > 0xff)
		{
		    registers.p.c = true;
		    result_safe &= 0xff;
		}

	    *dest = (byte) result_safe;
	}

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void AND(byte src, byte* dest)
{
    *dest &= src;

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void ASL(byte* dest)
{
    word dest_safe = (word) *dest << 1;
    *dest = dest_safe & 0xff;
    
    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(dest_safe)) registers.p.n = true;
    if(dest_safe & 0x100) registers.p.c = true;
}

void BIT(byte b1, byte b2)
{
    byte result = b1 & b2;

    if(result == 0) registers.p.z = true;
    if(result & 0x80) registers.p.n = true;
    if(result & 0x40) registers.p.v = true;
}

void CMP(byte b1, byte b2)
{
    if(b1 >= b2) registers.p.c = true;
    if(b1 == b2) registers.p.z = true;
    if(NEGATIVE(b1)) registers.p.n = true;
}

void EOR(byte src, byte* dest)
{
    *dest ^= src;

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void LSR(byte* dest)
{
    word dest_safe = (word) *dest << 7;
    
    *dest = (dest_safe & 0xff00) >> 8;

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
    if(dest_safe & 0x80) registers.p.c = true;
}

void ORA(byte src, byte* dest)
{
    *dest |= src;

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void ROL(byte* dest)
{
    word dest_safe = (word) *dest << 1;

    *dest = (dest_safe & 0xff) + registers.p.c;
    registers.p.c = dest_safe & 0x100;
    
    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
    
}

void ROR(byte* dest)
{
    word dest_safe = (word) *dest << 7;

    *dest = (dest_safe & 0xff) + (registers.p.c << 7);
    registers.p.c = dest_safe & 0x80;
    
    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void SBC(byte src, byte* dest) //I think it will work, but if it crashes and burns, this is probably the issue
{   
    registers.p.c = true;

    if((src | *dest < 0x80 && src + *dest >= 0x80) || //If src + *dest > 127
       (src & *dest >= 0x80 && src + *dest < 0x80)) //If src + *dest < -128
	registers.p.v = true;
    
    if(registers.p.d)
	{
	    byte low = LOWNIBBLE(*dest) - LOWNIBBLE(src);
	    byte high = HIGHNIBBLE(*dest) - HIGHNIBBLE(src);

	    if(low > 0xf)
		{
		    high--;
		    low &= 0xf;
		}
	    if(high > 0xf)
		{
		    registers.p.c = false;
		    high &= 0xf;
		}

	    *dest = (high << 4) + low;
	}
    else
	{
	    word result_safe = *dest - src;

	    if(result_safe > 0xff)
		{
		    registers.p.c = false;
		    result_safe &= 0xff;
		}

	    *dest = (byte) result_safe;
	}

    if(*dest == 0) registers.p.z = true;
    if(NEGATIVE(*dest)) registers.p.n = true;
}

void MOV(byte src, byte* dest, bool ld)
{
    *dest = src;

    if(ld) //If this is an ld* instruction
	{
	    if(*dest == 0) registers.p.z = true;
	    if(NEGATIVE(*dest)) registers.p.n = true;
	}
}
