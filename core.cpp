#include "core.h"
  
Core::Core(QString flashLink, DisplayDriver* DD)
{
    this->flash.resize(0x400000);
    this->ram.resize(512000);
    this->DD = DD;

    //Init registers (cf SH3 manual)
        this->PC = 0xA0000000;
        VBR = 0;
    //LoadOS in flash
        QFile file(flashLink);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "Fichier d'OS non trouvé";
        }
        QByteArray content = file.readAll();
        for(int i=0 ; i < content.size() ; i++)
        {
            this->flash[i] = content[i];
        }
        qDebug() << "Core initialized.";
}


void Core::setPC(uint32_t pc){
    this->PC = pc;
}
void Core::setR(uint8_t n, uint32_t value){
    if(n <= 15)
        this->R[n] = value;
}
uint32_t Core::getR(uint8_t n){
    if(n <= 15)
        return this->R[n];
    else return 0;
}

uint32_t Core::getMach(){
    return MACH;
}
uint32_t Core::getMacl(){
    return MACL;
}
uint32_t Core::getPr(){
    return PR;
}
uint32_t Core::getSr(){
    return SR;
}
uint32_t Core::getGbr(){
    return GBR;
}
uint32_t Core::getVbr(){
    return VBR;
}
uint32_t Core::getSsr(){
    return SSR;
}
uint32_t Core::getSpc(){
    return SPC;
}

uint32_t Core::getRn_bank(int n){
    switch(n)
    {
        case 0: return R0_BANK;
        case 1: return R1_BANK;
        case 2: return R2_BANK;
        case 3: return R3_BANK;
        case 4: return R4_BANK;
        case 5: return R5_BANK;
        case 6: return R6_BANK;
        case 7: return R7_BANK;
    }
    return 0;
}


void Core::Next()
{
    this->Execute(this->PC-4);
}




void Core::Execute(uint32_t address)
{
    this->Execute(this->Read_Word(address,true));
}

void Core::Delay_Slot(uint32_t address)
{
    this->Execute(this->Read_Word(address-4,true));
    //TODO: throw an "invalid slot" when delay_slot execute the followings instructions : BF, BT, BRA, BSR, JMP, JSR, RTS, RTE, TRAPA, BF/S, BT/S, BRAF, BSRF
}

uint8_t Core::Read_Byte(uint32_t address, bool execute)
{
    //Flash uncached
    if( (address >> 24) == 0xA0 )
    {
       return this->flash[address & 0xffffff];
    }
    //Flash
    else if( (address >> 24) == 0x80 )
    {
       return this->flash[address & 0xffffff];
    }
    //Ram
    else if( (address >> 24) == 0x88 )
    {
       return this->ram[address & 0xffffff];
    }
    else
    {
        if(execute)
            throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_ADDRESSNOTFOUND);
        else
            throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_READ,  EMULATOR_ERROR_ADDRESSNOTFOUND);
    }

}

uint16_t Core::Read_Word(uint32_t address, bool execute)
{
    //Check 2-byte alignment
    if(address%2 != 0)
    {
        if(execute)
            throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_2BYTEALIGNMENT);
        else
            throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_READ,  EMULATOR_ERROR_2BYTEALIGNMENT);

    }
    return ((this->Read_Byte(address, execute) << 8) + this->Read_Byte(address+1, execute));
}

uint32_t Core::Read_Long(uint32_t address)
{
    //Check 4-byte alignment
    if(address%4 != 0)
        throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_READ,  EMULATOR_ERROR_4BYTEALIGNMENT);
    return ((this->Read_Word(address) << 16) + this->Read_Word(address+2));
}


void Core::Write_Byte(uint32_t address, uint16_t value)
{
    //Flash uncached
    if( (address >> 24) == 0xA0 )
    {
       this->flash[address & 0xffffff] = (value&0xff);
    }
    //Flash
    else if( (address >> 24) == 0x80 )
    {
       this->flash[address & 0xffffff] = (value&0xff);
    }
    //Ram
    else if( (address >> 24) == 0x88 )
    {
       this->ram[address & 0xffffff] = (value&0xff);
    }
    else if(address == 0xb4000000)
    {
        this->DD->setRegister(value&0xff);
    }
    else if(address == 0xb4010000)
    {
        this->DD->putData(value&0xff);
    }
    else
        throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_WRITE,  EMULATOR_ERROR_ADDRESSNOTFOUND);
}

void Core::Write_Word(uint32_t address,  uint16_t value)
{
    //Check 2-byte alignment
    if(address%2 != 0)
        throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_WRITE,  EMULATOR_ERROR_2BYTEALIGNMENT);
    Write_Byte(address, value>>8);
    Write_Byte(address+1, value&0xffff);
}

void Core::Write_Long(uint32_t address, uint32_t value)
{
    //Check 4-byte alignment
    if(address%4 != 0)
        throw emulator_exception(address, this->PC, 0, EMULATOR_ACTION_WRITE,  EMULATOR_ERROR_4BYTEALIGNMENT);
    Write_Word(address, value>>16);
    Write_Word(address+2, value&0xffffffff);
}


uint32_t Core::getPC()
{
    return this->PC;
}


void Core::setT(bool pT)
{
    T_flag = pT;
}

