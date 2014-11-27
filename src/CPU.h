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

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define bit bool
#define byte int8_t
#define word int16_t

struct PFLAGS
{
    bit n; //Negative bit
    bit v; //Overflow bit
    //Bits 5 - Only exists on stack, always 1
    //Bit 4 (b) - Only exists on stack, set to 1 if there by instruction, not interrupt
    bit d; //BCD bit
    bit i; //Interrupt priority level
    bit z; //Zero bit
    bit c; //Carry bit
}

struct CPUREGS
{
    byte x; //General purpose X register
    byte y; //General purpose Y register
    byte ac; //Accumulator
    struct PFLAGS p; //Processor status flags
    byte sp = 0xff; //Stack pointer
    word pc; //Program Counter
}

struct CPUMEM
{
    byte* zero; //0x0000-0x00ff
    byte* stack; //0x0100-0x01ff
    //NOTE: In Atari 2600, 0x0080-0x00ff is same as 0x0180-0x01ff. For Atari emulators, should be implemented
    byte** pages; //Memory pages from 0x0200-0xffff
    //NOTE: 0xfffa-0xfffb is address of Non-Maskable Interrupt routine (NMI)
    //NOTE: 0xfffc-0xfffd is address of Reset routine (RST)
    //NOTE: 0xfffe-0xffff is address of Maskable Interrupt Request routine (IRQ)
}

#endif // CPU_H_INCLUDED
