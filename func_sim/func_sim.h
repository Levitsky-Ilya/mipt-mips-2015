/*
 * func_sim.h
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#ifndef FUNC_SIM_H
#define FUNC_SIM_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class MIPS {
          // storages of internal state
          RF* rf;
          uint32 PC;
          uint32 HI;
          uint32 LO;
          FuncMemory* mem;
          
          void read_src( FuncIntsr& instr);
          void load( FuncInstr& instr);
          void store( const FuncInstr& instr);
          void ld_st( const FuncInstr& instr);
     public:
          MIPS();
          ~MIPS();
          void run( const string&, uint instr_to_run);
          uint32 fetch() const { return mem->read( PC);}
          void updatePC( const FuncInstr& instr) { PC = instr.new_PC; }
};

enum RegNum {
      ZERO,
      AT,
      V0, V1,
      A0, A1, A2, A3,
      T0, T1, T2, T3, T4, T5, T6, T7,
      S0, S1, S2, S3, S4, S5, S6, S7,
      T8, T9,
      K0, K1,
      GP, SP,
      S8, RA,
      MAX_REG
};

class RF {
		uint32 array[MAX_REG];
	public:
		RF();
		~RF();
		uint32 read( RegNum index) const;
		void write( RegNum index, uint32 data);
		void reset( RegNum index); // clears register to 0 value
};

#endif //FUNC_SIM_H
