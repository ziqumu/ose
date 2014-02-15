#include "Disassembler.h"

#define numHex(num,nbrNibble) QString("%1").arg((long)num, nbrNibble, 16, QChar('0')).right(8)
#define rHex(num) QString::number((long)num, 16).right(8)
#define rDec(num) QString::number((long)num, 10)


Disassembler::Disassembler(Core* core, bool getBetterResults)
{
   this->core = core;
   this->getBetterResults = getBetterResults;
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
     //Get Better Result
        Core* core = NULL;
        if(getBetterResults)
        {
            core = this->core;
        }
    //temp
        uint8_t n;
        uint8_t m;
        uint8_t d;
    //Cut in nibble (4 bits)
        uint8_t nibble[4];
        nibble[0] = (value >> 12)&0xf;
        nibble[1] = (value >> 8)&0xf;
        nibble[2] = (value >> 4)&0xf;
        nibble[3] = (value)&0xf;
        QString temp;

        /*List of classes css. Do not put h' inside thes tags
         *
         * [mnemonic][/mnemonic] : the mnemonic (added automatically)
         * [n][/n] : the n parameter
         * [m][/m] : the m parameter
         * [d][/d] : the d (disp, displacement) parameter
         * [reg][/reg] : register (if there is no [n] or [m]) (T bit too)
         * [res][/res] : result of calcul
         *
         * When the instruction change the value of something, in the result I use -> ( "=" is used for calcul)
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
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getSr()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0001: //0000nnnn00010010 : STC GBR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]gbr[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getGbr()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0010: //0000nnnn00100010 : STC VBR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]vbr[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getVbr()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0011: //0000nnnn00110010 : STC SSR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]ssr[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getSsr()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b0100: //0000nnnn01000010 : STC SPC,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]spc[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getSpc()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1000: //0000nnnn10000010 : STC R0_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r0_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(0)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1001: //0000nnnn10010010 : STC R1_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r1_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(1)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1010: //0000nnnn10100010 : STC R2_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r2_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(2)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1011: //0000nnnn10110010 : STC R3_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r3_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(3)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1100: //0000nnnn11000010 : STC R4_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r4_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(4)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1101: //0000nnnn11010010 : STC R5_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r5_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(5)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1110: //0000nnnn11100010 : STC R6_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r6_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(6)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                            case b1111: //0000nnnn11110010 : STC R7_BANK,Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc [reg]r7_bank[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(7)) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store Control Register";
                                break;
                        } break;
                    case b0011:
                        switch(nibble[2]){
                            case b0000: //0000nnnn00000011 : BSRF Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "bsrf [n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "Jump to (h'[n]" + rHex(core->getR(n)) + "[/n]+h'[reg]" + rHex(pc)
                                            + "[/reg])=h'[res]" + rHex(core->getR(n) + pc) + "[/res]";
                                rtn.hint = "Branch Subroutine Far | delayed";
                                break;
                            case b0010: //0000nnnn00100011 : BRAF Rm
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "braf [n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "Jump to (h'[n]" + rHex(core->getR(n)) + "[/n]+h'[reg]" + rHex(pc)
                                            + "[/reg])=h'[res]" + rHex(core->getR(n) + pc) + "[/res]";
                                rtn.hint = "Branch Far | delayed";
                                break;
                                case b1000: //0000nnnn10000011 : PREF @Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "pref @[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result += "h'[n]" + rHex(core->getR(n)) + "[/n]";
                                rtn.hint = "Prefetch Data to the Cache | Not emulated";
                                break;
                        }break;
                    case b0100: //0000nnnnmmmm0100 : MOV.B Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        if(core != NULL)
                            rtn.result = "h'[m]" + rHex(0xff & core->getR(m)) + "[/m] -> @(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[n]" + rHex(core->getR(n))
                                    + "[/n])=@h'[res]" + rHex(core->getR(n) + core->getR(0)) + "[/res]";
                        rtn.hint = "Move Data";
                        break;
                    case b0101: //0000nnnnmmmm0101 : MOV.W Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        if(core != NULL)
                            rtn.result = "h'[m]" + rHex(0xffff & core->getR(m)) + "[/m] -> @(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[n]" + rHex(core->getR(n))
                                    + "[/n])=@h'[res]" + rHex(core->getR(n) + core->getR(0)) + "[/res]";
                        rtn.hint = "Move Data";
                        break;
                    case b0110: //0000nnnnmmmm0110 : MOV.L Rm,@(R0,Rn)
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@([reg]r0[/reg],[n]r" + rDec(n) + "[/n])";
                        if(core != NULL)
                            rtn.result = "h'[m]" + rHex(core->getR(m)) + "[/m] -> @(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[n]" + rHex(core->getR(n))
                                    + "[/n])=@h'[res]" + rHex(core->getR(n) + core->getR(0)) + "[/res]";
                        rtn.hint = "Move Data";
                        break;
                    case b0111: //0000nnnnmmmm0111 : MUL.L Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mul.l [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL)
                            rtn.result = "h'[m]" + rHex(core->getR(m)) + "[/m]*h'[n]" + rHex(core->getR(n)) + "[/n]=h'[res]" + rHex(core->getR(m)*core->getR(n)) + "[/res] -> [reg]macl[/reg]";
                        rtn.hint = "Multiply Long : macl=r" + rDec(n) + "*r" + rDec(m);
                        break;
                    case b1000:
                        if(nibble[1] == 0)
                        {
                            switch(nibble[2]) {
                                case b0000: //0000000000001000 : CLRT
                                    rtn.instruction = "clrt";
                                    rtn.result = "0 -> [reg]t[/reg]";
                                    rtn.hint = "Clear T Bit";
                                    break;
                                case b0001: //0000000000011000 : SETT
                                    rtn.instruction = "sett";
                                    rtn.result = "1 -> [reg]t[/reg]";
                                    rtn.hint = "Set T Bit";
                                    break;
                                case b0010: //0000000000101000 : CLRMAC
                                    rtn.instruction = "clrmac";
                                    rtn.result = "0 -> [reg]mach[/reg] ; 0 -> [reg]macl[/reg]";
                                    rtn.hint = "Clear MAC Register";
                                    break;
                                case b0011: //0000000000111000 : LDTLB
                                    rtn.instruction = "ldtlb";
                                    rtn.hint = "Load PTEH/PTEL to TLB | Not emulated";
                                    break;
                                case b0100: //0000000001001000 : CLRS
                                    rtn.instruction = "clrs";
                                    rtn.result = "0 -> [reg]s[/reg]";
                                    rtn.hint = "Clear S Bit";
                                    break;
                                case b0101: //0000000001011000 : SETS
                                    rtn.instruction = "sets";
                                    rtn.result = "1 -> [reg]s[/reg]";
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
                                    rtn.result = "0 -> [reg]M[/reg] ; 0 -> [reg]Q[/reg] ; 0 -> [reg]T[/reg]";
                                    rtn.hint = "Divide Step 0 as Unsigned";
                                }
                                break;
                            case b0010: //0000nnnn00101001 : MOVT Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "movt [n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "[reg]" + rDec(core->getT()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Move T Bit";
                                break;
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                            case b0000: //0000nnnn00001010 : STS MACH,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]mach[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    uint32_t t = core->getMach();
                                    if ((t&0x00000200)==0) t&=0x000003FF; else t|=0xFFFFFC00;
                                    rtn.result = "h'[reg]" + rHex(t) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Store System Register";
                                break;
                            case b0001: //0000nnnn00011010 : STS MACL,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]macl[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getMacl()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store System Register";
                                break;
                            case b0010: //0000nnnn00101010 : STS PR,Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts [reg]pr[/reg],[n]r" + rDec(n) + "[/n]";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getPr()) + "[/reg] -> [n]r" + rDec(n) + "[/n]";
                                rtn.hint = "Store System Register";
                                break;
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: //0000000000001011 : RTS
                                rtn.instruction = "rts";
                                if(core != NULL)
                                    rtn.result = "h'[reg]" + rHex(core->getPr()+4) + "[/reg]";
                                rtn.hint = "Return from Subroutine";
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
                                    rtn.hint = "Return from Exception | Delay slot";
                                }
                                break;
                        } break;
                    case b1100: //0000nnnnmmmm1100 : MOV.B @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            try{
                                uint32_t tmp = core->Read_Byte(core->getR(m)+core->getR(0));
                                if ((tmp&0x80)==0) tmp&=0x000000FF; else tmp|=0xFFFFFF00;
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res])=h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                            }
                            catch(std::exception & e){
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res]) -> [n]r" + rDec(n) + "[/n]";
                            }

                        }
                        rtn.hint = "Move Data";
                        break;
                    case b1101: //0000nnnnmmmm1101 : MOV.W @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            try{
                                uint32_t tmp = core->Read_Word(core->getR(m)+core->getR(0));
                                if ((tmp&0x8000)==0) tmp&=0x0000FFFF; else tmp|=0xFFFF0000;
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res])=h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                            }catch(std::exception & e){
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res]) -> [n]r" + rDec(n) + "[/n]";
                            }
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b1110: //0000nnnnmmmm1110 : MOV.L @(R0,Rm),Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l @([reg]r0[/reg],[m]r" + rDec(m) + "[/m]),[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            try{
                                uint32_t tmp = core->Read_Long(core->getR(m)+core->getR(0));
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res])=h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                            }catch(std::exception & e){
                                rtn.result = "@(h'[reg]" + rHex(core->getR(0)) + "[/reg]+h'[m]" + rHex(core->getR(m)) + "[/m])=@(h'[res]"
                                        + rHex(core->getR(m)+core->getR(0)) + "[/res]) -> [n]r" + rDec(n) + "[/n]";
                            }
                        }
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
                if(core != NULL){
                    rtn.result = "h'[m]" + rHex(core->getR(m)) + "[/m] -> @(h'[d]" + rHex(d) + "[/d]*4+h'[n]" + rHex(core->getR(n)) + "[/n])=@(h'[res]"
                            + rHex(core->getR(n)+d*4) + "[/res])";
                }
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
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(0xff & core->getR(m)) + "[/m] -> @[n]h'" + rHex(core->getR(n)) + "[/n]";
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b0001: //0010nnnnmmmm0001 : MOV.W Rm,@Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(0xffff & core->getR(m)) + "[/m] -> @[n]h'" + rHex(core->getR(n)) + "[/n]";
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b0010: //0010nnnnmmmm0010 : MOV.L Rm,@Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(core->getR(m)) + "[/m] -> @[n]h'" + rHex(core->getR(n)) + "[/n]";
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b0100: //0010nnnnmmmm0100 : MOV.B Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.b [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(0xff & core->getR(m)) + "[/m] -> @([n]r" + rDec(n) + "[/n]-1)=@[n]h'" + rHex(core->getR(n)-1) + "[/n]";
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b0101: //0010nnnnmmmm0101 : MOV.W Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.w [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(0xffff & core->getR(m)) + "[/m] -> @([n]r" + rDec(n) + "[/n]-2)=@[n]h'" + rHex(core->getR(n)-2) + "[/n]";
                        }
                        rtn.hint = "Move Data";
                        break;
                    case b0110: //0010nnnnmmmm0110 : MOV.L Rm,@-Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mov.l [m]r" + rDec(m) + "[/m],@-[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[m]" + rHex(core->getR(m)) + "[/m] -> @([n]r" + rDec(n) + "[/n]-4)=@[n]h'" + rHex(core->getR(n)-4) + "[/n]";
                        }
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
                        if(core != NULL){
                            QString T = "0";
                            if((core->getR(m)&core->getR(n))==0) T="1";
                            rtn.result = "!(h'[m]" + rHex(core->getR(m)) + "[/m]&h'[n]" + rHex(core->getR(n)) + "[/n])=![res]" + rHex(core->getR(m)&core->getR(n)) + "[/res]=[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Test Logical";
                        break;
                    case b1001: //0010nnnnmmmm1001 : AND Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "and [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "(h'[m]" + rHex(core->getR(m)) + "[/m]&h'[n]" + rHex(core->getR(n)) + "[/n])=[res]" + rHex(core->getR(m)&core->getR(n)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
                        rtn.hint = "AND Logical";
                        break;
                    case b1010: //0010nnnnmmmm1010 : XOR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "xor [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "(h'[m]" + rHex(core->getR(m)) + "[/m]^h'[n]" + rHex(core->getR(n)) + "[/n])=[res]" + rHex(core->getR(m)^core->getR(n)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
                        rtn.hint = "Exclusive OR Logical";
                        break;
                    case b1011: //0010nnnnmmmm1011 : OR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "or [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "(h'[m]" + rHex(core->getR(m)) + "[/m]|h'[n]" + rHex(core->getR(n)) + "[/n])=[res]" + rHex(core->getR(m)|core->getR(n)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
                        rtn.hint = "OR Logical";
                        break;
                  case b1100: //0010nnnnmmmm1100 : CMP/STR Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/str [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            unsigned long temp;
                            long HH,HL,LH,LL;
                            QString T = "0";
                            temp=core->getR(n)^core->getR(m);
                            HH=(temp&0xFF000000)>>12;
                            HL=(temp&0x00FF0000)>>8;
                            LH=(temp&0x0000FF00)>>4;
                            LL=temp&0x000000FF;
                            HH=HH&&HL&&LH&&LL;
                            if (HH==0) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Compare Conditionally | When a byte in R" + rDec(n) + " equals a byte in R" + rDec(m) + ", 1 -> T";
                        break;
                    case b1101: //0010nnnnmmmm1101 : XTRCT Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "xtrct [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[res]" + rHex(((core->getR(n)>>16)&0x0000FFFF)|((core->getR(m)<<16)&0xFFFF0000)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
                        rtn.hint = "Extract";
                        break;
                    case b1110: //0010nnnnmmmm1110 : MULU Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "mulu [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[res]" + rHex(((unsigned long)(unsigned short)core->getR(n)*(unsigned long)(unsigned short)core->getR(m))) + "[/res] -> [reg]macl[/reg]";
                        }
                        rtn.hint = "Multiply as Unsigned Word";
                        break;
                    case b1111: //0010nnnnmmmm1111 : MULS Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "muls [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[res]" + rHex(((long)(short)core->getR(n)*(long)(short)core->getR(m))) + "[/res] -> [reg]macl[/reg]";
                        }
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
                        if(core != NULL){
                            QString T = "0";
                            if (core->getR(n)==core->getR(m)) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Compare Conditionally | Equality";
                        break;
                    case b0010: //0011nnnnmmmm0010 : CMP_HS Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/hs [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = "0";
                            if((unsigned long)core->getR(n)>=(unsigned long)core->getR(m)) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Compare Conditionally | Unsigned Greater or Equal";
                        break;
                    case b0011: //0011nnnnmmmm0011 : CMP_GE Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/ge [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = "0";
                            if((long)core->getR(n)>=(long)core->getR(m)) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
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
                    case b0110: //0011nnnnmmmm0110 : CMP_HI Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/hi [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = "0";
                            if((unsigned long)core->getR(n)>(unsigned long)core->getR(m)) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Compare Conditionally | Unsigned Greater";
                        break;
                    case b0111: //0011nnnnmmmm0111 : CMP_GT Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "cmp/gt [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = "0";
                            if((long)core->getR(n)>(long)core->getR(m)) T="1";
                            rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Compare Conditionally | Signed Greater";
                        break;
                    case b1000: //0011nnnnmmmm1000 : SUB Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "sub [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[res]" + rHex(core->getR(n)-core->getR(m)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
                        rtn.hint = "Subtract Binary (r" + rDec(n) + " - r" + rDec(m) + ")";
                        break;
                    case b1010: //0011nnnnmmmm1010 : SUBC Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "subc [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = QString::number((int)core->getT());
                            unsigned long tmp0,tmp1;
                            tmp1=core->getR(n) - core->getR(m);
                            tmp0=core->getR(n);
                            if (tmp0<tmp1) T="1";
                            if (tmp1<(tmp1 - core->getT())) T="1";
                            rtn.result = "h'[res]" + rHex(tmp1 - core->getT()) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Subtract with Carry (r" + rDec(n) + " - r" + rDec(m) + ") | Used for subtraction of data that has more than 32 bits.";
                        break;
                    case b1011: //0011nnnnmmmm1011 : SUBV Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "subv [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = QString::number((int)core->getT());
                            long dest,src,ans;
                            if ((long)core->getR(n)>=0) dest=0;
                            else dest=1;
                            if ((long)core->getR(m)>=0) src=0;
                            else src=1;
                            src+=dest;
                            if ((long)(core->getR(n) - core->getR(m))>=0) ans=0;
                            else ans=1;
                            ans+=dest;
                            if (src==1) {
                            if (ans==1) T="1";
                            else T="0";
                            }
                            else T="0";
                            rtn.result = "h'[res]" + rHex(core->getR(n) - core->getR(m)) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Subtract with V Flag Underflow Check (r" + rDec(n) + " - r" + rDec(m) + ")";
                        break;
                    case b1100: //0011nnnnmmmm1100 : ADD Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "add [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            rtn.result = "h'[res]" + rHex(core->getR(n)+core->getR(m)) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                        }
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
                        if(core != NULL){
                            QString T = QString::number((int)core->getT());
                            unsigned long tmp0,tmp1;
                            tmp1=core->getR(n) + core->getR(m);
                            tmp0=core->getR(n);
                            if (tmp0>tmp1) T="1";
                            if (tmp1>(tmp1 - core->getT())) T="1";
                            rtn.result = "h'[res]" + rHex(tmp1 + core->getT()) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                        }
                        rtn.hint = "Add with Carry | Used for addition of data that has more than 32 bits.";
                        break;
                    case b1111: //0011nnnnmmmm1111 : ADDV Rm,Rn
                        nibbleClass[1] = "n";
                        n = nibble[1];
                        nibbleClass[2] = "m";
                        m = nibble[2];
                        rtn.instruction = "addv [m]r" + rDec(m) + "[/m],[n]r" + rDec(n) + "[/n]";
                        if(core != NULL){
                            QString T = QString::number((int)core->getT());
                            long dest,src,ans;
                            if ((long)core->getR(n)>=0) dest=0;
                            else dest=1;
                            if ((long)core->getR(m)>=0) src=0;
                            else src=1;
                            src+=dest;
                            if ((long)(core->getR(n) + core->getR(m))>=0) ans=0;
                            else ans=1;
                            ans+=dest;
                            if (src==0 || src==2) {
                            if (ans==1) T="1";
                            else T="0";
                            }
                            else T="0";
                            rtn.result = "h'[res]" + rHex(core->getR(n) - core->getR(m)) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                        }
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
                                if(core != NULL){
                                    QString T = "1";
                                    if ((core->getR(n)&0x80000000)==0) T="0";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg] ; h'[res]" + rHex(core->getR(n)<<1) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Shift Logical Left | Put the shifted bit in T";
                                break;
                            case b0001: //0100nnnn00010000 : DT Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "dt [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "0";
                                    if ((core->getR(n)-1)==0) T="1";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg] ; h'[res]" + rHex(core->getR(n)-1) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Decrement and Test";
                                break;
                            case b0010: //0100nnnn00100000 : SHAL Rn(Same as SHLL)
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shal [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "1";
                                    if ((core->getR(n)&0x80000000)==0) T="0";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg] ; h'[res]" + rHex(core->getR(n)<<1) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Shift Arithmetic Left | Put the shifted bit in T";
                                break;
                        } break;
                    case b0001:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000001 : SHLR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shlr [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "1";
                                    if ((core->getR(n)&0x00000001)==0) T="0";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg] ; h'[res]" + rHex((core->getR(n)>>1)&0x7FFFFFFF) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Shift Logical Right | Put the shifted bit in T";
                                break;
                            case b0001: //0100nnnn00010001 : CMP_PZ Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "cmp/pz [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "0";
                                    if ((long)core->getR(n)>=0) T="1";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Compare Conditionally | Greater than or equal to 0";
                                break;
                            case b0010: //0100nnnn00100001 : SHAR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "shar [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "1";
                                    if ((core->getR(n)&0x00000001)==0) T="0";
                                    long temp, temprn;
                                    if ((core->getR(n)&0x80000000)==0) temp=0; else temp=1;
                                    temprn = core->getR(n) >> 1;
                                    if (temp==1) temprn|=0x80000000;
                                    else temprn&=0x7FFFFFFF;
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg] ; h'[res]" + rHex(temprn) + "[/res] -> [n]r" + rDec(n) + "[/n]";
                                }
                                rtn.hint = "Shift Arithmetic Right | Put the shifted bit in T";
                                break;
                        } break;
                    case b0010:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000010 : STS.L MACH,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]mach[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    ulong temp;
                                    if ((core->getMach()&0x00000200)==0) temp = core->getMach()&0x000003FF;
                                    else  temp = core->getMach()|0xFFFFFC00;
                                    rtn.result = "h'[reg]" + rHex(temp) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "System Control Instruction";
                                break;
                            case b0001: //0100nnnn00010010 : STS.L MACL,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]macl[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getMacl()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "System Control Instruction";
                                break;
                            case b0010: //0100nnnn00100010 : STS.L PR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "sts.l [reg]pr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getPr()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "System Control Instruction";
                                break;
                         }break;
                    case b0011:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000011 : STC.L SR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]sr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getSr()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b0001: //0100nnnn00010011 : STC.L GBR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]gbr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getGbr()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b0010: //0100nnnn00100011 : STC.L VBR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]vbr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getVbr()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b0011: //0100nnnn00110011 : STC.L SSR,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]ssr[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getSsr()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b0100: //0100nnnn01000011 : STC.L SPC,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]spc[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getSpc()) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1000: //0100nnnn10000011 : STC.L R0_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r0_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(0)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1001: //0100nnnn10010011 : STC.L R1_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r1_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(1)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1010: //0100nnnn10100011 : STC.L R2_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r2_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(2)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1011: //0100nnnn10110011 : STC.L R3_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r3_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(3)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1100: //0100nnnn11000011 : STC.L R4_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r4_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(4)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1101: //0100nnnn11010011 : STC.L R5_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r5_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(5)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1110: //0100nnnn11100011 : STC.L R6_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r6_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(6)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                            case b1111: //0100nnnn11110011 : STC.L R7_BANK,@-Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "stc.l [reg]r7_bank[/reg],@-[n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    rtn.result = "h'[reg]" + rHex(core->getRn_bank(7)) + "[/reg] -> @([n]r" + rDec(n) + "[/n]-4)=@h'[n]" + rHex(core->getR(n)-4) + "[/n]";
                                }
                                rtn.hint = "Store Control Register";
                                break;
                        } break;
                    case b0100:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000100 : ROTL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotl [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "1";
                                    uint32_t tmp = core->getR(n)<<1;
                                    if ((core->getR(n)&0x80000000)==0) T="0";
                                    if (T=="1") tmp|=0x00000001;
                                    else tmp&=0xFFFFFFFE;
                                    rtn.result = "h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Rotate Left";
                                break;
                            case b0010: //0100nnnn00100100 : ROTCL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotcl [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "0";
                                    if ((core->getR(n)&0x80000000)==0) temp=0;
                                    else temp=1;
                                    uint32_t tmp = core->getR(n)<<1;
                                    if (T=="1") tmp|=0x00000001;
                                    else tmp&=0xFFFFFFFE;
                                    if (temp==1) T="1";
                                    rtn.result = "h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Rotate with Carry Left";
                                break;
                        } break;
                    case b0101:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000101 : ROTR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotr [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "1";
                                    uint32_t tmp = core->getR(n)>>1;
                                    if (T=="1") tmp|=0x80000000;
                                    else tmp&=0x7FFFFFFF;
                                    rtn.result = "h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Rotate Right";
                                break;
                            case b0001: //0100nnnn00010101 : CMP_PL Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "cmp/pl [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "0";
                                    if ((long)core->getR(n)>0) T="1";
                                    rtn.result = "[res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Compare Conditionally | Greater than 0";
                                break;
                            case b0010: //0100nnnn00100101 : ROTCR Rn
                                nibbleClass[1] = "n";
                                n = nibble[1];
                                rtn.instruction = "rotcr [n]r" + rDec(n) + "[/n]";
                                if(core != NULL){
                                    QString T = "0";
                                    if ((core->getR(n)&0x00000001)==0) temp=0;
                                    else temp=1;
                                    uint32_t tmp = core->getR(n)>>1;
                                    if (T=="1") tmp|=0x80000000;
                                    else tmp&=0x7FFFFFFF;
                                    if (temp==1) T="1";
                                    rtn.result = "h'[res]" + rHex(tmp) + "[/res] -> [n]r" + rDec(n) + "[/n] ; [res]" + T + "[/res] -> [reg]T[/reg]";
                                }
                                rtn.hint = "Rotate with Carry Right";
                                break;
                        } break;
                    case b0110:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00000110 : LDS.L @Rm+,MACH
                                nibbleClass[1] = "m";
                                m = nibble[1];
                                rtn.instruction = "lds.l @[m]r" + rDec(m) + "[/m]+,[reg]mach[/reg]";
                                if(core != NULL)
                                    uint32_t tmp = core->Read_Long(core->getR(m));
                                    if ((tmp&0x00000200)==0) tmp&=0x000003FF;
                                    else tmp|=0xFFFFFC00;
                                    rtn.result = "h'[m]" + rHex(tmp) + "[/m] -> [reg]mach[/reg]";
                                rtn.hint = "Store Control Register";
                                break;

                                m = nibble[1];
                                MACH=Read_Long(R[m]);
                                if ((MACH&0x00000200)==0) MACH&=0x000003FF;
                                else MACH|=0xFFFFFC00;
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100mmmm00010110 : DS.L @Rm+,MACL
                                m = nibble[1];
                                MACL=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100mmmm00100110 : LDS.L @Rm+,PR
                                m = nibble[1];
                                PR=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0111:
                        switch(nibble[2]){
                            case b0000: //0100mmmm00000111 : LDC.L @Rm+,SR
                                m = nibble[1];
                                SR=Read_Long(R[m])&0x0FFF0FFF;
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100mmmm00010111 : LDC.L @Rm+,GBR
                                m = nibble[1];
                                GBR=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100mmmm00100111 : LDC.L @Rm+,VBR
                                m = nibble[1];
                                VBR=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0011: //0100mmmm00110111 : LDC.L @Rm+,SSR
                                m = nibble[1];
                                SSR=Read_Long(R[m])&0x700003F3;
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b0100: //0100mmmm01000111 : LDC.L @Rm+,SPC
                                m = nibble[1];
                                SPC=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1000: //0100mmmm10000111 : LDC.L @Rm+,R0_BANK
                                m = nibble[1];
                                R0_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1001: //0100mmmm10010111 : LDC.L @Rm+,R1_BANK
                                m = nibble[1];
                                R1_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1010: //0100mmmm10100111 : LDC.L @Rm+,R2_BANK
                                m = nibble[1];
                                R2_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1011: //0100mmmm10110111 : LDC.L @Rm+,R3_BANK
                                m = nibble[1];
                                R3_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1100: //0100mmmm11000111 : LDC.L @Rm+,R4_BANK
                                m = nibble[1];
                                R4_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1101: //0100mmmm11010111 : LDC.L @Rm+,R5_BANK
                                m = nibble[1];
                                R5_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1110: //0100mmmm11100111 : LDC.L @Rm+,R6_BANK
                                m = nibble[1];
                                R6_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                            case b1111: //0100mmmm11110111 : LDC.L @Rm+,R7_BANK
                                m = nibble[1];
                                R7_BANK=Read_Long(R[m]);
                                RS=Read_Long(R[m]);
                                R[m]+=4;
                                PC+=2;
                                done = true;
                                break;
                        } break;








                    case b0110:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "lds.l  @r" + convertInt(nibble[1]) + "+, mach";	break; //0100mmmm00000110
                            case b0001: rtn.instruction = "lds.l  @r" + convertInt(nibble[1]) + "+, macl";	break; //0100mmmm00010110
                            case b0010: rtn.instruction = "lds.l  @r" + convertInt(nibble[1]) + "+, pr";	break; //0100mmmm00100110
                        } break;
                    case b0111:
                        switch(nibble[2]){
                            case b0000: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, sr"; break; //0100mmmm00000111
                            case b0001: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, gbr"; break; //0100mmmm00010111
                            case b0010: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, vbr"; break; //0100mmmm00100111
                            case b0011: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, ssr"; break; //0100mmmm00110111
                            case b0100: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, spc"; break; //0100mmmm01000111
                            case b1000: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r0_bank"; break; //0100mmmm10000111
                            case b1001: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r1_bank"; break; //0100mmmm10010111
                            case b1010: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r2_bank"; break; //0100mmmm10100111
                            case b1011: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r3_bank"; break; //0100mmmm10110111
                            case b1100: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r4_bank"; break; //0100mmmm11000111
                            case b1101: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r5_bank"; break; //0100mmmm11010111
                            case b1110: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r6_bank"; break; //0100mmmm11100111
                            case b1111: rtn.instruction = "ldc.l  @r" + convertInt(nibble[1]) + "+, r7_bank"; break; //0100mmmm11110111
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "lds  r" + convertInt(nibble[1]) + ", mach"; break; //0100mmmm00001010
                            case b0001: rtn.instruction = "lds  r" + convertInt(nibble[1]) + ", macl"; break; //0100mmmm00011010
                            case b0010: rtn.instruction = "lds  r" + convertInt(nibble[1]) + ", pr"; break; //0100mmmm00101010
                        } break;
                    case b1110:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", sr"; break; //0100mmmm00001110
                            case b0001: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", gbr"; break; //0100mmmm00011110
                            case b0010: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", vbr"; break; //0100mmmm00101110
                            case b0011: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", ssr"; break; //0100mmmm00111110
                            case b0100: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", spc"; break; //0100mmmm01001110
                            case b1000: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r0_bank"; break; //0100mmmm10001110
                            case b1001: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r1_bank"; break; //0100mmmm10011110
                            case b1010: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r2_bank"; break; //0100mmmm10101110
                            case b1011: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r3_bank"; break; //0100mmmm10111110
                            case b1100: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r4_bank"; break; //0100mmmm11001110
                            case b1101: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r5_bank"; break; //0100mmmm11011110
                            case b1110: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r6_bank"; break; //0100mmmm11101110
                            case b1111: rtn.instruction = "ldc  r" + convertInt(nibble[1]) + ", r7_bank"; break; //0100mmmm11111110
                        } break;
                    case b0100:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "rotl  r" + convertInt(nibble[1]); break; //0100nnnn00000100
                            case b0010: rtn.instruction = "rotcl  r" + convertInt(nibble[1]); break; //0100nnnn00100100
                        } break;
                    case b0101:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "rotr  r" + convertInt(nibble[1]); break; //0100nnnn00000101
                            case b0001: rtn.instruction = "cmp/pl  r" + convertInt(nibble[1]); break; //0100nnnn00010101
                            case b0010: rtn.instruction = "rotcr  r" + convertInt(nibble[1]); break; //0100nnnn00100101
                        } break;
                    case b1000:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "shll2  r" + convertInt(nibble[1]); break; //0100nnnn00001000
                            case b0001: rtn.instruction = "shll8  r" + convertInt(nibble[1]); break; //0100nnnn00011000
                            case b0010: rtn.instruction = "shll16  r" + convertInt(nibble[1]); break; //0100nnnn00101000
                        } break;
                    case b1001:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "shlr2  r" + convertInt(nibble[1]); break; //0100nnnn00001001
                            case b0001: rtn.instruction = "shlr8  r" + convertInt(nibble[1]); break; //0100nnnn00011001
                            case b0010: rtn.instruction = "shlr16  r" + convertInt(nibble[1]); break; //0100nnnn00101001
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: rtn.instruction = "jsr  @r" + convertInt(nibble[1]); break; //0100nnnn00001011
                            case b0001: rtn.instruction = "tas.b  @r" + convertInt(nibble[1]); break; //0100nnnn00011011
                            case b0010: rtn.instruction = "jmp  @r" + convertInt(nibble[1]);	break;
                        } break;
                    case b1100: rtn.instruction = "shad  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0100nnnnmmmm1100
                    case b1101: rtn.instruction = "shld  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0100nnnnmmmm1101
                    case b1111: rtn.instruction = "mac.w  @r" + convertInt(nibble[2]) + "+, @r" + convertInt(nibble[1]) + "+"; break; //0100nnnnmmmm1111
                } break;
            case b0101: rtn.instruction = "mov.l  @(h'" + convertHexNibbleU(nibble[3], 4) + ",r" + convertInt(nibble[2]) + "), r" + convertInt(nibble[1]); break; //0101nnnnmmmmdddd
            case b0110:
                switch(nibble[3]) {
                    case b0000: rtn.instruction = "mov.b  @r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm0000
                    case b0001: rtn.instruction = "mov.w  @r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm0001
                    case b0010: rtn.instruction = "mov.l  @r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm0010

                    case b0011: rtn.instruction = "mov  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm0011
                    case b0100: rtn.instruction = "mov.b  @r" + convertInt(nibble[2]) + "+, r" + convertInt(nibble[1]); break; //0110nnnnmmmm0100
                    case b0101: rtn.instruction = "mov.w  @r" + convertInt(nibble[2]) + "+, r" + convertInt(nibble[1]); break; //0110nnnnmmmm0101
                    case b0110: rtn.instruction = "mov.l  @r" + convertInt(nibble[2]) + "+, r" + convertInt(nibble[1]); break; //0110nnnnmmmm0110
                    case b0111: rtn.instruction = "not  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm0111
                    case b1000: rtn.instruction = "swap.b  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1000
                    case b1001: rtn.instruction = "swap.w  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1001
                    case b1010: rtn.instruction = "negc  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1010
                    case b1011: rtn.instruction = "neg  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1011
                    case b1100: rtn.instruction = "extu.b  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1100
                    case b1101: rtn.instruction = "extu.w  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1101
                    case b1110: rtn.instruction = "exts.b  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1110
                    case b1111: rtn.instruction = "exts.w  r" + convertInt(nibble[2]) + ", r" + convertInt(nibble[1]); break; //0110nnnnmmmm1111
                } break;
            case b0111:
                    rtn.instruction = "add  #h'" + convertInt(((nibble[2]<<4)+nibble[3]), 16) + ", r" + convertInt(nibble[1]);
                break; //0111nnnniiiiiiii
            case b1000:
                switch(nibble[1]) {
                    case b0000: rtn.instruction = "mov.b  r0, @(h'" + convertHexNibbleU(nibble[3]) + ",r" + convertInt(nibble[2]) + ")"; break; //10000000nnnndddd
                    case b0001: rtn.instruction = "mov.w  r0, @(h'" + convertHexNibbleU(nibble[3], 2) + ",r" + convertInt(nibble[2]) + ")"; break; //10000001nnnndddd
                    case b0100: rtn.instruction = "mov.b  @(h'" + convertHexNibbleU(nibble[3]) + ",r" + convertInt(nibble[2]) + "), r0"; break; //10000100mmmmdddd
                    case b0101: rtn.instruction = "mov.w  @(h'" + convertHexNibbleU(nibble[3], 2) + ",r" + convertInt(nibble[2]) + "), r0"; break; //10000101mmmmdddd
                    case b1000: rtn.instruction = "cmp/eq  #h'" + convertHex8NibbleU(((nibble[2]<<4)+nibble[3])) + ", r0"; break; //10001000iiiiiiii
                    case b1001: rtn.instruction = "bt  h'" + convertHex2NibbleS(((nibble[2]<<4)+nibble[3]),2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt2NibbleS((nibble[2]<<4)+nibble[3])) + ")" ; break; //10001001dddddddd
                    case b1011: rtn.instruction = "bf  h'" + convertHex2NibbleS(((nibble[2]<<4)+nibble[3]),2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt2NibbleS((nibble[2]<<4)+nibble[3])) + ")" ; break; //10001011dddddddd
                    case b1101: rtn.instruction = "bt/s  h'" + convertHex2NibbleS(((nibble[2]<<4)+nibble[3]),2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt2NibbleS((nibble[2]<<4)+nibble[3])) + ")" ; break; //10001101dddddddd
                    case b1111: rtn.instruction = "bf/s  h'" + convertHex2NibbleS(((nibble[2]<<4)+nibble[3]),2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt2NibbleS((nibble[2]<<4)+nibble[3])) + ")" ; break; //10001111dddddddd
                } break;
            case b1001: rtn.instruction = "mov.w  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 2) + ",pc), r" + convertInt(nibble[1]); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt2NibbleU((nibble[2]<<4)+nibble[3])) + ")" ;break; //1001nnnndddddddd
            case b1010: rtn.instruction = "bra  h'" + convertHex3NibbleS((nibble[1]<<8)+(nibble[2]<<4)+nibble[3], 2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt3NibbleS((nibble[1]<<8)+(nibble[2]<<4)+nibble[3])) + ")" ; break;  //1010dddddddddddd
            case b1011: rtn.instruction = "bsr  h'" + convertHex3NibbleS((nibble[1]<<8)+(nibble[2]<<4)+nibble[3], 2); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%2) + 2*convertInt3NibbleS((nibble[1]<<8)+(nibble[2]<<4)+nibble[3])) + ")" ; break;
            case b1100:
                switch(nibble[1]) {
                    case b0000: rtn.instruction = "mov.b  r0, @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3]) + ",gbr)"; break; //11000000dddddddd
                    case b0001: rtn.instruction = "mov.w  r0, @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 2) + ",gbr)"; break; //11000001dddddddd
                    case b0010: rtn.instruction = "mov.l  r0, @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 4) + ",gbr)"; break; //11000010dddddddd
                    case b0011: rtn.instruction = "trapa  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])); break; //11000011iiiiiiii
                    case b0100: rtn.instruction = "mov.b  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3]) + ",gbr), r0"; break; //11000100dddddddd
                    case b0101: rtn.instruction = "mov.w  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 2) + ",gbr), r0"; break; //11000101dddddddd
                    case b0110: rtn.instruction = "mov.l  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 4) + ",gbr), r0"; break; //11000110dddddddd
                    case b0111: rtn.instruction = "mova  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 4) + ",pc), r0"; rtn.result = "h'" + convertHex8NibbleU(offset + 4 -(offset%4) + 4*convertInt2NibbleU((nibble[2]<<4)+nibble[3])) ; break; //11000111dddddddd
                    case b1000: rtn.instruction = "tst  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", r0"; break; //11001000iiiiiiii
                    case b1001: rtn.instruction = "and  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", r0"; break; //11001001iiiiiiii
                    case b1010: rtn.instruction = "xor  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", r0"; break; //11001010iiiiiiii
                    case b1011: rtn.instruction = "or  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", r0"; break; //11001011iiiiiiii
                    case b1100: rtn.instruction = "tst.b  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", @(r0,gbr)"; break; //11001100iiiiiiii
                    case b1101: rtn.instruction = "and.b  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", @(r0,gbr)"; break; //11001101iiiiiiii
                    case b1110: rtn.instruction = "xor.b  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", @(r0,gbr)"; break; //11001110iiiiiiii
                    case b1111: rtn.instruction = "or.b  #h'" + convertHex2NibbleU(((nibble[2]<<4)+nibble[3])) + ", @(r0,gbr)"; break; //11001111iiiiiiii
                } break;
            case b1101: rtn.instruction = "mov.l  @(h'" + convertHex2NibbleU((nibble[2]<<4)+nibble[3], 4) + ",pc), r" + convertInt(nibble[1]); rtn.result = "@(h'" + convertHex8NibbleU(offset + 4 -(offset%4) + 4*convertInt2NibbleU((nibble[2]<<4)+nibble[3])) + ")" ;break; //1101nnnndddddddd
            case b1110: rtn.instruction = "mov  #h'" + convertHex8NibbleU(((nibble[2]<<4)+nibble[3])) + ", r" + convertInt(nibble[1]); break; //1110nnnniiiiiiii
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
                    + ".res{color:pink;}</style></head><body>";
            rtn.instruction = css + rtn.instruction + "</body></html>";
            rtn.result = css + rtn.result + "</body></html>";
            rtn.value = css + rtn.value + "</body></html>";
    return rtn;
}

int32_t Disassembler::coreReg(QString string, Core *core, bool *ok)
{
    *ok = false;
    QRegExp regR("^r([0-9]{1,2})$");
    if(regR.indexIn(string) != -1)
    {
        bool ok2;
        ushort n = regR.cap(1).toUShort(&ok2);
        if(n <= 15 && ok2)
        {
            *ok = true;
            return core->getR(n);
        }
    }
    else if(string == "pc"){
        *ok = true;
        return core->getPC();
    }
    return 0;
}

QString Disassembler::coreCalls(QString string, Core *core)
{
    //Core reg calls
        QRegExp regRegex("\\[coreReg\\]([^\\[]*)\\[\\/coreReg\\]");
        int pos = 0;
        while((pos=regRegex.indexIn(string,pos)) != -1)
        {
            bool ok;
            int32_t value = coreReg(regRegex.cap(1),core,&ok);
            if(ok){
                string = string.left(pos) + "h'" + QString::number(value,16).right(8)
                        + string.right(string.length() - regRegex.matchedLength() - pos);
            }

            pos ++;
        }
   //Core calc calls
    QRegExp regex("\\[core\\]([^\\[]*)\\[\\/core\\]");
    pos = 0;
    while((pos=regex.indexIn(string,pos)) != -1)
    {
        //Sum
        QRegExp regPlus("^\\+\\(([a-z0-9]+),([a-z0-9]+)\\)$");
        if(regPlus.indexIn(regex.cap(1)) != -1)
        {
            bool ok, ok2;
            int32_t val1 = coreReg(regPlus.cap(1),core,&ok);
            int32_t val2 = coreReg(regPlus.cap(2),core,&ok2);
            if(ok && ok2)
            {
                string = string.left(pos) + "h'" + QString::number(val1 + val2,16).right(8)
                            + string.right(string.length() - regex.matchedLength() - pos);
            }
        }
        pos ++;
        qDebug() << pos;
        qDebug() << string[pos];
    }
    return string;
}


int Disassembler::convertIntMain(unsigned int number, int nbrNibble, bool Signed)
{
    number &= 0xFFFFFFFF>>4*(8-nbrNibble);
    if(Signed && number&(0x8<<((nbrNibble-1)*4))) //if negatif
    {
        number |= (0xFFFFFFFF<<4*(nbrNibble))&0xFFFFFFFF;
    }
    return (int)number;
}
int Disassembler::convertInt2NibbleS(int number){
    return convertIntMain(number, 2, true);}
int Disassembler::convertInt2NibbleU(int number){
    return convertIntMain(number, 2, false);}
int Disassembler::convertInt3NibbleS(int number){
    return convertIntMain(number, 3, true);}
int Disassembler::convertInt8NibbleS(int number){
    return convertIntMain(number, 8, true);}

QString Disassembler::convertHexMain(unsigned int number, int nbrNibble, bool Signed, int multiple)
{
    std::stringstream ss;
    if(Signed && number&(0x8<<((nbrNibble-1)*4))) //if negatif
    {
        ss << '-';
        number = (~number)+1;
    }
        number &= 0xFFFFFFFF>>4*(8-nbrNibble);
        number *= multiple;
        ss << std::hex << number;
        return (QString(ss.str().c_str()));
}

QString Disassembler::convertHexNibbleS(int number,int multiple){
    return convertHexMain(number, 1, true, multiple);}
QString Disassembler::convertHexNibbleU(int number,int multiple){
    return convertHexMain(number, 1, false, multiple);}

QString Disassembler::convertHex2NibbleS(int number,int multiple){
    return convertHexMain(number, 2, true, multiple);}
QString Disassembler::convertHex2NibbleU(int number,int multiple){
    return convertHexMain(number, 2, false, multiple);}

QString Disassembler::convertHex3NibbleS(int number,int multiple){
    return convertHexMain(number, 3, true, multiple);}
QString Disassembler::convertHex3NibbleU(int number,int multiple){
    return convertHexMain(number, 3, false, multiple);}

QString Disassembler::convertHex8NibbleS(int number,int multiple){
    return convertHexMain(number, 8, true, multiple);}
QString Disassembler::convertHex8NibbleU(int number,int multiple){
    return convertHexMain(number, 8, false, multiple);}

QString Disassembler::convertInt(int number, int base)
{
    std::stringstream ss;
    if(base == 16)
        ss << std::hex << number;
    else
        ss << number;
    return QString(ss.str().c_str());
}


QString Disassembler::getInstruction()
{
    return this->instruction;
}

QString Disassembler::getResult()
{
    return this->result;
}
QString Disassembler::getHint()
{
    return this->hint;
}

QString Disassembler::getValue()
{
    return this->value;
}
