#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <cstdint>
#include <QString>
#include <QVector>
#include <sstream>
#include "core.h"

struct InstructionRow
{
    uint32_t offset;
    QString value;
    QString instruction;
    QString hint;
    QString result;
    bool breakpoint;
    bool actualOffset;
};


class Disassembler
{

public:
    Disassembler(Core* core=NULL);
    InstructionRow read(uint32_t offset, int32_t value = -1);
private:
    QString rHexNeg(int32_t value);
    Core* core;
};


#define b0000 0x0
#define b0001 0x1
#define b0010 0x2
#define b0011 0x3
#define b0100 0x4
#define b0101 0x5
#define b0110 0x6
#define b0111 0x7
#define b1000 0x8
#define b1001 0x9
#define b1010 0xA
#define b1011 0xB
#define b1100 0xC
#define b1101 0xD
#define b1110 0xE
#define b1111 0xF

#endif // DISASSEMBLER_H
