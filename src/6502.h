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
#define byte uint8_t
#define word uint16_t

#define LOWBYTE(w) (w&0xff00)>>2
#define HIGHBYTE(w) w&0xff

#define LITTLE(w) (HIGHBYTE(w) << 2) + (LOWBYTE(w))
#define BIG(w) LITTLE(w)

struct PFLAGS
{
    bit n; //Negative bit
    bit v; //Overflow bit
    //Bit 5 - Only exists on stack, always 1
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

//Backend function prototypes
byte* getpage(word address);

byte readb(word address);
word readw(word address);

void writeb(word address, byte data);

void writeblock(word start, byte* block, word len); //For loading large chunks of memory

void pushb(byte b);
void pushw(word w);
void pushp(struct PFLAGS p, bool b);
void pullb(byte* b);

void jump(word address);
void jumpi(word address);

void reset();

/*
 * 0 - Maskable Interrupt (/irq)
 * 1 - Non-Maskable Interrupt (/nmi)
 * 2 - Break Instruction (brk)
 */
void interrupt(int type);

void next();
void start();

//Opcode function prototypes
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

void INCzpf();
void INCzpxf();
void INCabsf();
void INCabsxf();

void JMPabsf();
void JMPindf();

void JSRf();

void LDAimmf();
void LDAzpf();
void LDAzpxf();
void LDAabsf();
void LDAabsxf();
void LDAabsyf();
void LDAindxf();
void LDAindyf();

void LDXimmf();
void LDXzpf();
void LDXzpyf();
void LDXabsf();
void LDXabsyf();

void LDYimmf();
void LDYzpf();
void LDYzpxf();
void LDYabsf();
void LDYabsxf();

void LSRaccf();
void LSRzpf();
void LSRzpxf();
void LSRabsf();
void LSRabsxf();

void NOPf();

void ORAimmf();
void ORAzpf();
void ORAzpxf();
void ORAabsf();
void ORAabsxf();
void ORAabsyf();
void ORAindxf();
void ORAindyf();

void TAXf();
void TXAf();
void DEXf();
void INXf();
void TAYf();
void TYAf();
void DEYf();
void INYf();

void ROLaccf();
void ROLzpf();
void ROLzpxf();
void ROLabsf();
void ROLabsxf();

void ROLaccf();
void ROLzpf();
void ROLzpxf();
void ROLabsf();
void ROLabsxf();

void RTIf();

void RTSf();

void STAzpf();
void STAzpxf();
void STAabsf();
void STAabsxf();
void STAabsyf();
void STAindxf();
void STAindyf();

void TXSf();
void TSXf();
void PHAf();
void PLAf();
void PHPf();
void PLPf();

void STXzpf();
void STXzpyf();
void STXabsf();

void STXzpf();
void STXzpxf();
void STXabsf();

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

    //INCrement memory
    opcode INCzp = { .code = 0xe6, .op = &(INCzpf), .len = 2, .time = 5 };
    opcode INCzpx = { .code = 0xf6, .op = &(INCzpxf), .len = 2, .time = 6 };
    opcode INCabs = { .code = 0xee, .op = &(INCabsf), .len = 3, .time = 6 };
    opcode INCabsx = { .code = 0xfe, .op = &(INCabsxf), .len = 3, .time = 7 };

    //JuMP to location
    opcode JMPabs = { .code = 0x4c, .op = &(JMPabsf), .len = 3, .time = 3 };
    opcode JMPind = { .code = 0x6c, .op = &(JMPindf), .len = 3, .time = 5 };

    //Jump to SubRoutine
    opcode JSR - { .code = 0x20, .op = &(JSRf), .len = 3, .time = 6 };

    //LoaD Accumulator
    opcode LDAimm = { .code = 0xa9, .op = &(LDAimmf), .len = 2, .time = 2 };
    opcode LDAzp = { .code = 0xa5, .op = &(LDAzpf), .len = 2, .time = 3 };
    opcode LDAzpx = { .code = 0xb5, .op = &(LDAzpxf), .len = 2, .time = 4 };
    opcode LDAabs = { .code = 0xad, .op = &(LDAabsf), .len = 3, .time = 4 };
    opcode LDAabsx = { .code = 0xbd, .op = &(LDAabsxf), .len = 3, .time = 4 };
    opcode LDAabsy = { .code = 0xb9, .op = &(LDAabsyf), .len = 3, .time = 4 };
    opcode LDAindx = { .code = 0xa1, .op = &(LDAindxf), .len = 2, .time = 6 };
    opcode LDAindy = { .code = 0xb1, .op = &(LDAindyf), .len = 2, .time = 5 };

