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
    byte** pages = byte*[254]; //Memory pages from 0x0200-0xffff
    //NOTE: 0xfffa-0xfffb is address of Non-Maskable Interrupt routine (NMI)
    //NOTE: 0xfffc-0xfffd is address of Reset routine (RST)
    //NOTE: 0xfffe-0xffff is address of Maskable Interrupt Request routine (IRQ)
}

struct CPUREGS registers;
struct CPUMEM memorymap;

void reset();

void interrupt(bool maskable);

void ADCimmf();
void ADCzpf();
void ADCzpxf();
void ADCabsf();
void ADCabsxf();
void ADCabsyf();
void ADCindxf();
void ADCindyf();

void ANDimmf();
void ANDzpf();
void ANDzpxf();
void ANDabsf();
void ANDabsxf();
void ANDabsyf();
void ANDindxf();
void ANDindyf();

void ASLaccf();
void ASLzpf();
void ASLzpxf();
void ASLabsf();
void ASLabsxf();

void BITzpf();
void BITabsf();

void BPLf();
void BMIf();
void BVCf();
void BVSf();
void BCCf();
void BCSf();
void BNEf();
void BEQf();

void BRKf();

void CMPimmf();
void CMPzpf();
void CMPzpxf();
void CMPabsf();
void CMPabsxf();
void CMPabsyf();
void CMPindxf();
void CMPindyf();

void CPXimmf();
void CPXzpf();
void CPXabsf();

void CPYimmf();
void CPYzpf();
void CPYabsf();

void DECzpf();
void DECzpxf();
void DECabsf();
void DECabsxf();

void EORimmf();
void EORzpf();
void EORzpxf();
void EORabsf();
void EORabsxf();
void EORabsyf();
void EORindxf();
void EORindyf();

void CLCf();
void SECf();
void CLIf();
void SEIf();
void CLVf();
void CLDf();
void SEDf();

typedef struct
{
    byte code;
    void (*op)();

    int len;
    int time;
} opcode;

struct INSTRUCTIONS
{
    //ADd with Carry
    opcode ADCimm = { .code = 0x69, .op = &(ADCimmf), .len = 2, .time = 2 };
    opcode ADCzp = { .code = 0x65, .op = &(ADCzpf), .len = 2, .time = 3 };
    opcode ADCzpx = { .code = 0x75, .op = &(ADCzpxf), .len = 2, .time = 4 };
    opcode ADCabs = { .code = 0x6d, .op = &(ADCabsf), .len = 3, .time = 4 };
    opcode ADCabsx = { .code = 0x7d, .op = &(ADCabsxf), .len = 3, .time = 4 };
    opcode ADCabsy = { .code = 0x79, .op = &(ADCabsyf), .len = 3, .time = 4 };
    opcode ADCindx = { .code = 0x61, .op = &(ADCindxf), .len = 2, .time = 6 };
    opcode ADCindy = { .code = 0x71, .op = &(ADCindyf), .len = 2, .time = 5 };

    //bitwise AND
    opcode ADDimm = { .code = 0x29, .op = &(ADDimmf), .len = 2, .time = 2 };
    opcode ADDzp = { .code = 0x25, .op = &(ADDzpf), .len = 2, .time = 3 };
    opcode ADDzpx = { .code = 0x35, .op = &(ADDzpxf), .len = 2, .time = 4 };
    opcode ADDabs = { .code = 0x2d, .op = &(ADDabsf), .len = 3, .time = 4 };
    opcode ADDabsx = { .code = 0x3d, .op = &(ADDabsxf), .len = 3, .time = 4 };
    opcode ADDabsy = { .code = 0x39, .op = &(ADDabsyf), .len = 3, .time = 4 };
    opcode ADDindx = { .code = 0x21, .op = &(ADDindxf), .len = 2, .time = 6 };
    opcode ADDindy = { .code = 0x31, .op = &(ADDindyf), .len = 2, .time = 5 };

    //Arithmetic Shift Left
    opcode ASLacc = { .code = 0x0a, .op = &(ASLaccf), .len = 1, .time = 2 };
    opcode ASLzp = { .code = 0x06, .op = &(ASLzpf), .len = 2, .time = 5 };
    opcode ASLzpx = { .code = 0x16, .op = &(ASLzpxf), .len = 2, .time = 6 };
    opcode ASLabs = { .code = 0x0e, .op = &(ASLabsf), .len = 3, .time = 6 };
    opcode ASLabsx = { .code = 0x1e, .op = &(ASLabsxf), .len = 3, .time = 7 };

    //test BITs
    opcode BITzp = { .code = 0x24, .op = &(BITzpf), .len  = 2, .time = 3 };
    opcode BITabs = { .code = 0x2c, .op = &(BITabsf), .len = 3, .time = 4 };