bool Core::getT()
{
    return T_flag;
}
void Core::Execute(uint16_t instruction)
{
    //Cut in nibble (4 bits)
        uint8_t nibble[4];
        nibble[0] = (instruction >> 12)&0xf;
        nibble[1] = (instruction >> 8)&0xf;
        nibble[2] = (instruction >> 4)&0xf;
        nibble[3] = (instruction)&0xf;
    //Declare instructions parameters
        uint16_t n;
        uint16_t m;
        uint32_t d;
        uint32_t disp;
        uint32_t imm;
        uint32_t temp;
        uint32_t i;
        long cnt, sgn;
    //  ---"The big switch"---
        bool done = false;
        switch(nibble[0])
        {
            case b0000:
                switch(nibble[3])
                {
                    case b0010:
                        switch(nibble[2]){
                            case b0000: //0000nnnn00000010 : STC SR,Rn
                                n = nibble[1];
                                R[n]=SR;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0000nnnn00010010 : STC GBR,Rn
                                n = nibble[1];
                                R[n]=GBR;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0000nnnn00100010 : STC VBR,Rn
                                n = nibble[1];
                                R[n]=VBR;
                                PC+=2;
                                done = true;
                                break;
                            case b0011: //0000nnnn00110010 : STC SSR,Rn
                                n = nibble[1];
                                R[n]=SSR;
                                PC+=2;
                                done = true;
                                break;
                            case b0100: //0000nnnn01000010 : STC SPC,Rn
                                n = nibble[1];
                                R[n]=SPC;
                                PC+=2;
                                done = true;
                                break;
                            case b1000: //0000nnnn10000010 : STC R0_BANK,Rm
                                n = nibble[1];
                                R[n]=R0_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1001: //0000nnnn10010010 : STC R1_BANK,Rm
                                n = nibble[1];
                                R[n]=R1_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1010: //0000nnnn10100010 : STC R2_BANK,Rm
                                n = nibble[1];
                                R[n]=R2_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1011: //0000nnnn10110010 : STC R3_BANK,Rm
                                n = nibble[1];
                                R[n]=R3_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1100: //0000nnnn11000010 : STC R4_BANK,Rm
                                n = nibble[1];
                                R[n]=R4_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1101: //0000nnnn11010010 : STC R5_BANK,Rm
                                n = nibble[1];
                                R[n]=R5_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1110: //0000nnnn11100010 : STC R6_BANK,Rm
                                n = nibble[1];
                                R[n]=R6_BANK;
                                PC+=2;
                                done = true;
                                break;
                            case b1111: //0000nnnn11110010 : STC R7_BANK,Rm
                                n = nibble[1];
                                R[n]=R7_BANK;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0011:
                        switch(nibble[2]){
                            case b0000: //0000nnnn00000011 : BSRF Rm
                                n = nibble[1];
                                PR=PC;
                                PC+=R[n];
                                Delay_Slot(PR+2);
                                done = true;
                                break;
                            case b0010: //0000nnnn00100011 : BRAF Rm
                                m = nibble[1];
                                temp=PC;
                                PC+=R[m];
                                Delay_Slot(temp+2);
                                done = true;
                                break;
                            case b1000: //0000nnnn10000011 : PREF @Rn
                                //n = nibble[1];
                                PC+=2;
                                done = true;
                                break;
                        }break;
                    case b0100: //0000nnnnmmmm0100 :MOV.B Rm,@(R0,Rn)
                        n = nibble[1];
                        m = nibble[2];
                        Write_Byte(R[n]+R[0],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0101: //0000nnnnmmmm0101 : MOV.W Rm,@(R0,Rn)
                        n = nibble[1];
                        m = nibble[2];
                        Write_Word(R[n]+R[0],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0110: //0000nnnnmmmm0110 : MOV.L Rm,@(R0,Rn)
                        n = nibble[1];
                        m = nibble[2];
                        Write_Long(R[n]+R[0],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0111: //0000nnnnmmmm0111 : MUL.L Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        MACL=R[n]*R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1000:
                        if(nibble[1] == 0)
                        {
                            switch(nibble[2]) {
                                case b0000: //0000000000001000 : CLRT
                                    T_flag=0;
                                    PC+=2;
                                    done = true;
                                    break;
                                case b0001: //0000000000011000 : SETT
                                    T_flag=1;
                                    PC+=2;
                                    done = true;
                                    break;
                                case b0010: //0000000000101000 : CLRMAC
                                    MACH=0;
                                    MACL=0;
                                    PC+=2;
                                    done = true;
                                    break;
                                case b0011: //0000000000111000 : LDTLB
                                    throw emulator_exception(0, this->PC, instruction, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_INSTRUCTIONNOTFINISHED);
                                    /*TLB_tag=PTEH;
                                    TLB_data=PTEL;
                                    PC+=2;
                                    done = true;*/
                                    break;
                                case b0100: //0000000001001000 : CLRS
                                    S=0;
                                    PC+=2;
                                    done = true;
                                    break;
                                case b0101: //0000000001011000 : SETS
                                    S=1;
                                    PC+=2;
                                    done = true;
                                    break;}
                        } break;
                    case b1001:
                        switch(nibble[2]) {
                            case b0000: //0000000000001001 : NOP
                                if(nibble[1] == 0)
                                {
                                    PC+=2;
                                    done = true;
                                }
                                break;
                            case b0001: //0000000000011001 : DIV0U
                                if(nibble[1] == 0)
                                {
                                    M=Q=T_flag=0;
                                    PC+=2;
                                    done = true;
                                }
                                break;
                            case b0010: //0000nnnn00101001 : MOVT Rn
                                    n = nibble[1];
                                    R[n]=(0x00000001 & SR);
                                    PC+=2;
                                    done = true;
                                break;
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                        case b0000: //0000nnnn00001010 : STS MACH,Rn
                            n = nibble[1];
                            R[n]=MACH;
                            if ((R[n]&0x00000200)==0)
                            R[n]&=0x000003FF;
                            else R[n]|=0xFFFFFC00;
                            PC+=2;
                            done = true;
                            break;
                        case b0001: //0000nnnn00011010 : STS MACL,Rn
                            n = nibble[1];
                            R[n]=MACL;
                            PC+=2;
                            done = true;
                            break;
                        case b0010: //0000nnnn00101010 : STS PR,Rn
                            n = nibble[1];
                            R[n]=PR;
                            PC+=2;
                            done = true;
                            break;
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: //0000000000001011 : RTS
                                if(nibble[1] == 0)
                                {
                                    temp=PC;
                                    PC=PR+2;
                                    Delay_Slot(temp+2);
                                    done = true;
                                }
                                break;
                            case b0001: //0000000000011011 : SLEEP
                                if(nibble[1] == 0)
                                {
                                    throw emulator_exception(0, this->PC, instruction, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_INSTRUCTIONNOTFINISHED);

                                    /*
                                    PC-=2;
                                    Error("Sleep Mode.");
                                    */
                                    done = true;
                                }
                                break;
                            case b0010: //0000000000101011 : RTE
                                if(nibble[1] == 0)
                                {
                                    temp=PC;
                                    PC=SPC;
                                    SR=SSR;
                                    Delay_Slot(temp+2);
                                }
                                break;
                        } break;
                    case b1100: //0000nnnnmmmm1100 : MOV.B @(R0,Rm),Rn
                        n = nibble[1];
                        m = nibble[2]; 
                        R[n]=(long)Read_Byte(R[m]+R[0]);
                        if ((R[n]&0x80)==0) R[n]&=0x000000FF;
                        else R[n]|=0xFFFFFF00;
                        PC+=2;                    
                        done = true;
                        break;
                    case b1101: //0000nnnnmmmm1101 : MOV.W @(R0,Rm),Rn
                        n = nibble[1];
                        m = nibble[2]; 
                        R[n]=(long)Read_Word(R[m]+R[0]);
                        if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
                        else R[n]|=0xFFFF0000;
                        PC+=2;
                        done = true;
                        break;
                    case b1110: //0000nnnnmmmm1110 : MOV.L @(R0,Rm),Rn
                        n = nibble[1];
                        m = nibble[2]; 
                        R[n]=Read_Long(R[m]+R[0]);
                        PC+=2;
                        done = true;
                        break;
                    case b1111: //0000nnnnmmmm1111 : MAC.L @Rm+,@Rn+
                        n = nibble[1];
                        m = nibble[2]; 
                        unsigned long RnL,RnH,RmL,RmH,Res0,Res1,Res2;
                        unsigned long temp0,temp1,temp2,temp3;
                        long tempm,tempn,fnLmL;
                        tempn=(long)Read_Long(R[n]);
                        R[n]+=4;
                        tempm=(long)Read_Long(R[m]);
                        R[m]+=4;
                        if ((long)(tempn^tempm)<0) fnLmL=-1;
                        else fnLmL=0;
                        if (tempn<0) tempn=0-tempn;
                        if (tempm<0) tempm=0-tempm;
                        temp1=(unsigned long)tempn;
                        temp2=(unsigned long)tempm;
                        RnL=temp1&0x0000FFFF;
                        RnH=(temp1>>16)&0x0000FFFF;
                        RmL=temp2&0x0000FFFF;
                        RmH=(temp2>>16)&0x0000FFFF;
                        temp0=RmL*RnL;
                        temp1=RmH*RnL;
                        temp2=RmL*RnH;
                        temp3=RmH*RnH;
                        Res2=0;
                        Res1=temp1+temp2;
                        if (Res1<temp1) Res2+=0x00010000;
                        temp1=(Res1<<16)&0xFFFF0000;
                        Res0=temp0+temp1;
                        if (Res0<temp0) Res2++;
                        Res2=Res2+((Res1>>16)&0x0000FFFF)+temp3;
                        if(fnLmL<0){
                        Res2=~Res2;
                        if (Res0==0) Res2++;
                        else Res0=(~Res0)+1;
                        }
                        if(S==1){
                        Res0=MACL+Res0;
                        if (MACL>Res0) Res2++;
                        Res2+=(MACH&0x0000FFFF);
                        if(((long)Res2<0)&&(Res2<0xFFFF8000)){
                        Res2=0x00008000;
                        Res0=0x00000000;
                        }
                        if(((long)Res2>0)&&(Res2>0x00007FFF)){
                        Res2=0x00007FFF;
                        Res0=0xFFFFFFFF;
                        };
                        MACH=Res2;
                        MACL=Res0;
                        }
                        else {
                        Res0=MACL+Res0;
                        if (MACL>Res0) Res2++;
                        Res2+=MACH;
                        MACH=Res2;
                        MACL=Res0;
                        }
                        PC+=2;
                        done = true;
                        break;
                } break;
            case b0001: //0001nnnnmmmmdddd : MOV.L Rm,@(disp,Rn)
                n = nibble[1];
                m = nibble[2];
                d = nibble[3];
                disp=(0x0000000F & (long)d);
                Write_Long(R[n]+(disp<<2),R[m]);
                PC+=2;
                done = true;
                break;
            case b0010:
                switch(nibble[3]) {
                    case b0000: //0010nnnnmmmm0000 : MOV.B Rm,@Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Byte(R[n],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0001: //0010nnnnmmmm0001 : MOV.W Rm,@Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Word(R[n],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0010: //0010nnnnmmmm0010 : MOV.L Rm,@Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Long(R[n],R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b0100: //0010nnnnmmmm0100 : MOV.B Rm,@-Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Byte(R[n]-1,R[m]);
                        R[n]-=1;
                        PC+=2;
                        done = true;
                        break;
                    case b0101: //0010nnnnmmmm0101 : MOV.W Rm,@-Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Word(R[n]-2,R[m]);
                        R[n]-=2;
                        PC+=2;
                        done = true;
                        break;
                    case b0110: //0010nnnnmmmm0110 : MOV.L Rm,@-Rn
                        n = nibble[1];
                        m = nibble[2];
                        Write_Long(R[n]-4,R[m]);
                        R[n]-=4;
                        PC+=2;
                        done = true;
                        break;
                    case b0111: //0010nnnnmmmm0111 : DIV0S Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((R[n]&0x80000000)==0) Q=0;
                        else Q=1;
                        if ((R[m]&0x80000000)==0) M=0;
                        else M=1;
                        T_flag=!(M==Q);
                        PC+=2;
                        done = true;
                        break;
                    case b1000: //0010nnnnmmmm1000 : TST Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((R[n]&R[m])==0) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1001: //0010nnnnmmmm1001 : AND Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]&=R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1010: //0010nnnnmmmm1010 : XOR Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]^=R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1011: //0010nnnnmmmm1011 : OR Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]|=R[m];
                        PC+=2;
                        done = true;
                        break;
                  case b1100: //0010nnnnmmmm1100 : CMP_STR Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        long HH,HL,LH,LL;
                        temp=R[n]^R[m];
                        HH=(temp&0xFF000000)>>12;
                        HL=(temp&0x00FF0000)>>8;
                        LH=(temp&0x0000FF00)>>4; LL=temp&0x000000FF;
                        HH=HH&&HL&&LH&&LL;
                        if (HH==0) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1101: //0010nnnnmmmm1101 : XTRCT Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        temp=(R[m]<<16)&0xFFFF0000;
                        R[n]=(R[n]>>16)&0x0000FFFF;
                        R[n]|=temp;
                        PC+=2;
                        done = true;
                        break;
                    case b1110: //0010nnnnmmmm1110 : MULU Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        MACL=((unsigned long)(unsigned short)R[n]
                        *(unsigned long)(unsigned short)R[m]);
                        PC+=2;
                        done = true;
                        break;
                    case b1111: //0010nnnnmmmm1111 : MULS Rm,Rn 
                        n = nibble[1];
                        m = nibble[2];
                        MACL=((long)(short)R[n]*(long)(short)R[m]);
                        PC+=2;
                        done = true;
                        break;
                } break;
            case b0011:
                switch(nibble[3]) {
                    case b0000: //0011nnnnmmmm0000 : CMP_EQ Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if (R[n]==R[m]) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b0010: //0011nnnnmmmm0010 : CMP_HS Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((unsigned long)R[n]>=(unsigned long)R[m]) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b0011: //0011nnnnmmmm0011 : CMP_GE Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((long)R[n]>=(long)R[m]) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b0100: //0011nnnnmmmm0100 : DIV1 Rm,Rn
                        if(true)
                        {
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long tmp0;
                        unsigned char old_q,tmp1;
                        old_q=Q;
                        Q=(unsigned char)((0x80000000 & R[n])!=0);
                        R[n]<<=1;
                        R[n]|=(unsigned long)T_flag;
                        switch(old_q){
                        case 0:switch(M){
                        case 0:tmp0=R[n];
                        R[n]-=R[m];
                        tmp1=(R[n]>tmp0);
                        switch(Q){
                        case 0:Q=tmp1;
                        break;
                        case 1:Q=(unsigned char)(tmp1==0);
                        break;
                        }
                        break;
                        case 1:tmp0=R[n];
                        R[n]+=R[m];
                        tmp1=(R[n]<tmp0);
                        switch(Q){
                        case 0:Q=(unsigned char)(tmp1==0);
                        break;
                        case 1:Q=tmp1;
                        break;
                        }
                        break;
                        }
                        break;
                        case 1:switch(M){
                        case 0:tmp0=R[n];
                        R[n]+=R[m];
                        tmp1=(R[n]<tmp0);
                        switch(Q){
                        case 0:Q=tmp1;
                        break;
                        case 1:Q=(unsigned char)(tmp1==0);
                            break;
                        }
                        break;
                        case 1:tmp0=R[n];
                        R[n]-=R[m];
                        tmp1=(R[n]>tmp0);
                        switch(Q){
                        case 0:Q=(unsigned char)(tmp1==0);
                            break;
                        case 1:Q=tmp1;
                        break;
                        }
                        break;
                        }
                        break;
                        }
                        T_flag=(Q==M);
                        PC+=2;
                        done = true;
                        }
                        break;
                    case b0101: //0011nnnnmmmm0101 : DMULU.L Rm,Rn
                        if(true)
                        {
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long RnL,RnH,RmL,RmH,Res0,Res1,Res2;
                        unsigned long temp0,temp1,temp2,temp3;
                        RnL=R[n]&0x0000FFFF;
                        RnH=(R[n]>>16)&0x0000FFFF;
                        RmL=R[m]&0x0000FFFF;
                        RmH=(R[m]>>16)&0x0000FFFF;
                        temp0=RmL*RnL;
                        temp1=RmH*RnL;
                        temp2=RmL*RnH;
                        temp3=RmH*RnH;
                        Res2=0;
                        Res1=temp1+temp2;
                        if (Res1<temp1) Res2+=0x00010000;
                        temp1=(Res1<<16)&0xFFFF0000;
                        Res0=temp0+temp1;
                        if (Res0<temp0) Res2++;
                        Res2=Res2+((Res1>>16)&0x0000FFFF)+temp3;
                        MACH=Res2;
                        MACL=Res0;
                        PC+=2;
                        done = true;
                        break;
                        }
                    case b0110: //0011nnnnmmmm0110 : CMP_HI Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((unsigned long)R[n]>(unsigned long)R[m]) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b0111: //0011nnnnmmmm0111 : CMP_GT Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if ((long)R[n]>(long)R[m]) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1000: //0011nnnnmmmm1000 : SUB Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]-=R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1010: //0011nnnnmmmm1010 : SUBC Rm,Rn
                        if(true)
                        {
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long tmp0,tmp1;
                        tmp1=R[n]-R[m];
                        tmp0=R[n];
                        R[n]=tmp1-T_flag;
                        if (tmp0<tmp1) T_flag=1;
                        else T_flag=0;
                        if (tmp1<R[n]) T_flag=1;
                        PC+=2;
                        done = true;
                        break;
                        }
                    case b1011: //0011nnnnmmmm1011 : SUBV Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        if(true)
                        {
                        long dest,src,ans;
                        if ((long)R[n]>=0) dest=0;
                        else dest=1;
                        if ((long)R[m]>=0) src=0;
                        else src=1;
                        src+=dest;
                        R[n]-=R[m];
                        if ((long)R[n]>=0) ans=0;
                        else ans=1;
                        ans+=dest;
                        if (src==1) {
                        if (ans==1) T_flag=1;
                        else T_flag=0;
                        }
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                        }
                    case b1100: //0011nnnnmmmm1100 : ADD Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]+=R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1101: //0011nnnnmmmm1101 : DMULS.L Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long RnL,RnH,RmL,RmH,Res0,Res1,Res2;
                        unsigned
                         long temp0,temp1,temp2,temp3;
                        long tempm,tempn,fnLmL;
                        tempn=(long)R[n];
                        tempm=(long)R[m];
                        if (tempn<0) tempn=0-tempn;
                        if (tempm<0) tempm=0-tempm;
                        if ((long)(R[n]^R[m])<0) fnLmL=-1;
                        else fnLmL=0;
                        temp1=(unsigned long)tempn;
                        temp2=(unsigned long)tempm;
                        RnL=temp1&0x0000FFFF;
                        RnH=(temp1>>16)&0x0000FFFF;
                        RmL=temp2&0x0000FFFF;
                        RmH=(temp2>>16)&0x0000FFFF;
                        temp0=RmL*RnL;
                        temp1=RmH*RnL;
                        temp2=RmL*RnH;
                        temp3=RmH*RnH;
                        Res2=0;
                        Res1=temp1+temp2;
                        if (Res1<temp1) Res2+=0x00010000;
                        temp1=(Res1<<16)&0xFFFF0000;
                        Res0=temp0+temp1;
                        if (Res0<temp0) Res2++;
                        Res2=Res2+((Res1>>16)&0x0000FFFF)+temp3;
                        if (fnLmL<0) {
                        Res2=~Res2;
                        if (Res0==0)
                        Res2++;
                        else
                        Res0=(~Res0)+1;
                        }
                        MACH=Res2;
                        MACL=Res0;
                        PC+=2;
                        done = true;
                        break;
                    case b1110: //0011nnnnmmmm1110 : ADDC Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long tmp0,tmp1;
                        tmp1=R[n]+R[m];
                        tmp0=R[n];
                        R[n]=tmp1+T_flag;
                        if (tmp0>tmp1) T_flag=1;
                        else T_flag=0;
                        if (tmp1>R[n]) T_flag=1;
                        PC+=2;
                        done = true;
                        break;
                    case b1111: //0011nnnnmmmm1111 : ADDV Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        long dest,src,ans;
                        if ((long)R[n]>=0) dest=0;
                        else dest=1;
                        if ((long)R[m]>=0) src=0;
                        else src=1;
                        src+=dest;
                        R[n]+=R[m];
                        if ((long)R[n]>=0) ans=0;
                        else ans=1;
                        ans+=dest;
                        if (src==0 || src==2) {
                        if (ans==1) T_flag=1;
                        else T_flag=0;
                        }
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                } break;
            case b0100:
                switch(nibble[3]) {
                    case b0000:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000000 : SHLL Rn(Same as SHAL)
                                n = nibble[1];
                                if ((R[n]&0x80000000)==0) T_flag=0;
                                else T_flag=1;
                                R[n]<<=1;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00010000 : DT Rn
                                n = nibble[1];
                                R[n]--;
                                if (R[n]==0) T_flag=1;
                                else T_flag=0;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100000 : SHAL Rn(Same as SHLL)
                                n = nibble[1];
                                if ((R[n]&0x80000000)==0) T_flag=0;
                                else T_flag=1;
                                R[n]<<=1;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0001:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000001 : SHLR Rn
                                n = nibble[1];
                                if ((R[n]&0x00000001)==0) T_flag=0;
                                else T_flag=1;
                                R[n]>>=1;
                                R[n]&=0x7FFFFFFF;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00010001 : CMP_PZ Rn
                                n = nibble[1];
                                if ((long)R[n]>=0) T_flag=1;
                                else T_flag=0;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100001 : SHAR Rn
                                n = nibble[1];
                                if ((R[n]&0x00000001)==0) T_flag=0;
                                else T_flag=1;
                                if ((R[n]&0x80000000)==0) temp=0;
                                else temp=1;
                                R[n]>>=1;
                                if (temp==1) R[n]|=0x80000000;
                                else R[n]&=0x7FFFFFFF;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0010:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000010 : STS.L MACH,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                if ((MACH&0x00000200)==0)
                                Write_Long(R[n],MACH&0x000003FF);
                                else Write_Long (R[n],MACH|0xFFFFFC00);
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00010010 : STS.L MACL,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],MACL);
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100010 : STS.L PR,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],PR);
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0011:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000011 : STC.L SR,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],SR);
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00010011 : STC.L GBR,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],GBR);
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100011 : STC.L VBR,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],VBR);
                                PC+=2;
                                done = true;
                                break;
                            case b0011: //0100nnnn00110011 : STC.L SSR,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],SSR);
                                PC+=2;
                                done = true;
                                break;
                            case b0100: //0100nnnn01000011 : STC.L SPC,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],SPC);
                                PC+=2;
                                done = true;
                                break;
                            case b1000: //0100nnnn10000011 : STC.L R0_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R0_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1001: //0100nnnn10010011 : STC.L R1_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R1_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1010: //0100nnnn10100011 : STC.L R2_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R2_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1011: //0100nnnn10110011 : STC.L R3_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R3_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1100: //0100nnnn11000011 : STC.L R4_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R4_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1101: //0100nnnn11010011 : STC.L R5_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R5_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1110: //0100nnnn11100011 : STC.L R6_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R6_BANK);
                                PC+=2;
                                done = true;
                                break;
                            case b1111: //0100nnnn11110011 : STC.L R7_BANK,@-Rn
                                n = nibble[1];
                                R[n]-=4;
                                Write_Long(R[n],R7_BANK);
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0100:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000100 : ROTL Rn
                                n = nibble[1];
                                if ((R[n]&0x80000000)==0) T_flag=0;
                                else T_flag=1;
                                R[n]<<=1;
                                if (T_flag==1) R[n]|=0x00000001;
                                else R[n]&=0xFFFFFFFE;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100100 : ROTCL Rn
                                n = nibble[1];
                                if ((R[n]&0x80000000)==0) temp=0;
                                else temp=1;
                                R[n]<<=1;
                                if (T_flag==1) R[n]|=0x00000001;
                                else R[n]&=0xFFFFFFFE;
                                if (temp==1) T_flag=1;
                                else T_flag=0;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0101:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00000101 : ROTR Rn
                                n = nibble[1];
                                if ((R[n]&0x00000001)==0) T_flag=0;
                                else T_flag=1;
                                R[n]>>=1;
                                if (T_flag==1) R[n]|=0x80000000;
                                else R[n]&=0x7FFFFFFF;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00010101 : CMP_PL Rn
                                n = nibble[1];
                                if ((long)R[n]>0) T_flag=1;
                                else T_flag=0;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00100101 : ROTCR Rn
                                n = nibble[1];
                                if ((R[n]&0x00000001)==0) temp=0;
                                else temp=1;
                                R[n]>>=1;
                                if (T_flag==1) R[n]|=0x80000000;
                                else R[n]&=0x7FFFFFFF;
                                if (temp==1) T_flag=1;
                                else T_flag=0;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b0110:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00000110 : LDS.L @Rm+,MACH
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
                    case b1000:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001000 : SHLL2 Rn
                                n = nibble[1];
                                R[n]<<=2;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00011000 : SHLL8 Rn
                                n = nibble[1];
                                R[n]<<=8;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00101000 : SHLL16 Rn
                                n = nibble[1];
                                R[n]<<=16;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b1001:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001001 : SHLR2 Rn
                                n = nibble[1];
                                R[n]>>=2;
                                R[n]&=0x3FFFFFFF;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100nnnn00011001 : SHLR8 Rn
                                n = nibble[1];
                                R[n]>>=8;
                                R[n]&=0x00FFFFFF;
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100nnnn00101001 : SHLR16 Rn
                                n = nibble[1];
                                R[n]>>=16;
                                R[n]&=0x0000FFFF;
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b1010:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00001010 : LDS Rm,MACH
                                m = nibble[1];
                                MACH=R[m];
                                if ((MACH&0x00000200)==0) MACH&=0x000003FF;
                                else MACH|=0xFFFFFC00;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100mmmm00011010 : LDS Rm,MACL
                                m = nibble[1];
                                MACL=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b0010: //0100mmmm00101010 : LDS Rm,PR
                                m = nibble[1];
                                PR=R[m];
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b1011:
                        switch(nibble[2]) {
                            case b0000: //0100nnnn00001011 : JSR @Rn
                                n = nibble[1];
                                PR=PC;
                                PC=R[n]+2;
                                Delay_Slot(PR+2);
                                done=true;
                                break;
                            case b0001: //0100nnnn00011011 : TAS.B @Rn
                                n = nibble[1];
                                temp=(long)Read_Byte(R[n]); // Bus Lock enable
                                if (temp==0) T_flag=1;
                                else T_flag=0;
                                temp|=0x00000080;
                                Write_Byte(R[n],temp);
                                //Bus Lock disable
                                PC+=2;
                                done=true;
                                break;
                            case b0010: //0100nnnn00101011 : JMP @Rn
                                n = nibble[1];
                                temp=PC;
                                PC=R[n]+2;
                                Delay_Slot(temp+2);
                                done=true;
                                break;
                        } break;
                    case b1100: //0100nnnnmmmm1100 : SHAD Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        sgn = R[m] &0x80000000;
                        cnt = R[m] &0x0000001F;
                        if (sgn==0)
                            R[n]<<=cnt;
                        else
                            R[n]=(signed long)R[n]>>((~cnt+1) & 0x1F); //shift arithmetic right
                        PC+=2;
                        done=true;
                        break;
                    case b1101: //0100nnnnmmmm1101 : SHLD Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        sgn = R[m]&0x80000000;
                        cnt = R[m]&0x0000001F;
                        if (sgn==0)
                         R[n]<<=cnt;
                        else
                         R[n]=R[n]>>((~cnt+1)&0x1F);
                        PC+=2;
                        done=true;
                        break;
                    case b1110:
                        switch(nibble[2]) {
                            case b0000: //0100mmmm00001110 : LDC Rm,SR
                                m = nibble[1];
                                SR=R[m]&0x0FFF0FFF;
                                PC+=2;
                                done = true;
                                break;
                            case b0001: //0100mmmm00011110 : LDC Rm,GBR
                                m = nibble[1];
                                GBR=R[m];
                                PC+=2;
                                done = true;
                                break;
                           case b0010: //0100mmmm00101110 : LDC Rm,VBR
                                m = nibble[1];
                                VBR=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b0011: //0100mmmm00111110 : LDC Rm,SSR
                                m = nibble[1];
                                SSR=R[m]&0x700003F3;
                                PC+=2;
                                done = true;
                                break;
                            case b0100: //0100mmmm01001110 : LDC Rm,SPC
                                m = nibble[1];
                                SPC=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1000: //0100mmmm10001110 : LDC Rm,R0_BANK
                                m = nibble[1];
                                R0_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1001: //0100mmmm10011110 : LDC Rm,R1_BANK
                                m = nibble[1];
                                R1_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1010: //0100mmmm10101110 : LDC Rm,R2_BANK
                                m = nibble[1];
                                R2_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1011: //0100mmmm10111110 : LDC Rm,R3_BANK
                                m = nibble[1];
                                R3_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1100: //0100mmmm11001110 : LDC Rm,R4_BANK
                                m = nibble[1];
                                R4_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1101: //0100mmmm11011110 : LDC Rm,R5_BANK
                                m = nibble[1];
                                R5_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1110: //0100mmmm11101110 : LDC Rm,R6_BANK
                                m = nibble[1];
                                R6_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                            case b1111: //0100mmmm11111110 : LDC Rm,R7_BANK
                                m = nibble[1];
                                R7_BANK=R[m];
                                PC+=2;
                                done = true;
                                break;
                        } break;
                    case b1111: //0100nnnnmmmm1111 : MAC.W @Rm+,@Rn+
                        n = nibble[1];
                        m = nibble[2];
                        long tempm,tempn,dest,src,ans;
                        unsigned long templ;
                        tempn=(long)Read_Word(R[n]);
                        R[n]+=2;
                        tempm=(long)Read_Word(R[m]);
                        R[m]+=2;
                        templ=MACL;
                        tempm=((long)(short)tempn*(long)(short)tempm);
                        if ((long)MACL>=0) dest=0;
                        else dest=1;
                        if ((long)tempm>=0) {
                        src=0;
                        tempn=0;
                        }
                        else {
                        src=1;
                        tempn=0xFFFFFFFF;
                        }
                        src+=dest;
                        MACL+=tempm;
                        if ((long)MACL>=0) ans=0;
                        else ans=1;
                        ans+=dest;
                        if (S==1) {
                        if (ans==1) {
                        if (src==0 || src==2) MACH|=0x00000001;
                        if (src==0) MACL=0x7FFFFFFF;
                        if (src==2) MACL=0x80000000;
                        }
                        }
                        else {
                        MACH+=tempn;
                        if (templ>MACL) MACH+=1;
                        if ((MACH&0x00000200)==0) MACH&=0x000003FF;
                        else MACH|=0xFFFFFC00;
                        }
                        PC+=2;
                        done=true;
                        break;
               } break;
            case b0101: //0101nnnnmmmmdddd : MOV.L @(disp,Rm),Rn
                n = nibble[1];
                m = nibble[2];
                d = nibble[3];
                disp=(0x0000000F & (long)d);
                R[n]=Read_Long(R[m]+(disp<<2));
                PC+=2;
                done = true;
                break;
            case b0110:
                switch(nibble[3]) {
                    case b0000: //0110nnnnmmmm0000 : MOV.B @Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=(long)Read_Byte(R[m]);
                        if ((R[n]&0x80)==0) R[n]&=0x000000FF;
                        else R[n]|=0xFFFFFF00;
                        PC+=2;
                        done = true;
                        break;
                    case b0001: //0110nnnnmmmm0001 : MOV.W @Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=(long)Read_Word(R[m]);
                        if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
                        else R[n]|=0xFFFF0000;
                        PC+=2;
                        done = true;
                        break;
                    case b0010: //0110nnnnmmmm0010 : MOV.L @Rm,Rn
                        n = nibble[1];
                        m = nibble[2]; 
                        R[n]=Read_Long(R[m]);
                        PC+=2;
                        done = true;
                        break;
                     case b0011: //0110nnnnmmmm0011 : MOV Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b0100: //0110nnnnmmmm0100 : MOV.B @Rm+,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=(long)Read_Byte(R[m]);
                        if ((R[n]&0x80)==0) R[n]&=0x000000FF;
                        else R[n]|=0xFFFFFF00;
                        if (n!=m) R[m]+=1;
                        PC+=2;
                        done = true;
                        break;
                    case b0101: //0110nnnnmmmm0101 : MOV.W @Rm+,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=(long)Read_Word(R[m]);
                        if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
                        else R[n]|=0xFFFF0000;
                        if (n!=m) R[m]+=2;
                        PC+=2;
                        done = true;
                        break;
                    case b0110: //0110nnnnmmmm0110 : MOV.L @Rm+,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=Read_Long(R[m]);
                        if (n!=m) R[m]+=4;
                        PC+=2;
                        done = true;
                        break;
                    case b0111: //0110nnnnmmmm0111 : NOT Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=~R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1000: //0110nnnnmmmm1000 : SWAP.B Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long temp0,temp1;
                        temp0=R[m]&0xffff0000;
                        temp1=(R[m]&0x000000ff)<<8;
                        R[n]=(R[m]&0x0000ff00)>>8;
                        R[n]=R[n]|temp1|temp0;
                        PC+=2;
                        done = true;
                        break;
                   case b1001: //0110nnnnmmmm1001 : SWAP.W Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        unsigned long temp;
                        temp=(R[m]>>16)&0x0000FFFF;
                        R[n]=R[m]<<16;
                        R[n]|=temp;
                        PC+=2;
                        done = true;
                        break;

                    case b1010: //0110nnnnmmmm1010 : NEGC Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        temp=0-R[m];
                        R[n]=temp-T_flag;
                        if (0<temp)
                            T_flag=1;
                        else T_flag=0;
                        if (temp<R[n])
                            T_flag=1;
                        PC+=2;
                        done = true;
                        break;
                    case b1011: //0110nnnnmmmm1011 : NEG Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=0-R[m];
                        PC+=2;
                        done = true;
                        break;
                    case b1100: //0110nnnnmmmm1100 : EXTU.B Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=R[m];
                        R[n]&=0x000000FF;
                        PC+=2;
                        done = true;
                        break;
                    case b1101: //0110nnnnmmmm1101 : EXTU.W Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=R[m];
                        R[n]&=0x0000FFFF;
                        PC+=2;
                        done = true;
                        break;
                    case b1110: //0110nnnnmmmm1110 : EXTS.B Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=R[m];
                        if ((R[m]&0x00000080)==0) R[n]&=0x000000FF;
                        else R[n]|=0xFFFFFF00;
                        PC+=2;
                        done = true;
                        break;
                    case b1111: //0110nnnnmmmm1111 : EXTS.W Rm,Rn
                        n = nibble[1];
                        m = nibble[2];
                        R[n]=R[m];
                        if ((R[m]&0x00008000)==0) R[n]&=0x0000FFFF;
                        else R[n]|=0xFFFF0000;
                        PC+=2;
                        done = true;
                        break;
                } break;
            case b0111: //0111nnnniiiiiiii : ADD #imm,Rn
                n = nibble[1];
                i = (nibble[2]<<4) + nibble[3];
                if ((i&0x80)==0) R[n]+=(0x000000FF & (long)i);
                else R[n]+=(0xFFFFFF00 | (long)i);
                PC+=2;
                done = true;
                break;
            case b1000:
                switch(nibble[1]) {
                    case b0000: //10000000nnnndddd : MOV.B R0,@(disp,Rn)
                        n = nibble[2];
                        d = nibble[3];
                        disp=(0x0000000F & (long)d);
                        Write_Byte(R[n]+disp,R[0]);
                        PC+=2;
                        done = true;
                        break;
                    case b0001: //10000001nnnndddd : MOV.W R0,@(disp,Rn)
                        n = nibble[2];
                        d = nibble[3];
                        disp=(0x0000000F & (long)d);
                        Write_Word(R[n]+(disp<<1),R[0]);
                        PC+=2;
                        done = true;
                        break;
                    case b0100: //10000100mmmmdddd : MOV.B @(disp,Rm),R0
                        m = nibble[2];
                        d = nibble[3];
                        disp=(0x0000000F & (long)d);
                        R[0]=Read_Byte(R[m]+disp);
                        if ((R[0]&0x80)==0) R[0]&=0x000000FF;
                        else R[0]|=0xFFFFFF00;
                        PC+=2;
                        done = true;
                        break;
                    case b0101: //10000101mmmmdddd : MOV.W @(disp,Rm),R0
                        m = nibble[2];
                        d = nibble[3];
                        disp=(0x0000000F & (long)d);
                        R[0]=Read_Word(R[m]+(disp<<1));
                        if ((R[0]&0x8000)==0) R[0]&=0x0000FFFF;
                        else R[0]|=0xFFFF0000;
                        PC+=2;
                        done = true;
                        break;
                    case b1000: //10001000iiiiiiii : CMP_EQ #imm,R0
                        i = (nibble[2]<<4) + nibble[3];
                        if ((i&0x80)==0) imm=(0x000000FF & (long)i);
                        else imm=(0xFFFFFF00 | (long)i);
                        if (R[0]==imm) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1001: //10001001dddddddd : BT disp
                         d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                        else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==1) PC=PC+(disp<<1)+4;
                        else PC+=2;
                        done = true;
                        break;
                    case b1011: //10001011dddddddd : BF disp
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                        else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==0) PC=PC+(disp<<1)+4;
                        else PC+=2;
                        done = true;
                        break;
                    case b1101: //10001101dddddddd : BT/S disp
                        d = (nibble[2]<<4) + nibble[3];
                        temp=PC;
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                            else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==1) {
                            PC=PC+(disp<<1)+2;
                            Delay_Slot(temp+2);
                        }
                        else PC+=2;
                        done = true;
                        break;
                    case b1111: //10001111dddddddd : BF/S disp
                        d = (nibble[2]<<4) + nibble[3];
                        temp=PC;
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                        else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==0) {
                            PC=PC+(disp<<1)+2;
                            Delay_Slot(temp+2);
                        }
                        else PC+=2;
                        done = true;
                        break;
                } break;
            case b1001: //1001nnnndddddddd : MOV.W @(disp,PC),Rn
                n = nibble[1];
                d = (nibble[2]<<4) + nibble[3];
                disp=(0x000000FF & (long)d);
                R[n]=(long)Read_Word(PC+(disp<<1));
                if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
                else R[n]|=0xFFFF0000;
                PC+=2;
                done = true;
                break;
           case b1010: //1010dddddddddddd : BRA disp
                d = (nibble[1]<<8) + (nibble[2]<<4) + nibble[3];
                if ((d&0x800)==0) disp=(0x00000FFF & d);
                else disp=(0xFFFFF000 | d);
                temp=PC;
                PC=PC+(disp<<1)+2;
                Delay_Slot(temp+2);
                done = true;
                break;
            case b1011: //1011dddddddddddd : BSR disp
                d = (nibble[1]<<8) + (nibble[2]<<4) + nibble[3];
                if ((d&0x800)==0) disp=(0x00000FFF & d);
                else disp=(0xFFFFF000 | d);
                PR=PC;
                PC=PC+(disp<<1)+2;
                Delay_Slot(PR+2);
                done = true;
                break;
            case b1100:
                switch(nibble[1]) {
                    case b0000: //11000000dddddddd : MOV.B R0,@(disp,GBR)
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        Write_Byte(GBR+disp,R[0]);
                        PC+=2;
                        done = true;
                        break;
                    case b0001: //11000001dddddddd : MOV.W R0,@(disp,GBR)
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        Write_Word(GBR+(disp<<1),R[0]);
                        PC+=2;
                        break;
                    case b0010: //11000010dddddddd : MOV.L R0,@(disp,GBR)
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        Write_Long(GBR+(disp<<2),R[0]);
                        PC+=2;
                        done = true;
                        break;
                    case b0011: //11000011iiiiiiii : TRAPA #imm
                        throw emulator_exception(0, this->PC, instruction, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_INSTRUCTIONNOTFINISHED);
                        /*
                        i = (nibble[2]<<4) + nibble[3];
                        imm=(0x000000FF & i);
                        TRA=imm<<2;
                        SSR=SR;
                        SPC=PC;
                        SR.MD=1;
                        SR.BL=1;
                        SR.RB=1;
                        EXPEVT=0x00000160;
                        PC=VBR+0x00000100;
                        */
                        break;
                    case b0100: //11000100dddddddd : MOV.B @(disp,GBR),R0
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        R[0]=(long)Read_Byte(GBR+disp);
                        if ((R[0]&0x80)==0) R[0]&=0x000000FF;
                        else R[0]|=0xFFFFFF00;
                        PC+=2;
                        done = true;
                        break;
                    case b0101: //11000101dddddddd : MOV.W @(disp,GBR),R0
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        R[0]=(long)Read_Word(GBR+(disp<<1));
                        if ((R[0]&0x8000)==0) R[0]&=0x0000FFFF;
                        else R[0]|=0xFFFF0000;
                        PC+=2;
                        done = true;
                        break;
                    case b0110: //11000110dddddddd : MOV.L @(disp,GBR),R0
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        R[0]=Read_Long(GBR+(disp<<2));
                        PC+=2;
                        done = true;
                        break;
                    case b0111: //11000111dddddddd : MOVA @(disp,PC),R0
                        d = (nibble[2]<<4) + nibble[3];
                        disp=(0x000000FF & (long)d);
                        R[0]=(PC&0xFFFFFFFC)+(disp<<2);
                        PC+=2;
                        done = true;
                        break;
                    case b1000: //11001000iiiiiiii : TST #imm,R0
                        i = (nibble[2]<<4) + nibble[3];
                        temp=R[0]&(0x000000FF & (long)i);
                        if (temp==0) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1001: //11001001iiiiiiii : AND #imm,R0
                        i = (nibble[2]<<4) + nibble[3];
                        R[0]&=(0x000000FF & (long)i);
                        PC+=2;
                        done = true;
                        break;
                    case b1010: //11001010iiiiiiii : XOR #imm,R0
                        i = (nibble[2]<<4) + nibble[3];
                        R[0]^=(0x000000FF & (long)i);
                        PC+=2;
                        done = true;
                        break;
                    case b1011: //11001011iiiiiiii : OR #imm,R0
                        i = (nibble[2]<<4) + nibble[3];
                        R[0]|=(0x000000FF & (long)i);
                        PC+=2;
                        done = true;
                        break;
                    case b1100: //11001100iiiiiiii : TST.B #imm,@(R0,GBR)
                        i = (nibble[2]<<4) + nibble[3];
                        temp=(long)Read_Byte(GBR+R[0]);
                        temp&=(0x000000FF & (long)i);
                        if (temp==0) T_flag=1;
                        else T_flag=0;
                        PC+=2;
                        done = true;
                        break;
                    case b1101: //11001101iiiiiiii : AND.B #imm,@(R0,GBR)
                        i = (nibble[2]<<4) + nibble[3];
                        temp=(long)Read_Byte(GBR+R[0]);
                        temp&=(0x000000FF & (long)i);
                        Write_Byte(GBR+R[0],temp);
                        PC+=2;
                        done = true;
                        break;
                    case b1110: //11001110iiiiiiii : XOR.B #imm,@(R0,GBR)
                        i = (nibble[2]<<4) + nibble[3];
                        temp=(long)Read_Byte(GBR+R[0]);
                        temp^=(0x000000FF & (long)i);
                        Write_Byte(GBR+R[0],temp);
                        PC+=2;
                        done = true;
                        break;
                    case b1111: //11001111iiiiiiii : OR.B #imm,@(R0,GBR)
                        i = (nibble[2]<<4) + nibble[3];
                        temp=(long)Read_Byte(this->GBR+this->R[0]);
                        temp|=(0x000000FF & (long)i);
                        Write_Byte(this->GBR+this->R[0],temp);
                        this->PC+=2;
                        done = true;
                        break;
                } break;
            case b1101: //1101nnnndddddddd : MOV.L @(disp,PC),Rn
                d = (nibble[2]<<4) + nibble[3];
                n = nibble[1];
                disp=(0x000000FF & (long)d);
                this->R[n]=Read_Long((this->PC&0xFFFFFFFC)+(disp<<2));
                this->PC+=2;
                done = true;
                break;
            case b1110: //1110nnnniiiiiiii :  MOV #imm,Rn
                i = (nibble[2]<<4) + nibble[3];
                n = nibble[1];
                if ((i&0x80)==0) 
                    R[n]=(0x000000FF & (long)i);
                else 
                    R[n]=(0xFFFFFF00 | (long)i);
                PC+=2;
                done = true;
        }
        //Si onction non trouveé
        if(done == false)
        {     
            throw emulator_exception(0, this->PC, instruction, EMULATOR_ACTION_EXECUTE,  EMULATOR_ERROR_NOTINSTRUCTION);
        }
}

/*
0xD = 0b1101
0xE = 0b1110
0xF = 0b1111


    DoBinary(short binInstruction)
    {

        unsigned char n;
        unsigned int temp;
        
    }
    
};*/

