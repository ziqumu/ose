#include "Disassembler.h"

#define rHex(num) QString::number((int32_t)num, 16).right(8)
#define rDec(num) QString::number((long)num, 10)


Disassembler::Disassembler(Core* core)
{
   this->core = core;
}

InstructionRow Disassembler::read(uint32_t offset, int32_t value)
{
    //init vars
        InstructionRow rtn;
        QVector<QString> nibbleClass;
        nibbleClass.resize(4);
        nibbleClass[0] = "";
        nibbleClass[1] = "";
        nibbleClass[2] = "";
        nibbleClass[3] = "";
        rtn.result = "";
        rtn.hint = "";
        rtn.instruction = "";
        uint32_t pc = 0;
        uint32_t tmp = 0;
    //init values
        pc = offset + 4;
        rtn.offset = offset;
        rtn.breakpoint = false; //Not implemented
        rtn.actualOffset = (core != NULL && rtn.offset == core->getPC()-4);
    //get value
        if(value == -1)
        {
            if(core != NULL)
            {
                try
                {
                    value = core->Read_Word(offset); // move this in the disassembler
                }
                catch (emulator_exception & e )
                {
                    value = -1;
                }
            }
            if(value == -1)
            {
                //Still -1 : line not readeable
                rtn.hint = "This line shouldn't be executed, read or write !";
                rtn.instruction = "This address doesn't exist !";
                rtn.result = "";
                rtn.value = "";
                return rtn;
            }
        }
    //temp
        uint8_t n;
        uint8_t m;
        uint32_t d;
        uint32_t i;
    //Cut in nibble (4 bits)
        uint8_t nibble[4];
        nibble[0] = (value >> 12)&0xf;
        nibble[1] = (value >> 8)&0xf;
        nibble[2] = (value >> 4)&0xf;
        nibble[3] = (value)&0xf;

        /*List of classes css. Do not put h' inside thes tags
         *
         * [mnemonic][/mnemonic] : the mnemonic (added automatically)
         * [n][/n] : the n parameter
         * [m][/m] : the m parameter
         * [d][/d] : the d (disp, displacement) parameter
         * [i][/i] : the i (imm, immediate data) parameter
         * [reg][/reg] : register (if there is no [n] or [m]) (T bit too)
       */
        switch(nibble[0]) // There is ~186 instructions
        {
            case b0000:
                switch(nibble[3])
                {
                    case b0010:
                        switch(nibble[2]){
                            case b0000: //0000nnnn00000010 : STC SR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]sr[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0001: //0000nnnn00010010 : STC GBR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]gbr[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0010: //0000nnnn00100010 : STC VBR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]vbr[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0011: //0000nnnn00110010 : STC SSR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]ssr[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0100: //0000nnnn01000010 : STC SPC,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]spc[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1000: //0000nnnn10000010 : STC R0_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r0_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1001: //0000nnnn10010010 : STC R1_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r1_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1010: //0000nnnn10100010 : STC R2_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r2_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1011: //0000nnnn10110010 : STC R3_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r3_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1100: //0000nnnn11000010 : STC R4_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r4_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1101: //0000nnnn11010010 : STC R5_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r5_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1110: //0000nnnn11100010 : STC R6_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r6_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1111: //0000nnnn11110010 : STC R7_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r7_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                        } break;
                    case b0011:
                        switch(nibble[2]){
                            case b0000: //0000nnnn00000011 : BSRF Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "bsrf [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Branch Subroutine Far | Delayed";
                                break;
                            case b0010: //0000nnnn00100011 : BRAF Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "braf [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Branch Far | Delayed";
                                break;
                            case b1000: //0000nnnn10000011 : PREF @Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "pref @[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Prefetch Data to the Cache | Not emulated";
                                break;
                        }break;
                    case b0100: //0000nnnnmmmm0100 : MOV.B Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        rtn.hint = "Move Data";
                        break;
                    case b0101: //0000nnnnmmmm0101 : MOV.W Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        rtn.hint = "Move Data";
                        break;
                    case b0110: //0000nnnnmmmm0110 : MOV.L Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        rtn.hint = "Move Data";
                        break;
                    case b0111: //0000nnnnmmmm0111 : MUL.L Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mul.l [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Multiply Long : macl=r" + rDec(n) + "*r" + rDec(m);
                        break;
                    case b1000:
                        if(nibble[1] == 0)
                        {
                            switch(nibble[2]) {
                                case b0000: //0000000000001000 : CLRT
                                    rtn.instruction = "clrt";
                                    rtn.result = "0 -> t";
                                    rtn.hint = "Clear T Bit";
                                    break;
                                case b0001: //0000000000011000 : SETT
                                    rtn.instruction = "sett";
                                    rtn.result = "1 -> t";
                                    rtn.hint = "Set T Bit";
                                    break;
                                case b0010: //0000000000101000 : CLRMAC
                                    rtn.instruction = "clrmac";
                                    rtn.hint = "Clear MAC Register";
                                    break;
                                case b0011: //0000000000111000 : LDTLB
                                    rtn.instruction = "ldtlb";
                                    rtn.hint = "Load PTEH/PTEL to TLB | Not emulated";
                                    break;
                                case b0100: //0000000001001000 : CLRS
                                    rtn.instruction = "clrs";
                                    rtn.result = "0 -> s";
                                    rtn.hint = "Clear S Bit";
                                    break;
                                case b0101: //0000000001011000 : SETS
                                    rtn.instruction = "sets";
                                    rtn.result = "1 -> s";
                                    rtn.hint = "Set S Bit";
                                    break;}
                        } break;
                    case b1001:
                        switch(nibble[2]) {
                            case b0000: //0000000000001001 : NOP
                                if(nibble[1] == 0)
                                {
                                    rtn.instruction = "nop";
                                    rtn.hint = "No Operation";
                                }
                                break;
                            case b0001: //0000000000011001 : DIV0U
                                if(nibble[1] == 0)
                                {
                                    rtn.instruction = "div0u";
                                    rtn.hint = "Divide Step 0 as Unsigned";
                                }
                                break;
                            case b0010: //0000nnnn00101001 : MOVT Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "movt [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Move T Bit";
                                break;
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                            case b0000: //0000nnnn00001010 : STS MACH,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]mach[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store System Register";
                                break;
                            case b0001: //0000nnnn00011010 : STS MACL,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]macl[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store System Register";
                                break;
                            case b0010: //0000nnnn00101010 : STS PR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]pr[/reg],[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store System Register";
                                break;
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: //0000000000001011 : RTS
                                rtn.instruction = "rts";
                                rtn.hint = "Return from Subroutine | Delayed";
                                break;
                            case b0001: //0000000000011011 : SLEEP
                                if(nibble[1] == 0)
                                {
                                    rtn.instruction = "sleep";
                                    rtn.hint = "Power-down mode";
                                }
                                break;
                            case b0010: //0000000000101011 : RTE
                                if(nibble[1] == 0)
                                {
                                    rtn.instruction = "rte";
                                    rtn.hint = "Return from Exception | Delayed";
                                }
                                break;
                        } break;
                    case b1100: //0000nnnnmmmm1100 : MOV.B @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b1101: //0000nnnnmmmm1101 : MOV.W @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b1110: //0000nnnnmmmm1110 : MOV.L @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b1111: //0000nnnnmmmm1111 : MAC.L @Rm+,@Rn+
                    nibbleClass[1] = "n";
                    n = nibble[1];
                    nibbleClass[2] = "m";
                    m = nibble[2];
                    rtn.instruction = "mac.l @[m]r" + rDec(m) + "[/m]+,@[n]r" + rDec(n) + "[/n]+";
                    rtn.hint = "Multiply and Accumulate Long";
                    break;
                } break;
            case b0001: //0001nnnnmmmmdddd : MOV.L Rm,@(disp,Rn)
                nibbleClass[1] = "n";
                n = nibble[1];
                nibbleClass[2] = "m";
                m = nibble[2];
                nibbleClass[3] = "d";
                d = 0xf & (long)nibble[3];
                rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@(h'[d]" + rHex(d*4) + "[/d],[n]r" + rDec(n) + "[/n])";
                rtn.hint = "Move Structure Data";
                break;
            case b0010:
                switch(nibble[3]) {
                    case b0000: //0010nnnnmmmm0000 : MOV.B Rm,@Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b [m]r" + rDec(m) + "[/m],@[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0001: //0010nnnnmmmm0001 : MOV.W Rm,@Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0010: //0010nnnnmmmm0010 : MOV.L Rm,@Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0100: //0010nnnnmmmm0100 : MOV.B Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0101: //0010nnnnmmmm0101 : MOV.W Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0110: //0010nnnnmmmm0110 : MOV.L Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0111: //0010nnnnmmmm0111 : DIV0S Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "div0s [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Divide Step 0 as Signed";
                        break;
                    case b1000: //0010nnnnmmmm1000 : TST Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "tst [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Test Logical";
                        break;
                    case b1001: //0010nnnnmmmm1001 : AND Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "and [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "AND Logical";
                        break;
                    case b1010: //0010nnnnmmmm1010 : XOR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "xor [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Exclusive OR Logical";
                        break;
                    case b1011: //0010nnnnmmmm1011 : OR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "or [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "OR Logical";
                        break;
                  case b1100: //0010nnnnmmmm1100 : CMP/STR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/str [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | When a byte in R" + rDec(n) + " equals a byte in R" + rDec(m) + ", 1 -> T";
                        break;
                    case b1101: //0010nnnnmmmm1101 : XTRCT Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "xtrct [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Extract";
                        break;
                    case b1110: //0010nnnnmmmm1110 : MULU Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mulu [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Multiply as Unsigned Word";
                        break;
                    case b1111: //0010nnnnmmmm1111 : MULS Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "muls [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Multiply as Signed Word";
                        break;
                } break;
            case b0011:
                switch(nibble[3]) {
                    case b0000: //0011nnnnmmmm0000 : CMP_EQ Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/eq [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | Equality";
                        break;
                    case b0010: //0011nnnnmmmm0010 : CMP_HS Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/hs [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | Unsigned Greater or Equal";
                        break;
                    case b0011: //0011nnnnmmmm0011 : CMP_GE Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/ge [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | Signed Greater or Equal";
                        break;
                    case b0100: //0011nnnnmmmm0100 : DIV1 Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "div1 [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Divide Step 1 (r" + rDec(m) + "/r" + rDec(n) + ")";
                        break;
                    case b0101: //0011nnnnmmmm0101 : DMULU.L Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "dmulu.l [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Double-Length Multiply as Unsigned";
                        break;
                    case b0110: //0011nnnnmmmm0110 : CMP_HI Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/hi [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | Unsigned Greater";
                        break;
                    case b0111: //0011nnnnmmmm0111 : CMP_GT Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/gt [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Compare Conditionally | Signed Greater";
                        break;
                    case b1000: //0011nnnnmmmm1000 : SUB Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "sub [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Subtract Binary (r" + rDec(n) + " - r" + rDec(m) + ")";
                        break;
                    case b1010: //0011nnnnmmmm1010 : SUBC Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "subc [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Subtract with Carry (r" + rDec(n) + " - r" + rDec(m) + ") | Used for subtraction of data that has more than 32 bits.";
                        break;
                    case b1011: //0011nnnnmmmm1011 : SUBV Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "subv [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Subtract with V Flag Underflow Check (r" + rDec(n) + " - r" + rDec(m) + ")";
                        break;
                    case b1100: //0011nnnnmmmm1100 : ADD Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "add [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Add Binary";
                        break;
                    case b1101: //0011nnnnmmmm1101 : DMULS.L Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "dmuls.l [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Double-Length Multiply as Signed";
                        break;
                    case b1110: //0011nnnnmmmm1110 : ADDC Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "addc [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Add with Carry | Used for addition of data that has more than 32 bits.";
                        break;
                    case b1111: //0011nnnnmmmm1111 : ADDV Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "addv [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Add with V Flag Overflow Check";
                        break;
                } break;
            case b0100:
                switch(nibble[3]) {
                    case b0000:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000000 : SHLL Rn(Same as SHAL)
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shll [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Left | Put the shifted bit in T";
                                break;
                            case b0001: //0100nnnn00010000 : DT Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "dt [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Decrement and Test";
                                break;
                            case b0010: //0100nnnn00100000 : SHAL Rn(Same as SHLL)
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shal [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Arithmetic Left | Put the shifted bit in T";
                                break;
                        } break;
                    case b0001:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000001 : SHLR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shlr [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Right | Put the shifted bit in T";
                                break;
                            case b0001: //0100nnnn00010001 : CMP_PZ Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "cmp/pz [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Compare Conditionally | Greater than or equal to 0";
                                break;
                            case b0010: //0100nnnn00100001 : SHAR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shar [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Arithmetic Right | Put the shifted bit in T";
                                break;
                        } break;
                    case b0010:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000010 : STS.L MACH,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]mach[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "System Control Instruction";
                                break;
                            case b0001: //0100nnnn00010010 : STS.L MACL,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]macl[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "System Control Instruction";
                                break;
                            case b0010: //0100nnnn00100010 : STS.L PR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]pr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "System Control Instruction";
                                break;
                         }break;
                    case b0011:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000011 : STC.L SR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]sr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0001: //0100nnnn00010011 : STC.L GBR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]gbr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0010: //0100nnnn00100011 : STC.L VBR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]vbr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0011: //0100nnnn00110011 : STC.L SSR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]ssr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0100: //0100nnnn01000011 : STC.L SPC,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]spc[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1000: //0100nnnn10000011 : STC.L R0_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r0_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1001: //0100nnnn10010011 : STC.L R1_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r1_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1010: //0100nnnn10100011 : STC.L R2_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r2_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1011: //0100nnnn10110011 : STC.L R3_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r3_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1100: //0100nnnn11000011 : STC.L R4_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r4_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1101: //0100nnnn11010011 : STC.L R5_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r5_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1110: //0100nnnn11100011 : STC.L R6_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r6_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1111: //0100nnnn11110011 : STC.L R7_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r7_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                        } break;
                    case b0100:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000100 : ROTL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotl [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Rotate Left";
                                break;
                            case b0010: //0100nnnn00100100 : ROTCL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotcl [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Rotate with Carry Left";
                                break;
                        } break;
                    case b0101:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000101 : ROTR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotr [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Rotate Right";
                                break;
                            case b0001: //0100nnnn00010101 : CMP_PL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "cmp/pl [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Compare Conditionally | Greater than 0";
                                break;
                            case b0010: //0100nnnn00100101 : ROTCR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotcr [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Rotate with Carry Right";
                                break;
                        } break;
                    case b0110:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00000110 : LDS.L @Rm+,MACH
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds.l @[m]r" + rDec(m) + "[/m]+,[reg]mach[/reg]";
                                rtn.hint = "Load to System Register";
                                break;
                            case b0001: //0100mmmm00010110 : LDS.L @Rm+,MACL
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds.l @[m]r" + rDec(m) + "[/m]+,[reg]macl[/reg]";
                                rtn.hint = "Load to System Register";
                                break;
                            case b0010: //0100mmmm00100110 : LDS.L @Rm+,PR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds.l @[m]r" + rDec(m) + "[/m]+,[reg]pr[/reg]";
                                rtn.hint = "Load to System Register";
                                break;
                        } break;
                    case b0111:
                        switch(nibble[2]){
                            case b0000: //0100mmmm00000111 : LDC.L @Rm+,SR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]sr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0001: //0100mmmm00010111 : LDC.L @Rm+,GBR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]gbr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0010: //0100mmmm00100111 : LDC.L @Rm+,VBR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]vbr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0011: //0100mmmm00110111 : LDC.L @Rm+,SSR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]ssr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0100: //0100mmmm01000111 : LDC.L @Rm+,SPC
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]spc[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1000: //0100mmmm10000111 : LDC.L @Rm+,R0_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r0_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1001: //0100mmmm10010111 : LDC.L @Rm+,R1_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r1_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1010: //0100mmmm10100111 : LDC.L @Rm+,R2_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r2_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1011: //0100mmmm10110111 : LDC.L @Rm+,R3_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r3_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1100: //0100mmmm11000111 : LDC.L @Rm+,R4_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r4_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1101: //0100mmmm11010111 : LDC.L @Rm+,R5_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r5_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1110: //0100mmmm11100111 : LDC.L @Rm+,R6_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r6_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1111: //0100mmmm11110111 : LDC.L @Rm+,R7_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc.l @[m]r" + rDec(m) + "[/m]+,[reg]r7_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                        } break;
                    case b1000:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001000 : SHLL2 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shll2 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Left 2 Bits";
                                break;
                            case b0001: //0100nnnn00011000 : SHLL8 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shll8 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Left 8 Bits";
                                break;
                            case b0010: //0100nnnn00101000 : SHLL16 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shll16 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Left 16 Bits";
                                break;
                        } break;
                    case b1001:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001001 : SHLR2 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shlr2 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Right 2 Bits";
                                break;
                            case b0001: //0100nnnn00011001 : SHLR8 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shlr8 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Right 8 Bits";
                                break;
                            case b0010: //0100nnnn00101001 : SHLR16 Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shlr16 [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Shift Logical Right 16 Bits";
                                break;
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00001010 : LDS Rm,MACH
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds [m]r" + rDec(m) + "[/m],[reg]mach[/reg]";
                                rtn.hint = "Load to System Register";
                            case b0001: //0100mmmm00011010 : LDS Rm,MACL
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds [m]r" + rDec(m) + "[/m],[reg]macl[/reg]";
                                rtn.hint = "Load to System Register";
                            case b0010: //0100mmmm00101010 : LDS Rm,PR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds [m]r" + rDec(m) + "[/m],[reg]pr[/reg]";
                                rtn.hint = "Load to System Register";
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001011 : JSR @Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "jsr @[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Jump to Subroutine | Delayed";
                                break;
                            case b0001: //0100nnnn00011011 : TAS.B @Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "tas.b @[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Test and Set";
                                break;
                            case b0010: //0100nnnn00101011 : JMP @Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "jmp @[n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Jump | Delayed";
                                break;
                        } break;
                    case b1100: //0100nnnnmmmm1100 : SHAD Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "shad [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Shift Arithmetic Dynamically | r" + rDec(m) + ">=0 : shift left else shift right";
                        break;
                    case b1101: //0100nnnnmmmm1101 : SHLD Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "shad [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Shift Logical Dynamically | r" + rDec(m) + ">=0 : shift left else shift right";
                        break;
                    case b1110:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00001110 : LDC Rm,SR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]sr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0001: //0100mmmm00011110 : LDC Rm,GBR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]gbr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                           case b0010: //0100mmmm00101110 : LDC Rm,VBR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]vbr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0011: //0100mmmm00111110 : LDC Rm,SSR
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]ssr[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b0100: //0100mmmm01001110 : LDC Rm,SPC
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]spc[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1000: //0100mmmm10001110 : LDC Rm,R0_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r0_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1001: //0100mmmm10011110 : LDC Rm,R1_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r1_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1010: //0100mmmm10101110 : LDC Rm,R2_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r2_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1011: //0100mmmm10111110 : LDC Rm,R3_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r3_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1100: //0100mmmm11001110 : LDC Rm,R4_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r4_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1101: //0100mmmm11011110 : LDC Rm,R5_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r5_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1110: //0100mmmm11101110 : LDC Rm,R6_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r6_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                            case b1111: //0100mmmm11111110 : LDC Rm,R7_BANK
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "ldc [m]r" + rDec(m) + "[/m],[reg]r7_bank[/reg]";
                                rtn.hint = "Load to Control Register";
                                break;
                        } break;
                    case b1111: //0100nnnnmmmm1111 : MAC.W @Rm+,@Rn+
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mac.w @[m]r" + rDec(m) + "[/m]+,@[n]r" + rDec(n) + "[/n]+";
                        rtn.hint = "Multiply and Accumulate";
                        break;
               } break;
            case b0101: //0101nnnnmmmmdddd : MOV.L @(disp,Rm),Rn
                nibbleClass[1] = "n";
                n = nibble[1];
                nibbleClass[2] = "m";
                m = nibble[2];
                nibbleClass[3] = "d";
                d = 0xf & (long)nibble[3];
                rtn.instruction = "mov.l @(h'[d]" + rHex(d<<2) + "[/d],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                rtn.hint = "Move Structure Data";
                break;
            case b0110:
                switch(nibble[3]) {
                    case b0000: //0110nnnnmmmm0000 : MOV.B @Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b @[m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0001: //0110nnnnmmmm0001 : MOV.W @Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w @[m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0010: //0110nnnnmmmm0010 : MOV.L @Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l @[m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                     case b0011: //0110nnnnmmmm0011 : MOV Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0100: //0110nnnnmmmm0100 : MOV.B @Rm+,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b @[m]r" + rDec(m) + "[/m]+,[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0101: //0110nnnnmmmm0101 : MOV.W @Rm+,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w @[m]r" + rDec(m) + "[/m]+,[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0110: //0110nnnnmmmm0110 : MOV.L @Rm+,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l @[m]r" + rDec(m) + "[/m]+,[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Move Data";
                        break;
                    case b0111: //0110nnnnmmmm0111 : NOT Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "not [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Logical Complement";
                        break;
                    case b1000: //0110nnnnmmmm1000 : SWAP.B Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "swap.b [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Swap Register Halves";
                        break;
                    case b1001: //0110nnnnmmmm1001 : SWAP.W Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "swap.w [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Swap Register Halves";
                        break;
                    case b1010: //0110nnnnmmmm1010 : NEGC Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "negc [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Negate with Carry";
                        break;
                    case b1011: //0110nnnnmmmm1011 : NEG Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "neg [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Negate";
                        break;
                    case b1100: //0110nnnnmmmm1100 : EXTU.B Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "extu.b [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Extend as Unsigned";
                        break;
                    case b1101: //0110nnnnmmmm1101 : EXTU.W Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "extu.w [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Extend as Unsigned";
                        break;
                    case b1110: //0110nnnnmmmm1110 : EXTS.B Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "exts.b [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Extend as Signed";
                        break;
                    case b1111: //0110nnnnmmmm1111 : EXTS.W Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "exts.w [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        rtn.hint = "Extend as Signed";
                        break;
                } break;
            case b0111: //0111nnnniiiiiiii : ADD #imm,Rn
                nibbleClass[1] = "n";
                n = nibble[1];
                nibbleClass[2] = "i";
                nibbleClass[3] = "i";
                i = (nibble[2]<<4) + nibble[3];
                if ((i&0x80)==0) i=(0x000000FF & (long)i);
                else i=(0xFFFFFF00 | (long)i);
                rtn.instruction = "add #h'[i]" + rHexNeg(i) + "[/i],[n]r" + rDec(n) + "[/n]";
                rtn.hint = "Add Binary";
                break;
            case b1000:
                switch(nibble[1]) {
                    case b0000: //10000000nnnndddd : MOV.B R0,@(disp,Rn)
                        nibbleClass[2] = "n";
                        n = nibble[2];
                        nibbleClass[3] = "d";
                        d = nibble[2];
                        d=(0xf & (long)d);
                        rtn.instruction = "mov.b [reg]r0[/reg],@(h'[d]" + rHex(d)  + "[/d],[n]r" + rDec(n) + "[/n])";
                        rtn.hint = "Move Structure Data";
                        break;
                    case b0001: //10000001nnnndddd : MOV.W R0,@(disp,Rn)
                        nibbleClass[2] = "n";
                        n = nibble[2];
                        nibbleClass[3] = "d";
                        d = nibble[2];
                        d=(0xf & (long)d);
                        rtn.instruction = "mov.w [reg]r0[/reg],@(h'[d]" + rHex(d)  + "[/d],[n]r" + rDec(n) + "[/n])";
                        rtn.hint = "Move Structure Data";
                        break;
                    case b0100: //10000100mmmmdddd : MOV.B @(disp,Rm),R0
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        nibbleClass[3] = "d";
                        d = nibble[2];
                        d=(0xf & (long)d);
                        rtn.instruction = "mov.b @(h'[d]" + rHex(d)  + "[/d],[m]r" + rDec(m) + "[/m]),[reg]r0[/reg]";
                        rtn.hint = "Move Structure Data";
                        break;
                    case b0101: //10000101mmmmdddd : MOV.W @(disp,Rm),R0
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        nibbleClass[3] = "d";
                        d = nibble[2];
                        d=(0xf & (long)d);
                        rtn.instruction = "mov.w @(h'[d]" + rHex(d)  + "[/d],[m]r" + rDec(m) + "[/m]),[reg]r0[/reg]";
                        rtn.hint = "Move Structure Data";
                        break;
                    case b1000: //10001000iiiiiiii : CMP_EQ #imm,R0
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = (nibble[2]<<4) + nibble[3];
                        if ((i&0x80)==0) i=(0xFF & (long)i);
                        else i=(0xFFFFFF00 | (long)i);
                        rtn.instruction = "cmp/eq #h'[i]" + rHex(i)  + "[/i],[reg]r0[/reg]";
                        rtn.hint = "Compare Conditionally : equality";
                        break;
                    case b1001: //10001001dddddddd : BT disp
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) d=(0xFF & (long)d); else d=(0xFFFFFF00 | (long)d);
                        rtn.instruction = "bt h'[d]" + rHexNeg(d<<1)  + "[/d]";
                        rtn.hint = "Branch if True | Not delayed";
                        rtn.result = "h'" + rHex(pc+(d<<1));
                        break;
                    case b1011: //10001011dddddddd : BF disp
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) d=(0xFF & (long)d); else d=(0xFFFFFF00 | (long)d);
                        rtn.instruction = "bf h'[d]" + rHexNeg(d<<1)  + "[/d]";
                        rtn.hint = "Branch if False | Not delayed";
                        rtn.result = "h'" + rHex(pc+(d<<1));
                        break;
                    case b1101: //10001101dddddddd : BT/S disp
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) d=(0xFF & (long)d); else d=(0xFFFFFF00 | (long)d);
                        rtn.instruction = "bt/s h'[d]" + rHexNeg(d<<1)  + "[/d]";
                        rtn.hint = "Branch if True | Delayed";
                        rtn.result = "h'" + rHex(pc+(d<<1));
                        break;
                    case b1111: //10001111dddddddd : BF/S disp
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) d=(0xFF & (long)d); else d=(0xFFFFFF00 | (long)d);
                        rtn.instruction = "bf/s h'[d]" + rHexNeg(d<<1)  + "[/d]";
                        rtn.hint = "Branch if False | Delayed";
                        rtn.result = "h'" + rHex(pc+(d<<1));
                        break;
                } break;
            case b1001: //1001nnnndddddddd : MOV.W @(disp,PC),Rn
                nibbleClass[1] = "n";
                nibbleClass[2] = "d";
                nibbleClass[3] = "d";
                d = 0xff&((nibble[2]<<4) + nibble[3]);
                n = nibble[1];
                tmp = (d<<1);
                if ((tmp&0x8000)==0) tmp&=0x0000FFFF; else tmp|=0xFFFF0000;
                tmp += pc;
                rtn.instruction = "mov.w @(h'[d]" + rHex(d<<1) + "[/d],[reg]pc[/reg]),[n]r" + rDec(n) + "[/n]";
                rtn.hint = "Move Immediate Data";
                rtn.result = "@h'" + rHex(tmp);
                break;
           case b1010: //1010dddddddddddd : BRA disp
                nibbleClass[1] = "d";
                nibbleClass[2] = "d";
                nibbleClass[3] = "d";
                d = (nibble[1]<<8) + (nibble[2]<<4) + nibble[3];
                if ((d&0x800)==0) d=(0x00000FFF & d); else d=(0xFFFFF000 | d);
                rtn.instruction = "bra h'[d]" + rHexNeg(d<<1)  + "[/d]";
                rtn.hint = "Branch | Delayed";
                rtn.result = "h'" + rHex(pc+(d<<1));
                break;
            case b1011: //1011dddddddddddd : BSR disp
                nibbleClass[1] = "d";
                nibbleClass[2] = "d";
                nibbleClass[3] = "d";
                d = (nibble[1]<<8) + (nibble[2]<<4) + nibble[3];
                if ((d&0x800)==0) d=(0x00000FFF & d); else d=(0xFFFFF000 | d);
                rtn.instruction = "bsr h'[d]" + rHexNeg(d<<1)  + "[/d]";
                rtn.hint = "Branch to Subroutine | Delayed";
                rtn.result = "h'" + rHex(pc+(d<<1));
                break;
            case b1100:
                switch(nibble[1]) {
                    case b0000: //11000000dddddddd : MOV.B R0,@(disp,GBR)
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = 0xff&((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "mov.b [reg]r0[/reg],@(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg])";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0001: //11000001dddddddd : MOV.W R0,@(disp,GBR)
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = 0xff&((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "mov.w [reg]r0[/reg],@(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg])";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0010: //11000010dddddddd : MOV.L R0,@(disp,GBR)
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = 0xff&((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "mov.l [reg]r0[/reg],@(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg])";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0011: //11000011iiiiiiii : TRAPA #imm
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = (nibble[2]<<4) + nibble[3];
                        i=(0x000000FF & i);
                        rtn.instruction = "trapa #h'[i]" + rHex(i)  + "[/i]";
                        rtn.hint = "Trap Always";
                        break;
                    case b0100: //11000100dddddddd : MOV.B @(disp,GBR),R0
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (0x000000FF & (long)((nibble[2]<<4) + nibble[3]));
                        rtn.instruction = "mov.b @(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg]),[reg]r0[/reg]";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0101: //11000101dddddddd : MOV.W @(disp,GBR),R0
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (0x000000FF & (long)((nibble[2]<<4) + nibble[3]));
                        rtn.instruction = "mov.w @(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg]),[reg]r0[/reg]";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0110: //11000110dddddddd : MOV.L @(disp,GBR),R0
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (0x000000FF & (long)((nibble[2]<<4) + nibble[3]));
                        rtn.instruction = "mov.l @(h'[d]" + rHex(d)  + "[/d],[reg]gbr[/reg]),[reg]r0[/reg]";
                        rtn.hint = "Move Peripheral Data";
                        break;
                    case b0111: //11000111dddddddd : MOVA @(disp,PC),R0
                        nibbleClass[2] = "d";
                        nibbleClass[3] = "d";
                        d = (nibble[2]<<4) + nibble[3];
                        d=(0x000000FF & (long)d)<<2;
                        rtn.instruction = "mova @(h'[d]" + rHex(d) + "[/d],[reg]pc[/reg]),[reg]r0[/reg]";
                        rtn.hint = "Move Effective Address";
                        rtn.result = "@h'" + rHex((pc&0xFFFFFFFC)+d);
                        break;
                    case b1000: //11001000iiiiiiii : TST #imm,R0
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "tst #h'[i]" + rHex(i) + "[/i],[reg]r0[/reg]";
                        rtn.hint = "Test Logical";
                        break;
                    case b1001: //11001001iiiiiiii : AND #imm,R0
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = (int8_t)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "and #h'[i]" + rHex(i) + "[/i],[reg]r0[/reg]";
                        rtn.hint = "AND Logical";
                        break;
                    case b1010: //11001010iiiiiiii : XOR #imm,R0
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "xor #h'[i]" + rHex(i) + "[/i],[reg]r0[/reg]";
                        rtn.hint = "Exclusive OR Logical";
                        break;
                    case b1011: //11001011iiiiiiii : OR #imm,R0
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "and #h'[i]" + rHex(i) + "[/i],[reg]r0[/reg]";
                        rtn.hint = "OR Logical";
                        break;
                    case b1100: //11001100iiiiiiii : TST.B #imm,@(R0,GBR)
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "tst.b #h'[i]" + rHex(i) + "[/i],@([reg]r0[/reg],gbr)";
                        rtn.hint = "Test Logical";
                        break;
                    case b1101: //11001101iiiiiiii : AND.B #imm,@(R0,GBR)
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "and.b #h'[i]" + rHex(i) + "[/i],@([reg]r0[/reg],gbr)";
                        rtn.hint = "AND Logical";
                        break;
                    case b1110: //11001110iiiiiiii : XOR.B #imm,@(R0,GBR)
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "xor.b #h'[i]" + rHex(i) + "[/i],@([reg]r0[/reg],gbr)";
                        rtn.hint = "Exclusive OR Logical";
                        break;
                    case b1111: //11001111iiiiiiii : OR.B #imm,@(R0,GBR)
                        nibbleClass[2] = "i";
                        nibbleClass[3] = "i";
                        i = 0x000000FF & (long)((nibble[2]<<4) + nibble[3]);
                        rtn.instruction = "and.b #h'[i]" + rHex(i) + "[/i],@([reg]r0[/reg],gbr)";
                        rtn.hint = "OR Logical";
                        break;
                } break;
            case b1101: //1101nnnndddddddd : MOV.L @(disp,PC),Rn
                nibbleClass[1] = "n";
                nibbleClass[2] = "d";
                nibbleClass[3] = "d";
                n = nibble[1];
                d = (nibble[2]<<4) + nibble[3];
                d=(0x000000FF & (long)d)<<2;
                rtn.instruction = "mov.l @(h'[d]" + rHex(d) + "[/d],[reg]pc[/reg]),[n]r" + rDec(n) + "[/n]";
                rtn.hint = "Move Immediate Data";
                rtn.result = "@h'" + rHex((pc&0xFFFFFFFC)+d);
                break;
            case b1110: //1110nnnniiiiiiii :  MOV #imm,Rn
                nibbleClass[1] = "n";
                nibbleClass[2] = "i";
                nibbleClass[3] = "i";
                n = nibble[1];
                i = (nibble[2]<<4) + nibble[3];
                if ((i&0x80)==0) i = (0x000000FF & (long)i); else i = (0xFFFFFF00 | (long)i);
                rtn.instruction = "mov #h'[i]" + rHex(i) + "[/i],[n]r" + rDec(n) + "[/n]";
                rtn.hint = "Move Immediate Data";
                break;
        }
        //Prepare html strings
        //value
            for(int i = 0; i <= 3 ; i++)
            {
                if(!nibbleClass[i].isEmpty())
                {
                    rtn.value += "<span class=\"" + nibbleClass[i] + "\">"
                            + QString::number(nibble[i],16).right(1)
                            + "</span>";
                }
                else
                    rtn.value += QString::number(nibble[i],16).right(1);
            }

        //Mnemonic
            QRegExp regex("^([^ ]+) ");
            rtn.instruction = rtn.instruction.replace(regex, "<span class=\"mnemonic\">\\1</span> ");
            QRegExp regex3("^([^ ]+)$");
            rtn.instruction = rtn.instruction.replace(regex3, "<span class=\"mnemonic\">\\1</span>");
        //Classes css
            QRegExp regex2("\\[([^\\[\\]\\/]+)\\]([^\\[\\]]+)\\[\\/([^\\[\\]\\/]+)\\]");//   ([^\\[\\[]]+)\\[\\/([^\\[\\[]]+)\\]
            rtn.instruction = rtn.instruction.replace(regex2, "<span class=\"\\1\">\\2</span>");
            rtn.result = rtn.result.replace(regex2, "<span class=\"\\1\">\\2</span>");
        //add CSS
            QString css = QString() + "<html><head><style>.mnemonic{color:#026e46;}"
                    + "body{color:#000;}"
                    + ".n{color:#2975f9;}"
                    + ".m{color:#03318d;}"
                    + ".d{color:#6d6afc;}"
                    + ".reg{color:#d6a005;}"
                    + ".i{color:red;}</style></head><body>";
            rtn.instruction = css + rtn.instruction + "</body></html>";
            rtn.result = rtn.result;
            rtn.value = css + rtn.value + "</body></html>";
    return rtn;
}

QString Disassembler::rHexNeg(int32_t value)
{
    if(value < 0)
        return ("-" + rHex(-value));
    else
        return rHex(value);
}
