#ifndef INST_TEST
#define INST_TEST

#include "../CPU.h"

void driver(CPU& c, bool ignoreFirstBrk);

class ldaTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class adcTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class andTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class aslTest {
public:
    void ZeroPage();
    void Accumulator();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class branchTest {
public:
    void BCC();
    void BCS();
    void BEQ();
    void BMI();
    void BNE();
    void BPL();
    void BVC();
    void BVS();
    void realProgram();
};

class cmpTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class bitTest {
public:
    void ZeroPage();
    void Absolute();
};

class brkTest {
public:
    void Implied();
};

class cpxTest {
public:
    void ZeroPage();
    void Immediate();
    void Absolute();
};

class cpyTest {
public:
    void ZeroPage();
    void Immediate();
    void Absolute();
};

class decTest {
public:
    void ZeroPage();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class deXYTest {
public:
    void Dex();
    void Dey();
};

class eorTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class incTest {
public:
    void ZeroPage();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class inXYTest {
public:
    void Inx();
    void Iny();
};

class jmpTest {
public:
    void Absolute();
    void Indirect();
};

class ldxTest {
public:
    void Immediate();
    void ZeroPage();
    void Absolute();
    void ZeroPageY();
    void AbsoluteY();
};

class ldyTest {
public:
    void Immediate();
    void ZeroPage();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class lsrTest {
public:
    void ZeroPage();
    void Accumulator();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class orTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

class pushPullTest {
public:
    void pha();
    void php();
    void pla();
    void plp();
};

class rolTest {
public:
    void ZeroPage();
    void Accumulator();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class rorTest {
public:
    void ZeroPage();
    void Accumulator();
    void Absolute();
    void ZeroPageX();
    void AbsoluteX();
};

class returnTest {
public:
    void rti();
    void rts();
};

class sbcTest {
public:
    void IndirectX();
    void ZeroPage();
    void Immediate();
    void Absolute();
    void IndirectY();
    void ZeroPageX();
    void AbsoluteY();
    void AbsoluteX();
};

#endif
