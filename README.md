# Single-cycle Processor in C++ 

## Datapath: 
```
1. Data Memory ⇒ implemented inside the CPU; added data memory read and write functions/ports inside CPU object, accounting for little-endian byte addressability
2. Register File ⇒ requires read and write ports; rs1 is always read, but use the AluSrc control signal to determine whether rs2 or immediate (output of ImmGen) is used as input to the ALU operation
3. ALU ⇒ takes in the two input values (rs1, rs2 or imm – junk value for the one that is not used), aluControl output, aluSrc control signal, and performs the necessary computations based on the provided inputs
4. Immediate Generator ⇒ takes in the instruction and extracts the immediate based on the opcode (and in the case of SRAI, the funct3 and shamt values)
5. Controller ⇒ requires control signals to determine reading/writign from data memory, writing to register file, branching, selecting ALU operations, etc. ⇒ added 2 new control signals to handle LUI (specifically to write the computed immediate value to the register-file, instead of the the junk output of the ALU) and JAL (to jump unconditionally and store previous value of PC to the register file).
6. ALU Controller ⇒ determines from the ALUOp, ALUSrc, etc. the specific operation being utilized ⇒ makes the ALU Unit much more generalizable.