    //LoaD X register
    opcode LDXimm = { .code = 0xa2, .op = &(LDXimmf), .len = 2, .time = 2 };
    opcode LDXzp = { .code = 0xa6, .op = &(LDXzpf), .len = 2, .time = 3 };
    opcode LDXzpy = { .code = 0xb6, .op = &(LDXzpyf), .len = 2, .time = 4 };
    opcode LDXabs = { .code = 0xae, .op = &(LDXabsf), .len = 3, .time = 4 };
    opcode LDXabsy = { .code = 0xbe, .op = &(LDXabsyf), .len = 3, .time = 4 };

    //LoaD Y register
    opcode LDYimm = { .code = 0xa0, .op = &(LDYimmf), .len = 2, .time = 2 };
    opcode LDYzp = { .code = 0xa4, .op = &(LDYzpf), .len = 2, .time = 3 };
    opcode LDYzpx = { .code = 0xb4, .op = &(LDYzpxf), .len = 2, .time = 4 };
    opcode LDYabs = { .code = 0xac, .op = &(LDYabsf), .len = 3, .time = 4 };
    opcode LDYabsx = { .code = 0xbc, .op = &(LDYabsxf), .len = 3, .time = 4 };

    //Logical Shift Right
    opcode LSRacc = { .code = 0x4a, .op = &(LSRaccf), .len = 1, .time = 2 };
    opcode LSRzp = { .code = 0x46, .op = &(LSRzpf), .len = 2, .time = 5 };
    opcode LSRzpx = { .code = 0x56, .op = &(LSRzpxf), .len = 2, .time = 6 };
    opcode LSRabs = { .code = 0x4e, .op = &(LSRabsf), .len = 3, .time = 6 };
    opcode LSRabsx = { .code = 0x5e, .op = &(LSRabsxf), .len = 3, .time = 7 };

    //No OPeration
    opcode NOP = { .code = 0xea, .op = &(NOPf), .len = 1, .time = 2 };

    //bitwise OR with Accumulator
    opcode ORAimm = { .code = 0x09, .op = &(ORAimmf), .len = 2, .time = 2 };
    opcode ORAzp = { .code = 0x05, .op = &(ORAzpf), .len = 2, .time = 3 };
    opcode ORAzpx = { .code = 0x15, .op = &(ORAzpxf), .len = 2, .time = 4 };
    opcode ORAabs = { .code = 0x0d, .op = &(ORAabsf), .len = 3, .time = 4 };
    opcode ORAabsx = { .code = 0x1d, .op = &(ORAabsxf), .len = 3, .time = 4 };
    opcode ORAabsy = { .code = 0x19, .op = &(ORAabsyf), .len = 3, .time = 4 };
    opcode ORAindx = { .code = 0x01, .op = &(ORAindxf), .len = 2, .time = 6 };
    opcode ORAindy = { .code = 0x11, .op = &(ORAindyf), .len = 2, .time = 5 };

    //register instructions
    opcode TAX = { .code = 0xaa, .op = &(TAXf), .len = 1, .time = 2 }; //Transfer A to X
    opcode TXA = { .code = 0x8a, .op = &(TXAf), .len = 1, .time = 2 }; //Transfer X to A
    opcode DEX = { .code = 0xca, .op = &(DEXf), .len = 1, .time = 2 }; //DEcrement X
    opcode INX = { .code = 0xe8, .op = &(INXf), .len = 1, .time = 2 }; //INcrement X
    opcode TAY = { .code = 0xa8, .op = &(TAYf), .len = 1, .time = 2 }; //Transfer A to Y
    opcode TYA = { .code = 0x98, .op = &(TYAf), .len = 1, .time = 2 }; //Transfer Y to A
    opcode DEY = { .code = 0x88, .op = &(DEYf), .len = 1, .time = 2 }; //DEcrement Y
    opcode INY = { .code = 0xc8, .op = &(INYf), .len = 1, .time = 2 }; //INcrement Y

    //ROtate Left
    opcode ROLacc = { .code = 0x2a, .op = &(ROLaccf), .len = 1, .time = 2 };
    opcode ROLzp = { .code = 0x26, .op = &(ROLzpf), .len = 2, .time = 5 };
    opcode ROLzpx = { .code = 0x36, .op = &(ROLzpxf), .len = 2, .time = 6 };
    opcode ROLabs = { .code = 0x2e, .op = &(ROLabsf), .len = 3, .time = 6 };
    opcode ROLabsx = { .code = 0x3e, .op = &(ROLabsxf), .len = 3, .time = 7 };

