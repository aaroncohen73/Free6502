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

//All memory read/write operations should assume BIG ENDIAN (Makes it easier)

byte* getpage(word address) //Get the current memory page
{
    byte high = HIGHBYTE(address);
    byte low = LOWBYTE(address);
    byte* page;

    if(high == 0) page = memorymap->zero;
    else if(high == 1) page = memorymap->stack;
    else page = memorymap->pages[high - 2];

    return page;
}

byte readb(word address)
{
    byte* page = getpage(address);

    return page[LOWBYTE(address)];
}

byte* readbp(word address)
{
    byte* page = getpage(address);

    return &(page[LOWBYTE(address)]);
}

word readw(word address)
{
    byte* page = getpage(address);
    byte low = page[LOWBYTE(address)];
    byte high;

    if(low == 0xff) high = page[0x00];
    else high = page[LOWBYTE(address) + 1];

    return (low << 8) + high;
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

void pushp(struct PFLAGS p, bool b)
{
    pushb(PtoB(p, b));
}

byte pullb()
{
    byte b = memorymap->stack[registers.sp];
    memorymap->stack[registers.sp] = 0;
    registers.sp++;

    return b;
}

word pullw()
{
    byte low = pullb();
    byte high = pullb();

    return (low << 8) + high;
}

struct PFLAGS pullp()
{
    byte b;
    
    b = pullb();
    return BtoP(b);
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
    registers.sp = 0xfd; //For some reason it's not default to 0xff
    jumpi(0xfcff);
}

void interrupt(int type)
{
    if(type == 0 && !registers.p.i) //If it's a maskable interrupt
	{
	    pushw(registers.pc);
	    pushp(registers.p, 0);
	    jumpi(0xfeff);
	}
    else //If it's non-maskable
	{
	    pushw(registers.pc);
	    pushp(registers.p, (type == 2)?1:0); //Bit 4 is only set if interrupt was called with BRK
	    jumpi(0xfaff);
	}
}

void next()
{
    opcode o;
    
    o.op();
    incPC(o.len);
}

void start()
{
    reset();
    next();
}

void ADC(byte src, byte* dest)
{
    registers.p.v = (src | *dest < 0x80 && src + *dest >= 0x80) || //If src + *dest > 127
                    (src & *dest >= 0x80 && src + *dest < 0x80) //If src + *dest < -128
    
    if(registers.p.d)
	{
	    byte low = LOWNIBBLE(src) + LOWNIBBLE(*dest);
	    byte high = HIGHNIBBLE(src) + HIGHNIBBLE(*dest);

	    if(low >= 10)
		{
		    high += low - (low % 10);
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
	    int result_safe = src + *dest;

	    if(result_safe > 0xff)
		{
		    registers.p.c = true;
		    result_safe &= 0xff;
		}

	    *dest = (byte) result_safe;
	}
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void AND(byte src, byte* dest)
{
    *dest &= src;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void ASL(byte* dest)
{
    int dest_safe = (int) *dest << 1;
    
    *dest = (byte) dest_safe & 0xff;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(dest_safe);
    registers.p.c = (dest_safe & 0x100);
}

void BIT(byte b1, byte b2)
{
    byte result = b1 & b2;
    /********************/
    registers.p.z = (result == 0);
    registers.p.n = (result & 0x80);
    registers.p.v = (result & 0x40);
}

void CMP(byte b1, byte b2)
{
    registers.p.c = (b1 >= b2);
    registers.p.z = (b1 == b2);
    registers.p.n = NEGATIVE(b1);
}

void DEC(byte* dest)
{
    *dest--;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void EOR(byte src, byte* dest)
{
    *dest ^= src;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void INC(byte* dest)
{
    *dest++;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void LSR(byte* dest)
{
    int dest_safe = (int) *dest >> 7;
    
    *dest = (byte) dest_safe;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
    registers.p.c = (dest_safe & 0x80);
}

void ORA(byte src, byte* dest)
{
    *dest |= src;
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void ROL(byte* dest)
{
    int dest_safe = (int) *dest << 1;

    *dest = (byte) (dest_safe & 0xff) + registers.p.c;
    /********************/
    registers.p.c = (dest_safe & 0x100);
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
    
}

void ROR(byte* dest)
{
    int dest_safe = (int) *dest >> 1;
    
    *dest = (byte) dest_safe + (registers.p.c << 7);
    /********************/
    registers.p.c = (dest_safe & 0x80);
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void SBC(byte src, byte* dest) //I think it will work, but if it crashes and burns, this is probably the issue
{
    registers.p.v = (src | *dest < 0x80 && src + *dest >= 0x80) || //If src + *dest > 127
	            (src & *dest >= 0x80 && src + *dest < 0x80); //If src + *dest < -128
    
    registers.p.c = true;
    
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
	    int result_safe = *dest - src;

	    if(result_safe > 0xff)
		{
		    registers.p.c = false;
		    result_safe &= 0xff;
		}

	    *dest = (byte) result_safe;
	}
    /********************/
    registers.p.z = (*dest == 0);
    registers.p.n = NEGATIVE(*dest);
}

void MOV(byte src, byte* dest, bool flags)
{
    *dest = src;
    /********************/
    if(flags) //If flags need to be set
	{
	    registers.p.z = (*dest == 0);
	    registers.p.n = NEGATIVE(*dest);
	}
}

//Here we go!
void ADCimmf() { ADC(readb(registers.pc + 1), &(registers.ac)); }
void ADCzpf() { ADC(readb(readb(registers.pc + 1)), &(registers.ac)); }
void ADCzpxf() { ADC(readb(readb(registers.pc + 1) + registers.x), &(registers.ac)); }
void ADCabsf() { ADC(readb(readw(WORDPLUS(registers.pc, 1))), &(registers.ac)); }
void ADCabsxf() { ADC(readb(readw(WORDPLUS(registers.pc, 1)) + registers.x), &(registers.ac)); }
void ADCabsyf() { ADC(readb(readw(WORDPLUS(registers.pc, 1)) + registers.y), &(registers.ac)); }
void ADCindxf() { ADC(readb(readw(readb(registers.pc + 1) + registers.x)), &(registers.ac)); }
void ADCindyf() { ADC(readb(WORDPLUS(readw(readb(registers.pc + 1)) + registers.y)), &(registers.ac)); }

void ANDimmf() { AND(readb(registers.pc + 1), &(registers.ac)); }
void ANDzpf() { AND(readb(readb(registers.pc + 1)), &(registers.ac)); }
void ANDzpxf() { AND(readb(readb(registers.pc + 1) + registers.x), &(registers.ac)); }
void ANDabsf() { AND(readb(readw(WORDPLUS(registers.pc, 1))), &(registers.ac)); }
void ANDabsxf() { AND(readb(readw(WORDPLUS(registers.pc, 1)) + registers.x), &(registers.ac)); }
void ANDabsyf() { AND(readb(readw(WORDPLUS(registers.pc, 1)) + registers.y), &(registers.ac)); }
void ANDindxf() { AND(readb(readw(WORDPLUS(readb(registers.pc + 1), registers.x))), &(registers.ac)); }
void ANDindyf() { AND(readb(readw(readb(registers.pc + 1)) + registers.y), &(registers.ac)); }

void ASLaccf() { ASL(&(registers.ac)); }
void ASLzpf() { ASL(readbp(readb(registers.pc + 1))); }
void ASLzpxf() { ASL(readbp(readb(registers.pc + 1) + registers.x)); }
void ASLabsf() { ASL(readbp(readw_o(registers.pc, 1))); }
void ASLabsxf() { ASL(readbp(readw_o(registers.pc, 1) + registers.x)); }

void BITzpf() { BIT(readb(readb(registers.pc + 1))); }
void BITabsf() { BIT(readb(readw_o(registers.pc, 1))); }

//If only functions were first class objects in C... this would be much easier!
void BPLf() { if(!registers.p.n) incPC(readb(registers.pc + 1)); }
void BMIf() { if(registers.p.n) incPC(readb(registers.pc + 1)); }
void BVCf() { if(!registers.p.v) incPC(readb(registers.pc + 1)); }
void BVSf() { if(registers.p.v) incPC(readb(registers.pc + 1)); }
void BCCf() { if(!registers.p.c) incPC(readb(registers.pc + 1)); }
void BCSf() { if(registers.p.c) incPC(readb(registers.pc + 1)); }
void BNEf() { if(!registers.p.z) incPC(readb(registers.pc + 1)); }
void BEQf() { if(registers.p.z) incPC(readb(registers.pc + 1)); }

void BRKf() { interrupt(2); }

void CMPimmf() { CMP(readb(registers.pc + 1), registers.ac); }
void CMPzpf() { CMP(readb(readb(registers.pc + 1)), registers.ac); }
void CMPzpxf() { CMP(readb(readb(registers.pc + 1) + registers.x), registers.ac); }
void CMPabsf() { CMP(readb(readw_o(registers.pc, 1)), registers.ac); }
void CMPabsxf() { CMP(readb(readw_o(registers.pc, 1) + registers.x), registers.ac); }
void CMPabsyf() { CMP(readb(readw_o(registers.pc, 1) + registers.y), registers.ac); }
void CMPindxf() { CMP(readb(readw(readb(registers.pc + 1) + registers.x)), registers.ac); }
void CMPindyf() { CMP(readb(readw(readb(registers.pc + 1)) + registers.y), registers.ac); }

void CPXimmf() { CMP(readb(registers.pc + 1), registers.x); }
void CPXzpf() { CMP(readb(readb(registers.pc + 1)), registers.x); }
void CPXabsf() { CMP(readb(readw_o(registers.pc, 1)), registers.x); }

void CPYimmf() { CMP(readb(registers.pc + 1), registers.y); }
void CPYzpf() { CMP(readb(readb(registers.pc + 1)), registers.y); }
void CPYabsf() { CMP(readb(readw_o(registers.pc, 1)), registers.y); }

void DECzpf() { DEC(readbp(readb(registers.pc + 1))); }
void DECzpxf() { DEC(readbp(readb(registers.pc + 1) + registers.x)); }
void DECabsf() { DEC(readbp(readw_o(registers.pc, 1))); }
void DECabsxf() { DEC(readbp(readw_o(registers.pc, 1) + registers.x)); }

void EORimmf() { EOR(readb(registers.pc + 1), &(registers.ac)); }
void EORzpf() { EOR(readb(readb(registers.pc + 1)), &(registers.ac)); }
void EORzpxf() { EOR(readb(readb(registers.pc + 1) + registers.x), &(registers.ac)); }
void EORabsf() { EOR(readb(readw_o(registers.pc, 1)), &(registers.ac)); }
void EORabsxf() { EOR(readb(readw_o(registers.pc, 1) + registers.x), &(registers.ac)); }
void EORabsyf() { EOR(readb(readw_o(registers.pc, 1) + registers.y), &(registers.ac)); }
void EORindxf() { EOR(readb(readw(readb(registers.pc + 1) + registers.x)), &(registers.ac)); }
void EORindyf() { EOR(readb(readw(readb(registers.pc + 1)) + registers.y), &(registers.ac)); }

void CLCf() { registers.p.c = false; }
void SECf() { registers.p.c = true; }
void CLIf() { registers.p.i = false; }
void SEIf() { registers.p.i = true; }
void CLVf() { registers.p.v = false; }
void CLDf() { registers.p.d = false; }
void SEDf() { registers.p.d = true; }

void INCzpf() { INC(readbp(readb(registers.pc + 1))); }
void INCzpxf() { INC(readbp(readb(registers.pc + 1) + registers.x)); }
void INCabsf() { INC(readbp(readw_o(registers.pc, 1))); }
void INCabsxf() { INC(readbp(readw_o(registers.pc, 1) + registers.x)); }

void JMPabsf() { registers.pc = readw_o(registers.pc, 1); }
void JMPindf() { registers.pc = readw(readb(readw_o(registers.pc, 1)) << 8 + (readb(registers.pc + 1) == 0xff) ?
				      readb(readw_o(registers.pc, 1) - 0xff) :
				      readb(readw_o(registers.pc, 1) + 1)); }

void JSRf() { pushw(registers.pc); registers.pc = readw_o(registers.pc, 1); }

void LDAimmf() { MOV(readb(registers.pc + 1), &(registers.ac), true); }
void LDAzpf() { MOV(readb(readb(registers.pc + 1)), &(registers.ac), true); }
void LDAzpxf() { MOV(readb(readb(registers.pc + 1) + registers.x), &(registers.ac), true); }
void LDAabsf() { MOV(readb(readw_o(registers.pc, 1)), &(registers.ac), true); }
void LDAabsxf() { MOV(readb(readw_o(registers.pc, 1) + registers.x), &(registers.ac), true); }
void LDAabsyf() { MOV(readb(readw_o(registers.pc, 1) + registers.y), &(registers.ac), true); }
void LDAindxf() { MOV(readb(readb(readb(registers.pc + 1) + registers.x)), &(registers.ac), true); }
void LDAindyf() { MOV(readb(readb(readb(registers.pc + 1)) + registers.y), &(registers.ac), true); }

void LDXimmf() { MOV(readb(registers.pc + 1), &(registers.x), true); }
void LDXzpf() { MOV(readb(readb(registers.pc + 1)), &(registers.x), true); }
void LDXzpyf() { MOV(readb(readb(registers.pc + 1) + registers.y), &(registers.x), true); }
void LDXabsf() { MOV(readb(readw_o(registers.pc, 1)), &(registers.x), true); }
void LDXabsyf() { MOV(readb(readw_o(registers.pc, 1) + registers.y), &(registers.x), true); }

void LDYimmf() { MOV(readb(registers.pc + 1), &(registers.y), true); }
void LDYzpf() { MOV(readb(readb(registers.pc + 1)), &(registers.y), true); }
void LDYzpxf() { MOV(readb(readb(registers.pc + 1) + registers.x), &(registers.y), true); }
void LDYabsf() { MOV(readb(readw_o(registers.pc, 1)), &(registers.y), true); }
void LDYabsxf() { MOV(readb(readw_o(registers.pc, 1) + registers.x), &(registers.y), true); }

void LSRaccf() { LSR(&(registers.ac)); }
void LSRzpf() { LSR(readbp(readb(registers.pc + 1))); }
void LSRzpxf() { LSR(readbp(readb(registers.pc + 1) + registers.x)); }
void LSRabsf() { LSR(readbp(readw_o(registers.pc, 1))); }
void LSRabsxf() { LSR(readbp(readw_o(registers.pc, 1) + registers.x)); }

void NOPf() { return; } //What did you expect?

void ORAimmf() { ORA(readb(registers.pc + 1), &(registers.ac)); }
void ORAzpf() { ORA(readb(readb(registers.pc + 1)), &(registers.ac)); }
void ORAzpxf() { ORA(readb(readb(registers.pc + 1) + registers.x), &(registers.ac)); }
void ORAabsf() { ORA(readb(readw_o(registers.pc + 1)), &(registers.ac)); }
void ORAabsxf() { ORA(readb(readw_o(registers.pc, 1) + registers.x), &(registers.ac)); }
void ORAabsyf() { ORA(readb(readw_o(registers.pc, 1) + registers.y), &(registers.ac)); }
void ORAindxf() { ORA(readb(readb(readb(registers.pc + 1) + registers.x)), &(registers.ac)); }
void ORAindyf() { ORA(readb(readb(readb(registers.pc + 1)) + registers.y), &(registers.ac)); }

void TAXf() { MOV(registers.ac, &(registers.x)); }
void TXAf() { MOV(registers.x, &(registers.ac)); }
void DEXf() { DEC(&(registers.x)); }
void INXf() { INC(&(registers.x)); }
void TAYf() { MOV(registers.ac, &(registers.y)); }
void TYAf() { MOV(registers.y, &(registers.ac)); }
void DEYf() { DEC(&(registers.y)); }
void INYf() { DEC(&(registers.y)); }

void ROLaccf() { ROL(&(registers.ac)); }
void ROLzpf() { ROL(readbp(readb(registers.pc + 1))); }
void ROLzpxf() { ROL(readbp(readb(registers.pc + 1) + registers.x)); }
void ROLabsf() { ROL(readbp(readw(registers.pc + 1))); }
void ROLabsxf() { ROL(readbp(readw(registers.pc + 1) + registers.x)); }

void RORaccf() { ROR(&(registers.ac)); }
void RORzpf() { ROR(readbp(readb(registers.pc + 1))); }
void RORzpxf() { ROR(readbp(readb(registers.pc + 1) + registers.x)); }
void RORabsf() { ROR(readbp(readw(registers.pc + 1))); }
void RORabsxf() { ROR(readbp(readw(registers.pc + 1) + registers.x)); }

void RTIf() { pullp(&(registers.p)); pullw(&(registers.pc)); }
void RTSf() { pullw(&(registers.pc)); incPC(1)}
