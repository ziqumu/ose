#ifndef EMULATOR_EXCEPTION_HPP
#define EMULATOR_EXCEPTION_HPP

#include <exception>
#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>
#include <QString>

#define EMULATOR_ACTION_READ 1
#define EMULATOR_ACTION_WRITE 2
#define EMULATOR_ACTION_EXECUTE 3

#define EMULATOR_ERROR_2BYTEALIGNMENT 0
#define EMULATOR_ERROR_4BYTEALIGNMENT 1
#define EMULATOR_ERROR_ADDRESSNOTFOUND 2
#define EMULATOR_ERROR_NOTINSTRUCTION 3

#define EMULATOR_ERROR_INSTRUCTIONNOTFINISHED 255

class emulator_exception: public std::exception
{
	private: 
		uint32_t address;
		uint32_t PC;
		uint16_t instruction; //Binary instruction code
		uint8_t action; //00:read, 01:write, 2:execute
		uint8_t error; //00:alignement_2byte, 01:alignement_4byte, 2:address_not_found, 3:not_instruction
        QString msg;
	public:
		uint32_t getPC();
		uint32_t getAction();
		//getAddress();
		//getInstruction();
		//getError();
		//getAction();
		emulator_exception(uint32_t address, uint32_t PC, uint16_t instruction, uint8_t action,  uint8_t error);
		virtual const char* what() const throw();
};


#endif // EMULATOR_EXCEPTION_HPP
