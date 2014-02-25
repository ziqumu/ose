#include "core.h"
  
Core::Core(QString flashLink, DisplayDriver* DD)
{
    this->flash.resize(0x400000);
    this->ram.resize(0x40000);
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

uint32_t Core::getLastJumpOrigin(){
    return lastJumpOrigin;
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
    //Hardware address.. different fix to make the OS work..
    else if(address == 0xfffffe92)  return 0;
    //else if(address == 0xfffffea8)  return 0;
    else if(address == 0xfffffea9)  return 0;
    else if(address == 0xfffffedc)  return 0x80;
    else if(address == 0xfffffede)  return 0x9;
    //else if(address == 0xfffffee2)  return 0;
    //else if(address == 0xfffffee3)  return 0;
    //else if(address == 0xfffffee4)  return 0;
    //else if(address == 0xfffffee5)  return 0;
    else if(address == 0xffffff82)  return 0;
    else if(address == 0xffffff86)  return 8;
    else if(address == 0xfffffec0)  return ((int)(QDateTime::currentDateTime().toMSecsSinceEpoch()/50)) ; // RTC_GetTicks returns the value of the RTC counter. This counter is initialized every days at 00:00, and is incremented every 1/128
     //else if(address == 0xffffffd4)  return 0;
    //else if(address == 0xffffffd5)  return 0;
    //else if(address == 0xffffffd6)  return 0;
    //else if(address == 0xffffffd7)  return 0;
    else if(address == 0xa4000004)  return 0x80; //When a key is pressed this value is 0x80 else 0
    //else if(address == 0xa4000010)  return 0;
    else if(address == 0xa4000011)  return 0;
    else if(address == 0xa4000012)  return 0;
    else if(address == 0xa4000013)  return 0;
    else if(address == 0xa4000018)  return 0;
    else if(address == 0xa4000019)  return 0;
    else if(address == 0xa400001a)  return 0;
    else if(address == 0xa400001b)  return 0;
    else if(address == 0xa400001c)  return 0;
    else if(address == 0xa400001d)  return 0;
    else if(address == 0xa4000088)  return 0;
    else if(address == 0xa4000089)  return 0;
    else if(address == 0xa400008a)  return 0;
    else if(address == 0xa400008b)  return 0;
    else if(address == 0xa400008c)  return 0;
    else if(address == 0xa400008d)  return 0;
    else if(address == 0xa400008e)  return 0;
    else if(address == 0xa400008f)  return 0;
    else if((address&0xfffffffc) == 0xa4000014)  return 0;
    else if(address == 0xa4000108)  return 0;
    else if(address == 0xa4000109)  return 0;
    else if(address == 0xa400010a)  return 0;
    else if(address == 0xa400010b)  return 0;
    else if(address == 0xa400010c)  return 0;
    else if(address == 0xa400010d)  return 0;
    else if(address == 0xa400010e)  return 0;
    else if(address == 0xa400010f)  return 0;
    else if(address == 0xa4000116)  return 0;
    else if(address == 0xa4000117)  return 0;
    else if(address == 0xa4000118)  return 0;
    else if(address == 0xa4000119)  return 0;
    else if(address == 0xa4000120)  return 0;
    else if(address == 0xa4000122)  return 0;
    else if(address == 0xa4000124)  return 0;
    else if(address == 0xa4000126)  return 0;
    else if(address == 0xa4000128)  return 0;
    else if(address == 0xa400012a)  return 0;
    else if(address == 0xa400012c)  return 0;
    else if(address == 0xa4000136)  return 0;
    else if(address == 0xa4000138)  return 0;
    else if(address == 0xa400012e)  return 0;
    else if(address == 0xa4080000)  return 0;
    else if(address == 0xa4080001)  return 0;
    else if(address == 0xa4080002)  return 0;
    else if(address == 0xa4080003)  return 0;
    else if(address == 0xa405014e)  return 0;
    else if(address == 0xa4050148)  return 0;
    else if(address == 0xa413fee4)  return 0;
    else if(address == 0xa4410008)  return 0;
    else if(address == 0xa4410009)  return 0;
    else if(address == 0xa44c0020)  return 0;
    else if(address == 0xa44c0030)  return 0;
    else if(address == 0xa44c003c)  return 0;
    else if((address&0xfffffffc) == 0xa5603a98)  return 0;
    else if(address == 0xa5603aa0)  return 0;
    else if(address == 0xa5603aa1)  return 0;
    else if(address == 0xa5603aa2)  return 0;
    else if(address == 0xa5603aa3)  return 0;
    else if(address == 0xb4010000)  return 0;
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
    //Hardware address.. some fix to make the OS work..
    else if((address&0xfffffffc) == 0xf2000000){}
    else if((address&0xfffffffc) == 0xf2001000){}
    else if((address&0xfffffffc) == 0xf2002000){}
    else if((address&0xfffffffc) == 0xf2003000){}
    else if((address&0xfffffffc) == 0xf2004000){}
    else if((address&0xfffffffc) == 0xf2005000){}
    else if((address&0xfffffffc) == 0xf2006000){}
    else if((address&0xfffffffc) == 0xf2007000){}
    else if((address&0xfffffffc) == 0xf2008000){}
    else if((address&0xfffffffc) == 0xf2009000){}
    else if((address&0xfffffffc) == 0xf200a000){}
    else if((address&0xfffffffc) == 0xf200b000){}
    else if((address&0xfffffffc) == 0xf200c000){}
    else if((address&0xfffffffc) == 0xf200d000){}
    else if((address&0xfffffffc) == 0xf200e000){}
    else if((address&0xfffffffc) == 0xf200f000){}
    else if((address&0xfffffffc) == 0xf2010000){}
    else if((address&0xfffffffc) == 0xf2011000){}
    else if((address&0xfffffffc) == 0xf2012000){}
    else if((address&0xfffffffc) == 0xf2013000){}
    else if((address&0xfffffffc) == 0xf2014000){}
    else if((address&0xfffffffc) == 0xf2015000){}
    else if((address&0xfffffffc) == 0xf2016000){}
    else if((address&0xfffffffc) == 0xf2017000){}
    else if((address&0xfffffffc) == 0xf2018000){}
    else if((address&0xfffffffc) == 0xf2019000){}
    else if((address&0xfffffffc) == 0xf201a000){}
    else if((address&0xfffffffc) == 0xf201b000){}
    else if((address&0xfffffffc) == 0xf201c000){}
    else if((address&0xfffffffc) == 0xf201d000){}
    else if((address&0xfffffffc) == 0xf201e000){}
    else if((address&0xfffffffc) == 0xf201f000){}

    else if((address&0xfffffffc) == 0xf2000100){}
    else if((address&0xfffffffc) == 0xf2001100){}
    else if((address&0xfffffffc) == 0xf2002100){}
    else if((address&0xfffffffc) == 0xf2003100){}
    else if((address&0xfffffffc) == 0xf2004100){}
    else if((address&0xfffffffc) == 0xf2005100){}
    else if((address&0xfffffffc) == 0xf2006100){}
    else if((address&0xfffffffc) == 0xf2007100){}
    else if((address&0xfffffffc) == 0xf2008100){}
    else if((address&0xfffffffc) == 0xf2009100){}
    else if((address&0xfffffffc) == 0xf200a100){}
    else if((address&0xfffffffc) == 0xf200b100){}
    else if((address&0xfffffffc) == 0xf200c100){}
    else if((address&0xfffffffc) == 0xf200d100){}
    else if((address&0xfffffffc) == 0xf200e100){}
    else if((address&0xfffffffc) == 0xf200f100){}
    else if((address&0xfffffffc) == 0xf2010100){}
    else if((address&0xfffffffc) == 0xf2011100){}
    else if((address&0xfffffffc) == 0xf2012100){}
    else if((address&0xfffffffc) == 0xf2013100){}
    else if((address&0xfffffffc) == 0xf2014100){}
    else if((address&0xfffffffc) == 0xf2015100){}
    else if((address&0xfffffffc) == 0xf2016100){}
    else if((address&0xfffffffc) == 0xf2017100){}
    else if((address&0xfffffffc) == 0xf2018100){}
    else if((address&0xfffffffc) == 0xf2019100){}
    else if((address&0xfffffffc) == 0xf201a100){}
    else if((address&0xfffffffc) == 0xf201b100){}
    else if((address&0xfffffffc) == 0xf201c100){}
    else if((address&0xfffffffc) == 0xf201d100){}
    else if((address&0xfffffffc) == 0xf201e100){}
    else if((address&0xfffffffc) == 0xf201f100){}

    else if((address&0xfffffffc) == 0xf2000200){}
    else if((address&0xfffffffc) == 0xf2001200){}
    else if((address&0xfffffffc) == 0xf2002200){}
    else if((address&0xfffffffc) == 0xf2003200){}
    else if((address&0xfffffffc) == 0xf2004200){}
    else if((address&0xfffffffc) == 0xf2005200){}
    else if((address&0xfffffffc) == 0xf2006200){}
    else if((address&0xfffffffc) == 0xf2007200){}
    else if((address&0xfffffffc) == 0xf2008200){}
    else if((address&0xfffffffc) == 0xf2009200){}
    else if((address&0xfffffffc) == 0xf200a200){}
    else if((address&0xfffffffc) == 0xf200b200){}
    else if((address&0xfffffffc) == 0xf200c200){}
    else if((address&0xfffffffc) == 0xf200d200){}
    else if((address&0xfffffffc) == 0xf200e200){}
    else if((address&0xfffffffc) == 0xf200f200){}
    else if((address&0xfffffffc) == 0xf2010200){}
    else if((address&0xfffffffc) == 0xf2011200){}
    else if((address&0xfffffffc) == 0xf2012200){}
    else if((address&0xfffffffc) == 0xf2013200){}
    else if((address&0xfffffffc) == 0xf2014200){}
    else if((address&0xfffffffc) == 0xf2015200){}
    else if((address&0xfffffffc) == 0xf2016200){}
    else if((address&0xfffffffc) == 0xf2017200){}
    else if((address&0xfffffffc) == 0xf2018200){}
    else if((address&0xfffffffc) == 0xf2019200){}
    else if((address&0xfffffffc) == 0xf201a200){}
    else if((address&0xfffffffc) == 0xf201b200){}
    else if((address&0xfffffffc) == 0xf201c200){}
    else if((address&0xfffffffc) == 0xf201d200){}
    else if((address&0xfffffffc) == 0xf201e200){}
    else if((address&0xfffffffc) == 0xf201f200){}

    else if((address&0xfffffffc) == 0xf2000300){}
    else if((address&0xfffffffc) == 0xf2001300){}
    else if((address&0xfffffffc) == 0xf2002300){}
    else if((address&0xfffffffc) == 0xf2003300){}
    else if((address&0xfffffffc) == 0xf2004300){}
    else if((address&0xfffffffc) == 0xf2005300){}
    else if((address&0xfffffffc) == 0xf2006300){}
    else if((address&0xfffffffc) == 0xf2007300){}
    else if((address&0xfffffffc) == 0xf2008300){}
    else if((address&0xfffffffc) == 0xf2009300){}
    else if((address&0xfffffffc) == 0xf200a300){}
    else if((address&0xfffffffc) == 0xf200b300){}
    else if((address&0xfffffffc) == 0xf200c300){}
    else if((address&0xfffffffc) == 0xf200d300){}
    else if((address&0xfffffffc) == 0xf200e300){}
    else if((address&0xfffffffc) == 0xf200f300){}
    else if((address&0xfffffffc) == 0xf2010300){}
    else if((address&0xfffffffc) == 0xf2011300){}
    else if((address&0xfffffffc) == 0xf2012300){}
    else if((address&0xfffffffc) == 0xf2013300){}
    else if((address&0xfffffffc) == 0xf2014300){}
    else if((address&0xfffffffc) == 0xf2015300){}
    else if((address&0xfffffffc) == 0xf2016300){}
    else if((address&0xfffffffc) == 0xf2017300){}
    else if((address&0xfffffffc) == 0xf2018300){}
    else if((address&0xfffffffc) == 0xf2019300){}
    else if((address&0xfffffffc) == 0xf201a300){}
    else if((address&0xfffffffc) == 0xf201b300){}
    else if((address&0xfffffffc) == 0xf201c300){}
    else if((address&0xfffffffc) == 0xf201d300){}
    else if((address&0xfffffffc) == 0xf201e300){}
    else if((address&0xfffffffc) == 0xf201f300){}

    else if(address == 0xfffffe92){}
    else if(address == 0xfffffe93){}
    else if(address == 0xfffffe94){}
    else if(address == 0xfffffe95){}
    else if(address == 0xfffffea0){}
    else if(address == 0xfffffea1){}
    else if(address == 0xfffffea2){}
    else if(address == 0xfffffea3){}
    else if(address == 0xfffffea4){}
    else if(address == 0xfffffea5){}
    else if(address == 0xfffffea6){}
    else if(address == 0xfffffea7){}
    else if(address == 0xfffffea8){}
    else if(address == 0xfffffea9){}
    else if(address == 0xfffffeac){}
    else if(address == 0xfffffead){}
    else if(address == 0xfffffeae){}
    else if(address == 0xfffffeaf){}
    else if(address == 0xfffffeb0){}
    else if(address == 0xfffffeb1){}
    else if(address == 0xfffffeb2){}
    else if(address == 0xfffffeb3){}
    else if(address == 0xfffffeb4){}
    else if(address == 0xfffffeb5){}
    else if(address == 0xfffffeb6){}
    else if(address == 0xfffffeb7){}
    else if(address == 0xfffffed0){}
    else if(address == 0xfffffed2){}
    else if(address == 0xfffffed4){}
    else if(address == 0xfffffed6){}
    else if(address == 0xfffffed8){}
    else if(address == 0xfffffeda){}
    else if(address == 0xfffffedc){}
    else if(address == 0xfffffede){}
    else if(address == 0xfffffee2){}
    else if(address == 0xfffffee3){}
    else if(address == 0xfffffee4){}
    else if(address == 0xfffffee5){}
    else if(address == 0xffffff80){}
    else if(address == 0xffffff81){}
    else if(address == 0xffffff82){}
    else if(address == 0xffffff84){}
    else if(address == 0xffffff85){}
    else if(address == 0xffffff86){}
    else if(address == 0xffffff87){}
    else if(address == 0xffffff88){}
    else if(address == 0xffffffe0){}
    else if(address == 0xffffffe1){}
    else if(address == 0xffffffe2){}
    else if(address == 0xffffffe3){}
    else if(address == 0xffffffec){}
    else if(address == 0xffffffed){}
    else if(address == 0xffffffee){}
    else if(address == 0xffffffef){}
    else if(address == 0xa4000004){}
    else if(address == 0xa4000005){}
    else if(address == 0xa4000010){}
    else if(address == 0xa4000011){}
    else if(address == 0xa4000012){}
    else if(address == 0xa4000013){}
    else if((address&0xfffffffc) == 0xa4000014){}
    else if(address == 0xa4000018){}
    else if(address == 0xa4000019){}
    else if(address == 0xa400001a){}
    else if(address == 0xa400001b){}
    else if(address == 0xa4000088){}
    else if(address == 0xa4000089){}
    else if(address == 0xa400008a){}
    else if(address == 0xa400008b){}
    else if(address == 0xa400008c){}
    else if(address == 0xa400008d){}
    else if(address == 0xa400008e){}
    else if(address == 0xa400008f){}
    else if(address == 0xa40000b4){}
    else if(address == 0xa40000b5){}
    else if(address == 0xa40000b6){}
    else if(address == 0xa40000b7){}
    else if(address == 0xa4000100){}
    else if(address == 0xa4000101){}
    else if(address == 0xa4000102){}
    else if(address == 0xa4000103){}
    else if(address == 0xa4000104){}
    else if(address == 0xa4000105){}
    else if(address == 0xa4000106){}
    else if(address == 0xa4000107){}
    else if(address == 0xa4000108){}
    else if(address == 0xa4000109){}
    else if(address == 0xa400010a){}
    else if(address == 0xa400010b){}
    else if(address == 0xa400010c){}
    else if(address == 0xa400010d){}
    else if(address == 0xa400010e){}
    else if(address == 0xa400010f){}
    else if(address == 0xa4000110){}
    else if(address == 0xa4000111){}
    else if(address == 0xa4000112){}
    else if(address == 0xa4000113){}
    else if(address == 0xa4000114){}
    else if(address == 0xa4000115){}
    else if(address == 0xa4000116){}
    else if(address == 0xa4000117){}
    else if(address == 0xa4000118){}
    else if(address == 0xa4000119){}
    else if(address == 0xa400011a){}
    else if(address == 0xa400011b){}
    else if(address == 0xa4000122){}
    else if(address == 0xa4000124){}
    else if(address == 0xa4000126){}
    else if(address == 0xa4000128){}
    else if(address == 0xa400012a){}
    else if(address == 0xa400012c){}
    else if(address == 0xa400012e){}
    else if(address == 0xa4000136){}
    else if(address == 0xa4000138){}
    else if(address == 0xa400014e){}
    else if(address == 0xa4050146){}
    else if(address == 0xa4050148){}
    else if(address == 0xa405014a){}
    else if(address == 0xa405014e){}
    else if(address == 0xa405015a){}
    else if(address == 0xa405015c){}
    else if(address == 0xa405017b){}
    else if(address == 0xa4080000){}
    else if(address == 0xa4080001){}
    else if(address == 0xa4080002){}
    else if(address == 0xa4080003){}
    else if(address == 0xa4080004){}
    else if(address == 0xa4080005){}
    else if(address == 0xa40a0000){}
    else if(address == 0xa40a0004){}
    else if(address == 0xa40a0008){}
    else if(address == 0xa40a0009){}
    else if(address == 0xa413fee0){}
    else if(address == 0xa413fee1){}
    else if(address == 0xa413fee2){}
    else if(address == 0xa413fee3){}
    else if(address == 0xa413fee4){}
    else if(address == 0xa413fee5){}
    else if(address == 0xa413fee6){}
    else if(address == 0xa413fee7){}
    else if(address == 0xa4410008){}
    else if(address == 0xa4410009){}
    else if(address == 0xa44c0004){}
    else if(address == 0xa44c0005){}
    else if(address == 0xa44c0006){}
    else if(address == 0xa44c0020){}
    else if(address == 0xa44c0030){}
    else if(address == 0xa44c0034){}
    else if(address == 0xa44c0035){}
    else if(address == 0xa44c0036){}
    else if(address == 0xa44c0037){}
    else if(address == 0xa44c0038){}
    else if(address == 0xa44c0039){}
    else if(address == 0xa44c003a){}
    else if(address == 0xa44c003b){}
    else if(address == 0xa44c003c){}
    else if(address == 0xa4fd0000){}
    else if(address == 0xa4fd0001){}
    else if(address == 0xa4fd0002){}
    else if(address == 0xa4fd0003){}
    else if(address == 0xa4fd0004){}
    else if(address == 0xa4fd0005){}
    else if(address == 0xa4fd0006){}
    else if(address == 0xa4fd0007){}
    else if(address == 0xa4fd0008){}
    else if(address == 0xa4fd0009){}
    else if(address == 0xa4fd000a){}
    else if(address == 0xa4fd000b){}
    else if(address == 0xa4fd0010){}
    else if(address == 0xa4fd0011){}
    else if(address == 0xa4fd0012){}
    else if(address == 0xa4fd0013){}
    else if(address == 0xa4fd0014){}
    else if(address == 0xa4fd0015){}
    else if(address == 0xa4fd0016){}
    else if(address == 0xa4fd0017){}
    else if(address == 0xa4fd0018){}
    else if(address == 0xa4fd0019){}
    else if(address == 0xa4fd001a){}
    else if(address == 0xa4fd001b){}
    else if(address == 0xa4fd0024){}
    else if(address == 0xa4fd0025){}
    else if(address == 0xa4fd0026){}
    else if(address == 0xa4fd0027){}
    else if(address == 0xa4fd0028){}
    else if(address == 0xa4fd0029){}
    else if(address == 0xa4fd002a){}
    else if(address == 0xa4fd002b){}
    else if(address == 0xa4fd0030){}
    else if(address == 0xa4fd0031){}
    else if(address == 0xa4fd0032){}
    else if(address == 0xa4fd0033){}
    else if(address == 0xa4fd0034){}
    else if(address == 0xa4fd0035){}
    else if(address == 0xa4fd0036){}
    else if(address == 0xa4fd0037){}
    else if(address == 0xa4fd0038){}
    else if(address == 0xa4fd0039){}
    else if(address == 0xa4fd003a){}
    else if(address == 0xa4fd003b){}
    else if((address&0xfffffffc) == 0xa5600000){}
    else if((address&0xfffffffc) == 0xa5600004){}
    else if((address&0xfffffffc) == 0xa5600008){}
    else if((address&0xfffffffc) == 0xa560000c){}
    else if((address&0xfffffffc) == 0xa5600010){}
    else if((address&0xfffffffc) == 0xa5600014){}
    else if((address&0xfffffffc) == 0xa5600018){}
    else if((address&0xfffffffc) == 0xa560001c){}
    else if((address&0xfffffffc) == 0xa5600020){}
    else if((address&0xfffffffc) == 0xa5600024){}
    else if((address&0xfffffffc) == 0xa5600028){}
    else if((address&0xfffffffc) == 0xa560002c){}
    else if((address&0xfffffffc) == 0xa5600030){}
    else if((address&0xfffffffc) == 0xa5600034){}
    else if((address&0xfffffffc) == 0xa5600038){}
    else if((address&0xfffffffc) == 0xa560003c){}
    else if((address&0xfffffffc) == 0xa5600040){}
    else if((address&0xfffffffc) == 0xa5600044){}
    else if((address&0xfffffffc) == 0xa5600048){}
    else if((address&0xfffffffc) == 0xa560004c){}
    else if((address&0xfffffffc) == 0xa5600050){}
    else if((address&0xfffffffc) == 0xa5600054){}
    else if((address&0xfffffffc) == 0xa5600058){}
    else if((address&0xfffffffc) == 0xa560005c){}
    else if((address&0xfffffffc) == 0xa5600060){}
    else if((address&0xfffffffc) == 0xa5600064){}
    else if((address&0xfffffffc) == 0xa5600068){}
    else if((address&0xfffffffc) == 0xa560006c){}
    else if((address&0xfffffffc) == 0xa5600070){}
    else if((address&0xfffffffc) == 0xa5600074){}
    else if((address&0xfffffffc) == 0xa5600078){}
    else if((address&0xfffffffc) == 0xa560007c){}
    else if((address&0xfffffffc) == 0xa5600080){}
    else if((address&0xfffffffc) == 0xa5600084){}
    else if((address&0xfffffffc) == 0xa5600088){}
    else if((address&0xfffffffc) == 0xa560008c){}
    else if((address&0xfffffffc) == 0xa5600090){}
    else if((address&0xfffffffc) == 0xa5600094){}
    else if((address&0xfffffffc) == 0xa5600098){}
    else if((address&0xfffffffc) == 0xa560009c){}
    else if((address&0xfffffffc) == 0xa56000a0){}
    else if((address&0xfffffffc) == 0xa56000a4){}
    else if((address&0xfffffffc) == 0xa56000a8){}
    else if((address&0xfffffffc) == 0xa56000ac){}
    else if((address&0xfffffffc) == 0xa56000b0){}
    else if((address&0xfffffffc) == 0xa56000b4){}
    else if((address&0xfffffffc) == 0xa56000b8){}
    else if((address&0xfffffffc) == 0xa56000bc){}
    else if((address&0xfffffffc) == 0xa56000c0){}
    else if((address&0xfffffffc) == 0xa56000c4){}
    else if((address&0xfffffffc) == 0xa56000c8){}
    else if((address&0xfffffffc) == 0xa56000cc){}
    else if((address&0xfffffffc) == 0xa56000d0){}
    else if((address&0xfffffffc) == 0xa56000d4){}
    else if((address&0xfffffffc) == 0xa56000d8){}
    else if((address&0xfffffffc) == 0xa56000dc){}
    else if((address&0xfffffffc) == 0xa56000e0){}
    else if((address&0xfffffffc) == 0xa56000e4){}
    else if((address&0xfffffffc) == 0xa56000e8){}
    else if((address&0xfffffffc) == 0xa56000ec){}
    else if((address&0xfffffffc) == 0xa56000f0){}
    else if((address&0xfffffffc) == 0xa56000f4){}
    else if((address&0xfffffffc) == 0xa56000f8){}
    else if((address&0xfffffffc) == 0xa56000fc){}


    else if((address&0xfffffffc) == 0xa5600100){}
    else if((address&0xfffffffc) == 0xa5600104){}
    else if((address&0xfffffffc) == 0xa5600108){}
    else if((address&0xfffffffc) == 0xa560010c){}
    else if((address&0xfffffffc) == 0xa5600110){}
    else if((address&0xfffffffc) == 0xa5600114){}
    else if((address&0xfffffffc) == 0xa5600118){}
    else if((address&0xfffffffc) == 0xa560011c){}
    else if((address&0xfffffffc) == 0xa5600120){}
    else if((address&0xfffffffc) == 0xa5600124){}
    else if((address&0xfffffffc) == 0xa5600128){}
    else if((address&0xfffffffc) == 0xa560012c){}
    else if((address&0xfffffffc) == 0xa5600130){}
    else if((address&0xfffffffc) == 0xa5600134){}
    else if((address&0xfffffffc) == 0xa5600138){}
    else if((address&0xfffffffc) == 0xa560013c){}
    else if((address&0xfffffffc) == 0xa5600140){}
    else if((address&0xfffffffc) == 0xa5600144){}
    else if((address&0xfffffffc) == 0xa5600148){}
    else if((address&0xfffffffc) == 0xa560014c){}
    else if((address&0xfffffffc) == 0xa5600150){}
    else if((address&0xfffffffc) == 0xa5600154){}
    else if((address&0xfffffffc) == 0xa5600158){}
    else if((address&0xfffffffc) == 0xa560015c){}
    else if((address&0xfffffffc) == 0xa5600160){}
    else if((address&0xfffffffc) == 0xa5600164){}
    else if((address&0xfffffffc) == 0xa5600168){}
    else if((address&0xfffffffc) == 0xa560016c){}
    else if((address&0xfffffffc) == 0xa5600170){}
    else if((address&0xfffffffc) == 0xa5600174){}
    else if((address&0xfffffffc) == 0xa5600178){}
    else if((address&0xfffffffc) == 0xa560017c){}
    else if((address&0xfffffffc) == 0xa5600180){}
    else if((address&0xfffffffc) == 0xa5600184){}
    else if((address&0xfffffffc) == 0xa5600188){}
    else if((address&0xfffffffc) == 0xa560018c){}
    else if((address&0xfffffffc) == 0xa5600190){}
    else if((address&0xfffffffc) == 0xa5600194){}
    else if((address&0xfffffffc) == 0xa5600198){}
    else if((address&0xfffffffc) == 0xa560019c){}
    else if((address&0xfffffffc) == 0xa56001a0){}
    else if((address&0xfffffffc) == 0xa56001a4){}
    else if((address&0xfffffffc) == 0xa56001a8){}
    else if((address&0xfffffffc) == 0xa56001ac){}
    else if((address&0xfffffffc) == 0xa56001b0){}
    else if((address&0xfffffffc) == 0xa56001b4){}
    else if((address&0xfffffffc) == 0xa56001b8){}
    else if((address&0xfffffffc) == 0xa56001bc){}
    else if((address&0xfffffffc) == 0xa56001c0){}
    else if((address&0xfffffffc) == 0xa56001c4){}
    else if((address&0xfffffffc) == 0xa56001c8){}
    else if((address&0xfffffffc) == 0xa56001cc){}
    else if((address&0xfffffffc) == 0xa56001d0){}
    else if((address&0xfffffffc) == 0xa56001d4){}
    else if((address&0xfffffffc) == 0xa56001d8){}
    else if((address&0xfffffffc) == 0xa56001dc){}
    else if((address&0xfffffffc) == 0xa56001e0){}
    else if((address&0xfffffffc) == 0xa56001e4){}
    else if((address&0xfffffffc) == 0xa56001e8){}
    else if((address&0xfffffffc) == 0xa56001ec){}
    else if((address&0xfffffffc) == 0xa56001f0){}
    else if((address&0xfffffffc) == 0xa56001f4){}
    else if((address&0xfffffffc) == 0xa56001f8){}
    else if((address&0xfffffffc) == 0xa56001fc){}
    else if(address == 0xa5603a98){}
    else if(address == 0xa5603a99){}
    else if(address == 0xa5603a9a){}
    else if(address == 0xa5603a9b){}
    else if(address == 0xa5603a9c){}
    else if(address == 0xa5603a9d){}
    else if(address == 0xa5603a9e){}
    else if(address == 0xa5603a9f){}
    else if(address == 0xa5603aa0){}
    else if(address == 0xa5603aa1){}
    else if(address == 0xa5603aa2){}
    else if(address == 0xa5603aa3){}
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
                                    lastJumpOrigin = PC-4;
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
                                    lastJumpOrigin = PC-4;
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
                            case b0001: //0100mmmm00010110 : LDS.L @Rm+,MACL
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
                                lastJumpOrigin = PC-4;
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
                                lastJumpOrigin = PC-4;
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
                        if (T_flag==1){
                            lastJumpOrigin = PC-4;
                            PC=PC+(disp<<1)+4;
                        }
                        else PC+=2;
                        done = true;
                        break;
                    case b1011: //10001011dddddddd : BF disp
                        d = (nibble[2]<<4) + nibble[3];
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                        else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==0){
                            lastJumpOrigin = PC-4;
                            PC=PC+(disp<<1)+4;}
                        else PC+=2;
                        done = true;
                        break;
                    case b1101: //10001101dddddddd : BT/S disp
                        d = (nibble[2]<<4) + nibble[3];
                        temp=PC;
                        if ((d&0x80)==0) disp=(0x000000FF & (long)d);
                            else disp=(0xFFFFFF00 | (long)d);
                        if (T_flag==1) {
                            lastJumpOrigin = PC-4;
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
                            lastJumpOrigin = PC-4;
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
                lastJumpOrigin = PC-4;
                temp=PC;
                PC=PC+(disp<<1)+2;
                Delay_Slot(temp+2);
                done = true;
                break;
            case b1011: //1011dddddddddddd : BSR disp
                d = (nibble[1]<<8) + (nibble[2]<<4) + nibble[3];
                if ((d&0x800)==0) disp=(0x00000FFF & d);
                else disp=(0xFFFFF000 | d);
                lastJumpOrigin = PC-4;
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
                        done = true;
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

