#include "CPU.h"

CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}

unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPC()
{
	PC += 4;
}
void CPU::setPC(int update) {
	PC += update;
}
int CPU::dmemRead(int address, int funct3) {
	if (funct3 == 0b000) {
		// cout << "MemRead: " << "Reading " << dmemory[address] << " from address " << address << endl;
		return dmemory[address];
	}
	if (funct3 == 0b010) {
		// for (int i = 0; i < 4; i++)
		// 	cout << "Address " << address + i << ": " << hex << dmemory[address+i] << endl;
		int output =  (dmemory[address+3]<<24) | (dmemory[address+2]<<16) | (dmemory[address+1]<<8) | dmemory[address];
		// cout << "MemRead: " << "Reading " << output << " from address " << address << endl;
		return output;
	}
}
void CPU::dmemWrite(int address, int data, int funct3) {
	if (funct3 == 0b000) {
		// cout << "MemWrite: " << (data&0xFF) << " written to address " << address << endl;
		dmemory[address] = data & 0xFF;
	}
	if (funct3 == 0b010) {
		dmemory[address] = data & 0xFF; 
		dmemory[address+1] = (data << 8) & 0xFF;
		dmemory[address+2] = (data << 16) & 0xFF;
		dmemory[address+3] = (data << 24) & 0xFF;
		// dmemory[address] = data;
	}
}
u_int32_t CPU::fetchInstruction(u_int32_t instMem[], int PC) {
	int instruction = 0;
	for (int i = 0; i < 4; i++) { 
		instruction |= (instMem[PC+i] << (8*(i)));
		// cout << hex << instruction << endl;
	}
	return instruction;
}

// Add other functions here ... 
RegFile::RegFile() 
{
	regWrite = 0;
	writeRegister = 0;
	for (int i = 0; i < 32; i++) {
		registerValues[i] = 0;
	}
}
void RegFile::setRegWrite(int rW) {
	regWrite = rW;
}
int RegFile::readRegister1(int rs1) {
	return registerValues[rs1];
}
int RegFile::readRegister2(int rs2) {
	return registerValues[rs2];
}
void RegFile::setWriteRegister(int rd) {
	if (regWrite == 1) {
		writeRegister = rd;
	}
}
void RegFile::writeDatatoReg(int data) {
	if (regWrite == 1 && writeRegister != 0) {
		registerValues[writeRegister] = data;
	}
}

void ALU::setAluSrc(int aC) {
	aluSrc = aC;
}
void ALU::setAluControl(int aC) {
	aluControl = aC;
}
void ALU::setAluInputs(int i1, int i2) {
	rs1 = i1;
	if (aluSrc == 0) {
		rs2 = i2;
		imm = 0;
	} else {
		imm = i2;
		rs2 = 0;
	}
}
int ALU::output() {
	if (aluSrc == 0) { // R-type operation
		if (aluControl == 0b0010) {
			// cout << "Performing R-type ADD operation!" << endl;
			return rs1 + rs2;
		} // ADD 
		if (aluControl == 0b0110) // SUB
			return rs1 - rs2;
		if (aluControl == 0b0011) // XOR 
			return rs1 ^ rs2;
	} else {
		if (aluControl == 0b0010) // ADDI
			return rs1 + imm;
		if (aluControl == 0b0001) // ORI
			return rs1 | imm;
		if (aluControl == 0b0100) // SRAI
			return rs1 >> imm;
	}
}