    //Branch
    opcode BPL = { .code = 0x10, .op = &(BPLf), .len = 2, .time = 2 }; //on PLus
    opcode BMI = { .code = 0x30, .op = &(BMIf), .len = 2, .time = 2 }; //on MInus
    opcode BVC = { .code = 0x50, .op = &(BVCf), .len = 2, .time = 2 }; //on oVerflow Clear
    opcode BVS = { .code = 0x70, .op = &(BVSf), .len = 2, .time = 2 }; //on oVerflow Set
    opcode BCC = { .code = 0x90, .op = &(BCCf), .len = 2, .time = 2 }; //on Carry Clear
    opcode BCS = { .code = 0xb0, .op = &(BCSf), .len = 2, .time = 2 }; //on Carry Set
    opcode BNE = { .code = 0xd0, .op = &(BNEf), .len = 2, .time = 2 }; //on Not Equal
    opcode BEQ = { .code = 0xf0, .op = &(BEQf), .len = 2, .time = 2 }; //on EQual

    //BReaK
    opcode BRK = { .code = 0x00, .op = &(BRKf), .len = 1, .time = 7 };

    //CoMPare with accumulator
    opcode CMPimm = { .code = 0xc9, .op = &(CMPimmf), .len = 2, .time = 2 };
    opcode CMPzp = { .code = 0xc5, .op = &(CMPzpf), .len = 2, .time = 3 };
    opcode CMPzpx = { .code = 0xd5, .op = &(CMPzpxf), .len = 2, .time = 4 };
    opcode CMPabs = { .code = 0xcd, .op = &(CMPabsf), .len = 3, .time = 4 };
    opcode CMPabsx = { .code = 0xdd, .op = &(CMPabsxf), .len = 3, .time = 4 };
    opcode CMPabsy = { .code = 0xd9, .op = &(CMPabsyf), .len = 3, .time = 4 };
    opcode CMPindx = { .code = 0xc1, .op = &(CMPindxf), .len = 2, .time = 6 };
    opcode CMPindy = { .code = 0xd1, .op = &(CMPindyf), .len = 2, .time = 5 };

    //ComPare with X register
    opcode CPXimm = { .code = 0xe0, .op = &(CPXimmf), .len = 2, .time = 2 };
    opcode CPXzp = { .code = 0xe4, .op = &(CPXzpf), .len = 2, .time = 3 };
    opcode CPXabs = { .code = 0xec, .op = &(CPXabsf), .len = 2, .time = 4 };

    //ComPare with Y register
    opcode CPYimm = { .code = 0xc0, .op = &(CPYimmf), .len = 2, .time = 2 };
    opcode CPYzp = { .code = 0xc4, .op = &(CPYzpf), .len = 2, .time = 3 };
    opcode CPYabs = { .code = 0xcc, .op = &(CPYabsf), .len  = 3, .time = 4 };

    //DECrement memory
    opcode DECzp = { .code = 0xc6, .op = &(DECzpf), .len = 2, .time = 5 };
    opcode DECzpx = { .code = 0xd6, .op = &(DECzpxf), .len = 2, .time = 6 };
    opcode DECabs = { .code = 0xce, .op = &(DECabsf), .len = 3, .time = 6 };
    opcode DECabsx = { .code = 0xde, .op = &(DECabsxf), .len = 3, .time = 7 };

    //bitwise Exclusive OR
    opcode EORimm = { .code = 0x49, .op = &(EORimmf), .len = 2, .time = 2 };
    opcode EORzp = { .code = 0x45, .op = &(EORzpf), .len = 2, .time = 3 };
    opcode EORzpx = { .code = 0x55, .op = &(EORzpxf), .len = 2, .time = 4 };
    opcode EORabs = { .code = 0x4d, .op = &(EORabsf), .len = 3, .time = 4 };
    opcode EORabsx = { .code = 0x5d, .op = &(EORabsxf), .len = 3, .time = 4 };
    opcode EORabsy = { .code = 0x59, .op = &(EORabsyf), .len = 3, .time = 4 };
    opcode EORindx = { .code = 0x41, .op = &(EORindxf), .len = 2, .time = 6 };
    opcode EORindy = { .code = 0x51, .op = &(EORindyf), .len = 2, .time = 5 };

    //flag/processor status
    opcode CLC = { .code = 0x18, .op = &(CLCf), .len = 1, .time = 2 }; //CLear Carry
    opcode SEC = { .code = 0x38, .op = &(SECf), .len = 1, .time = 2 }; //SEt Carry
    opcode CLI = { .code = 0x58, .op = &(CLIf), .len = 1, .time = 2 }; //CLear Interrupt
    opcode SEI = { .code = 0x78, .op = &(SEIf), .len = 1, .time = 2 }; //SEt Interrupt
    opcode CLV = { .code = 0xb8, .op = &(CLVf), .len = 1, .time = 2 }; //CLear oVerflow
    opcode CLD = { .code = 0xd8, .op = &(CLDf), .len = 1, .time = 2 }; //CLear Decimal
    opcode SED = { .code = 0xf8, .op = &(SEDf), .len = 1, .time = 2 }; //SEt Decimal
}

struct INSTRUCTIONS_EX {} //To be used for extensions

#endif // CPU_H_INCLUDED
