#ifndef CORE_HPP
#define CORE_HPP

#include <fstream>
#include <string>
#include <vector>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <cstdint> //width constant types : http://www.cplusplus.com/reference/cstdint/
#include "emulator_exception.hpp"
#include "displaydriver.h"

#define M ((*(struct SR0 *)(&SR)).M0)
#define Q ((*(struct SR0 *)(&SR)).Q0)
#define S ((*(struct SR0 *)(&SR)).S0)
//#define T ((*(struct SR0 *)(&SR)).T0)
//#define T_flag ((*(struct SR0 *)(&SR)).T0)
#define T_flag T
#define RF1 ((*(struct SR0 *)(&SR)).RF10)
#define RF0 ((*(struct SR0 *)(&SR)).RF00)

class Core
{
	private:
        //uint8_t flash[0x400000]; //Flash : 0xA0000000(not cached) OR 0x80000000(cached) (accessed between 0x80000000 and 0x803FFFFF)
        //uint8_t ram[512000];//Ram : 0x88000000

        std::vector<uint8_t> flash;
        std::vector<uint8_t> ram;

		//PC : Progam Counter : Indicate starting address of the current instruction indremented by four (two instructions). Initialized to H'A000 0000
		//PR : Stores the return address for existing subroutines. Underfined after reset
		//MACH/MACL : Multiply and Accumulate Hight and Low register (MACH/L) : Store the result of multiply and multiply-and-accumulate operation. Undefined after reset.

        uint32_t R[16] = {0};
		uint32_t SR,GBR,VBR;
		uint32_t MACH,MACL,PR;
        uint32_t PC;
        uint8_t TRA;
        bool T;
        uint32_t SSR,SPC;
        uint8_t RS;
        uint8_t test = 1;

        uint32_t R0_BANK;
        uint32_t R1_BANK;
        uint32_t R2_BANK;
        uint32_t R3_BANK;
        uint32_t R4_BANK;
        uint32_t R5_BANK;
        uint32_t R6_BANK;
        uint32_t R7_BANK;
        uint32_t lastJumpOrigin = 0;

		int8_t display_select = 0;
		int8_t display_data = 0;


		struct SR0 
		{
			uint32_t dummy0:4;
			uint32_t RC0:12;
			uint32_t dummy1:4;
			uint32_t DMY0:1;
			uint32_t DMX0:1;
			uint32_t M0:1;
			uint32_t Q0:1;
			uint32_t I0:4;
			uint32_t RF10:1;
			uint32_t RF00:1;
			uint32_t S0:1;
			uint32_t T0:1;
		};

        DisplayDriver* DD;


	public:
        Core(QString flashLink, DisplayDriver *DD);
        void Execute(uint32_t address);
		void Execute(uint16_t instruction);

		uint8_t Read_Byte(uint32_t address, bool execute = false);
		uint16_t Read_Word(uint32_t address, bool execute = false);
		uint32_t Read_Long(uint32_t address);
		void Write_Byte(uint32_t address, uint16_t value);
		void Write_Word(uint32_t address,  uint16_t value);
        void Write_Long(uint32_t address, uint32_t value);
        void Delay_Slot(uint32_t address);


        void setPC(uint32_t pc);
        uint32_t getPC();
        void setR(uint8_t n, uint32_t value);
        uint32_t getR(uint8_t n);
        void setT(bool pT);
        bool getT();
        uint32_t getMach();
        uint32_t getMacl();
        uint32_t getPr();
        uint32_t getSr();
        uint32_t getGbr();
        uint32_t getVbr();
        uint32_t getSsr();
        uint32_t getSpc();
        uint32_t getRn_bank(int n);
        uint32_t getLastJumpOrigin();



public slots:
        void Next();
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

#endif // CORE_HPP