Controller::Controller() {
	opcode = 0;
	for (int i = 0; i < 7; i++) {
		controlSignals[i] = 0;
	}
}
void Controller::setOpcode(int o) {
	opcode = o;
}
int Controller::getOpcode() {
	return opcode;
}
void Controller::computeControlSignals() {
	/* 
	[0]: Branch
	[1]: MemRead
	[2]: MemtoReg
	[3]: ALUOp
	[4]: MemWrite
	[5]: AluSrc
	[6]: RegWrite
	[7]: LoadImm
	[8]: Jump
	*/
	if (opcode == 0b0110011) { // R-type 
		controlSignals[0] = 0;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 0b10;
		controlSignals[4] = 0;
		controlSignals[5] = 0;
		controlSignals[6] = 1;
		controlSignals[7] = 0;
		controlSignals[8] = 0;
	} else if (opcode == 0b0010011) { // I-type
		// cout << "Computing I-type control signals!" << endl;
		controlSignals[0] = 0;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 0b10;
		controlSignals[4] = 0;
		controlSignals[5] = 1;
		controlSignals[6] = 1;
		controlSignals[7] = 0;
		controlSignals[8] = 0;
	} else if (opcode == 0b0000011) { // Load instructions
		controlSignals[0] = 0;
		controlSignals[1] = 1;
		controlSignals[2] = 1;
		controlSignals[3] = 0;
		controlSignals[4] = 0;
		controlSignals[5] = 1;
		controlSignals[6] = 1;
		controlSignals[7] = 0;
		controlSignals[8] = 0;
	} else if (opcode == 0b0100011) { // Store instructions
		controlSignals[0] = 0;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 0;
		controlSignals[4] = 1;
		controlSignals[5] = 1;
		controlSignals[6] = 0;
		controlSignals[7] = 0;
		controlSignals[8] = 0;
	} else if (opcode == 0b1100011) { // Branch instructions
		controlSignals[0] = 1;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 1;
		controlSignals[4] = 0;
		controlSignals[5] = 0;
		controlSignals[6] = 0;
		controlSignals[7] = 0;
		controlSignals[8] = 0;
	} else if (opcode == 0b0110111) { // LUI
		controlSignals[0] = 0;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 0;
		controlSignals[4] = 0;
		controlSignals[5] = 1;
		controlSignals[6] = 1;
		controlSignals[7] = 1;
		controlSignals[8] = 0;
	} else if (opcode == 0b1101111) { // JAL
		controlSignals[0] = 0;
		controlSignals[1] = 0;
		controlSignals[2] = 0;
		controlSignals[3] = 0;
		controlSignals[4] = 0;
		controlSignals[5] = 0;
		controlSignals[6] = 1;
		controlSignals[7] = 0;
		controlSignals[8] = 1;
	}
	else {
		for (int i = 0; i < 9; i++) {
			controlSignals[i] = 0;
		}
	}

}
int Controller::getControlSignal(int ind) {
	return controlSignals[ind];
}

int ALUController::getAluControl(int aluOp, int funct3, int funct7, int aluSrc) {
	if (aluOp == 0b00) 
		return 0b0010;
	else if (aluOp == 0b01) 
		return 0b0110;
	else if (aluOp == 0b10) {
		if (aluSrc == 0) {
			// ADD
			if (funct3 == 0b000 && funct7 == 0b0000000)
				return 0b0010;
			// SUB
			if (funct3 == 0b000 && funct7 == 0b0100000)
				return 0b0110;
			// XOR
			if (funct3 == 0b100)
				return 0b0011;
		} else {
			// ADDI
			if (funct3 == 0b000)
				return 0b0010;
			// ORI
			if (funct3 == 0b110)
				return 0b0001;
			// SRAI ==> NEEDS TO BE IMPLEMENTED! ADD INPUT VALUES FOR SHAMT & SHIFT OP ID??
			if (funct3 == 0b101 && funct7 == 0b0100000)
				return 0b0100;
		}
	}
}

void ImmGen::computeImm(int instr, int funct3, int funct7) {
	//I-type
	if ((instr & 0x7F) == 0b0010011) {
		if (funct3 == 0b101 && funct7 == 0b0100000) {
			imm = (instr >> 20) & 0x1F;
		} else {
			imm = (instr >> 20) & 0xFFF;
			if (imm & 0x800)
                imm |= 0xFFFFF000;
		}
	}
	//S-type
	else if ((instr & 0x7F) == 0b0100011) {
		imm = (((instr >> 25) & 0x7F) << 5) | ((instr >> 7) & 0x1F);
		if (imm & 0x800)
            imm |= 0xFFFFF000;
	}
	//B-type
	else if ((instr & 0x7F) == 0b1100011) {
		imm = ((instr>>31)&0x1)<<12 | ((instr>>7)&0x1)<<11 | ((instr>>25)&0x3F)<<5 | ((instr>>8)&0xF)<<1;
		if (imm & 0x1000)
            imm |= 0xFFFFF000;
	}
	//U-type
	else if ((instr & 0x7F) == 0b0110111) {
		imm = (instr & 0xFFFFF000);
	}
	//J-type
	else if ((instr & 0x7F) == 0b1101111) {
		// cout << "Computing Imm for Jump Instruction!" << endl;
		imm = ((instr>>31)&0x1)<<20 | ((instr>>12)&0xFF)<<12 | ((instr>>20)&0x1)<<11 | ((instr>>21)&0x3FF)<<1;
		if (imm & 0x100000)
            imm |= 0xFFFFF000;
	}
	//Load instructions
	else if ((instr & 0x7F) == 0b0000011) {
		imm = (instr >> 20) & 0xFFF;
		if (imm & 0x800)
            imm |= 0xFFFFF000;
	}
	else if ((instr & 0x7F) == 0b0110011) {
		// cout << "R-type instruction has no immediate!" << endl;
		imm = 0;
	}
	else {
		// cout << "Something is going wrong!!" << endl;
		imm = 0;
	}
}
int ImmGen::getImm() {
	return imm;
}