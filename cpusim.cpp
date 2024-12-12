#include "CPU.h"

#include <iostream>
#include  <iomanip>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	u_int32_t instMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
			infile>>line;
			stringstream line2(line);
			char x; 
			line2>>x;
			
			char byte[3]; 
			byte[2] = '\0';
			// instMem[i][0] = x; // be careful about hex
			byte[0] = x;

			i++;
			line2>>x;
			byte[1] = x;
			// instMem[i][1] = x; // be careful about hex
			instMem[i/2] = strtol(byte, NULL, 16);;

			// cout << "IM:" << hex << instMem[i/2] << endl;
			i++;
		}
	int maxPC= i/2; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU = CPU();  // call the appropriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 

	RegFile regFile = RegFile();
	ALU alu;
	Controller controller;
	ALUController aluController;
	ImmGen immGen;

	// for (int i = 0; i < 4096; i++) {
	// 	cout << "IM: " << instMem[i] << endl;;
	// }

	// cout << "Registers: [";
	// for (int i = 0; i < 32; i++) {
	// 	cout << dec << regFile.readRegister1(i) << ", ";
	// }
	// cout << "]" << endl;
	
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		// fetch
		if (myCPU.readPC() > maxPC)
			break;
		u_int32_t instruction = myCPU.fetchInstruction(instMem, myCPU.readPC());

		// cout << "Instruction: " << setfill('0') << setw(8) << hex << instruction << endl;

		// decode
		controller.setOpcode(instruction & 0x7F);
		controller.computeControlSignals();

		// cout << "Opcode: " << hex << controller.getOpcode() << endl;

		int funct3 = (instruction >> 12) & 0x7;; 
		int funct7 = (instruction >> 25) & 0x7F;

		// cout << "Funct3: " << hex << funct3 << endl;
		// cout << "Funct7: " << hex << funct7 << endl;

		immGen.computeImm(instruction, funct3, funct7);
		
		int rs1 = (instruction >> 15) & 0x1F;
		int rs2 = (instruction >> 20) & 0x1F;
		int rd = (instruction >> 7) & 0x1F;

		// cout << "rs1: " << dec << rs1 << endl;
		// cout << "rs2: " << dec << rs2 << endl;
		// cout << "rd: " << dec << rd << endl;
		// cout << "Imm: " << hex << "0x" << immGen.getImm() << endl;
		// cout << "Imm: " << dec << immGen.getImm() << endl;

		rs1 = regFile.readRegister1(rs1);
		rs2 = regFile.readRegister2(rs2); 

		// cout << "Value of rs1: " << dec << rs1 << endl;
		// cout << "Value of rs2: " << dec << rs2 << endl;

		regFile.setRegWrite(controller.getControlSignal(6));
		regFile.setWriteRegister(rd);
		
		// execution
		int aluSrc = controller.getControlSignal(5);
		int aluCtrl = aluController.getAluControl(controller.getControlSignal(3), funct3, funct7, aluSrc);

		// cout << "ALU Src: " << aluSrc << endl;
		// cout << "ALU Control: " << aluCtrl << endl;

		alu.setAluControl(aluCtrl);
		alu.setAluSrc(aluSrc);
		if (aluSrc == 0) {
			// cout << "Using Register Input2!" << endl;
			alu.setAluInputs(rs1, rs2);
		} else {
			// cout << "Using Immediate Input2!" << endl;
			alu.setAluInputs(rs1, immGen.getImm());
		}
		
		int aluOut = alu.output();
		if (controller.getControlSignal(7) == 1) {
			aluOut = immGen.getImm();
		} 

		// cout << "ALU Output: " << dec << aluOut << endl;

		int branch = controller.getControlSignal(0);
		int jump = controller.getControlSignal(8);
		if (branch == 1 && aluOut == 0) {
			// cout << "Branch instruction: Setting PC to " << immGen.getImm() << endl;
			myCPU.setPC(immGen.getImm());
		} else if (jump == 1) {
			// cout << "Jump Instruction: Setting PC to " << immGen.getImm() << endl;
			aluOut = myCPU.readPC() + 4;
			myCPU.setPC(immGen.getImm());
		} else 
			myCPU.incPC();

		// memory
		int regOut = aluOut;

		int memRead = controller.getControlSignal(1);
		if (memRead == 1) {
			regOut = myCPU.dmemRead(aluOut, funct3);
		}
		int memWrite = controller.getControlSignal(4);
		if (memWrite == 1) {
			myCPU.dmemWrite(aluOut, rs2, funct3);
		}

		// write back
		int regWrite = controller.getControlSignal(6); 
		if (regWrite == 1) {
			regFile.writeDatatoReg(regOut);
		}

		// break;
		// cout << "Registers: [";
		// for (int i = 0; i < 32; i++) {
		// 	cout << dec << regFile.readRegister1(i) << ", ";
		// }
		// cout << "]" << endl;
		// cout << endl;

		if (myCPU.readPC() > maxPC)
			break;

	}
	int a0 = regFile.readRegister1(10);
	int a1 = regFile.readRegister2(11);  
	cout << "(" << a0 << "," << a1 << ")" << endl;
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	
	return 0;

}