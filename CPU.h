#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;

// class instruction { // optional
// public:
// 	bitset<32> instr; //instruction
// 	instruction(bitset<32> fetch); // constructor

// };

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 

public:
	CPU();
	unsigned long readPC();
	void incPC();
	void setPC(int update);
	int dmemRead(int address, int funct3);
	void dmemWrite(int address, int data, int funct3);
	u_int32_t fetchInstruction(u_int32_t instMem[], int PC);
};

// add other functions and objects here
class RegFile { 
private:
	int regWrite;
	int writeRegister;

	int registerValues[32]; 
public: 
	RegFile();
	void setRegWrite(int rW);
	void setWriteRegister(int rd);
	int readRegister1(int rs1);
	int readRegister2(int rs2);
	void writeDatatoReg(int data);
};
class ALU { 
private: 
	int rs1;
	int rs2;
	int imm; 
	int aluSrc;
	int aluControl; 
	// int addr(int rs1, int rs2);
	// int addi(int rs1, int imm);
	// int ori(int rs1, int imm);
	// int xorr(int rs1, int rs2);
public: 
	void setAluSrc(int aS); 
	void setAluInputs(int i1, int i2);
	void setAluControl(int aC);
	int output();
};
class Controller {
private: 
	int opcode; 
	int controlSignals[9]; 
	/* 
	[0]: Branch
	[1]: MemRead
	[2]: MemtoReg
	[3]: ALUOp
	[4]: MemWrite
	[5]: AluSrc
	[6]: RegWrite
	[7]: LoadImm ** NEW CONTROL SIGNAL **
	[8]: Jump ** NEW CONTROL SIGNAL **
	*/

public: 
	Controller();
	void setOpcode(int o);
	int getOpcode();
	void computeControlSignals();
	int getControlSignal(int ind);
};
class ALUController {
// private: 
// 	int aluOp;
public: 
	// void setAluOp(int aO);
	int getAluControl(int aluOp, int funct3, int funct7, int aluSrc);
};
class ImmGen { 
private: 
	int imm;
public: 
	void computeImm(int instr, int funct3, int funct7);
	int getImm();
};