    //ROtate Right
    opcode RORacc = { .code = 0x6a, .op = &(RORaccf), .len = 1, .time = 2 };
    opcode RORzp = { .code = 0x66, .op = &(RORzpf), .len = 2, .time = 5 };
    opcode RORzpx = { .code = 0x76, .op = &(RORzpxf), .len = 2, .time = 6 };
    opcode RORabs = { .code = 0x6e, .op = &(RORabsf), .len = 3, .time = 6 };
    opcode RORabsx = { .code = 0x7e, .op = &(RORabsxf), .len = 3, .time = 7 };

    //ReTurn from Interrupt
    opcode RTI = { .code = 0x40, .op = &(RTIf), .len = 1, .time = 6 };

    //ReTurn from Subroutine
    opcode RTS = { .code = 0x60, .op = &(RTSf), .len = 1, .time = 6 };

    //SuBtract with Carry
    opcode SBCimm = { .code = 0xe9, .op = &(SBCimmf), .len = 2, .time = 2 };
    opcode SBCzp = { .code = 0xe5, .op = &(SBCzpf), .len = 2, .time = 3 };
    opcode SBCzpx = { .code = 0xf5, .op = &(SBCzpxf), .len = 2, .time = 4 };
    opcode SBCabs = { .code = 0xed, .op = &(SBCabsf), .len = 3, .time = 4 };
    opcode SBCabsx = { .code = 0xfd, .op = &(SBCabsxf), .len = 3, .time = 4 };
    opcode SBCabsy = { .code = 0xf9, .op = &(SBCabsyf), .len = 3, .time = 4 };
    opcode SBCindx = { .code = 0xe1, .op = &(SBCindxf), .len = 2, .time = 6 };
    opcode SBCindy = { .code = 0xf1, .op = &(SBCindyf), .len = 2, .time = 5 };

    //STore Accumulator
    opcode STAzp = { .code = 0x85, .op = &(STAzpf), .len = 2, .time = 3 };
    opcode STAzpx = { .code = 0x95, .op = &(STAzpxf), .len = 2, .time = 4 };
    opcode STAabs = { .code = 0x8d, .op = &(STAabsf), .len = 3, .time = 4 };
    opcode STAabsx = { .code = 0x9d, .op = &(STAabsxf), .len = 3, .time = 5 };
    opcode STAabsy = { .code = 0x99, .op = &(STAabsyf), .len = 3, .time = 5 };
    opcode STAindx = { .code = 0x81, .op = &(STAindxf), .len = 2, .time = 6 };
    opcode STAindy = { .code = 0x91, .op = &(STAindyf), .len = 2, .time = 6 };

    //stack instructions
    opcode TXS = { .code = 0x9a, .op = &(TXSf), .len = 1, .time = 2 }; //Transfer X to Stack pointer
    opcode TSX = { .code = 0xba, .op = &(TSXf), .len = 1, .time = 2 }; //Transfer Stack pointer to X
    opcode PHA = { .code = 0x48, .op = &(PHAf), .len = 1, .time = 3 }; //PusH Accumulator
    opcode PLA = { .code = 0x68, .op = &(PLAf), .len = 1, .time = 4 }; //PuLl (pop) Accumulator
    opcode PHP = { .code = 0x08, .op = &(PHPf), .len = 1, .time = 3 }; //PusH Processor status
    opcode PLP = { .code = 0x28, .op = &(PLPf), .len = 1, .time = 4 }; //PuLl (pop) Processor status

    //STore X register
    opcode STXzp = { .code = 0x86, .op = &(STXzpf), .len = 2, .time = 3 };
    opcode STXzpy = { .code = 0x96, .op = &(STXzpyf), .len = 2, .time = 4 };
    opcode STXabs = { .code = 0x8e, .op = &(STXabsf), .len = 3, .time = 4 };

    //STore Y register
    opcode STYzp = { .code = 0x84, .op = &(STYzpf), .len = 2, .time = 3 };
    opcode STYzpy = { .code = 0x94, .op = &(STYzpyf), .len = 2, .time = 4 };
    opcode STYabs = { .code = 0x8c, .oop = &(STYabsf), .len = 3, .time = 4 };
}

struct INSTRUCTIONS_EX {} //To be used for extensions

#endif // CPU_H_INCLUDED
