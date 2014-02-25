#include "emulator_exception.hpp"
using namespace std;

emulator_exception::emulator_exception(uint32_t address, uint32_t PC, uint16_t instruction, uint8_t action,  uint8_t error)
{
	this->address = address;
	this->PC = PC;
	this->instruction = instruction;
	this->action = action;
	this->error = error;

	//get action string
        QString action_str;
		switch(action)
		{
			case EMULATOR_ACTION_READ: action_str = "read"; break;
			case EMULATOR_ACTION_WRITE: action_str = "write"; break;
			case EMULATOR_ACTION_EXECUTE: action_str = "execute"; break;
		}
	//get error string
        QString error_str;
		switch(error)
		{
            case EMULATOR_ERROR_2BYTEALIGNMENT: error_str = "address not 2-byte aligned"; break;
            case EMULATOR_ERROR_4BYTEALIGNMENT: error_str = "address not 4-byte aligned"; break;
            case EMULATOR_ERROR_ADDRESSNOTFOUND: error_str = "address not found"; break;
		}
    //Make hex numbers
        //Address
        QString address_str(QString::number(this->address,16));
        //instruction
        QString instruction_str(QString::number(this->instruction,16));

    //make error string
	if(error == EMULATOR_ERROR_NOTINSTRUCTION)
        this->msg = "Can't " + action_str + " the instruction 0x" + instruction_str + " but this is not a valid instruction !";
	else if(error == EMULATOR_ERROR_INSTRUCTIONNOTFINISHED)
        this->msg = "Can't " + action_str + " the instruction 0x" + instruction_str + " but this instruction is not working !";
	else
        this->msg = "Can't " + action_str + " at 0x" + address_str + " : " + error_str + " !";
}

uint32_t emulator_exception::getPC()
{
	return this->PC;
}
uint32_t emulator_exception::getAction()
{
	return this->action;
}

const char* emulator_exception::what() const throw()
{
    return this->msg.toStdString().c_str();
}
