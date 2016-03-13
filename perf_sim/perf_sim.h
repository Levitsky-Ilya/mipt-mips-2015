/**
 * func_sim.cpp - mips single-cycle simulator
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS 
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <assert.h>
#include <func_instr.h>
#include <func_memory.h>
#include <ports.h>
#include <rf.h>

class Fetch;
class Decode;
class Execute;
class Memory;
class Writeback;

struct Reg
{
	uint32 value;
	bool isValid;
	Reg() : value(0), isValid(true)
	{}
};

class PerfMIPS
{
    private:
        RF* rf;
        Reg PC;
        FuncMemory* mem;
		
		Fetch* fetch;
		Decode* decode;
		Execute* execute;
		Memory* memory;
		Writeback* writeback;

   public:
        PerfMIPS();
        void run(const std::string& tr, uint32 instrs_to_run, bool silent = true);
        ~PerfMIPS();
        
};

class Fetch
{
	public:
	    void clock( Reg& PC, uint32 cycle, bool silent);
	    Fetch( const FuncMemory* mem);
	    ~Fetch();
	    uint32 fetch();
	    void init_ports();
	private:
		const FuncMemory* mem_;
		uint32 instr_code;
		ReadPort< bool>*    rp_decode_2_fetch_stall;
		WritePort< uint32>*  wp_fetch_2_decode;
};

class Decode
{
	public:
	    void clock( uint32 PC, uint32 cycle, bool silent);
	    Decode( RF *rf);
	    ~Decode();
	    void read_src(FuncInstr& instr) const ;
	    void init_ports();
	private:
		RF* rf_;
		uint32 instr_code;
		FuncInstr instr;
		std::queue<uint32> queueDecode;
		ReadPort< bool>*        rp_execute_2_decode_stall;
		ReadPort< uint32>*      rp_fetch_2_decode;
		WritePort< bool>*	   wp_decode_2_fetch_stall;
		WritePort< FuncInstr>*  wp_decode_2_execute;
};

class Execute
{
	public:
		void clock( uint32 cycle, bool silent);
		Execute();
		~Execute();
		void init_ports();
	private:
		FuncInstr instr;
		ReadPort< bool>*        rp_memory_2_execute_stall;
		ReadPort< FuncInstr>*   rp_decode_2_execute;
		WritePort< bool>*       wp_execute_2_decode_stall;
		WritePort< FuncInstr>*  wp_execute_2_memory;
};

class Memory
{
	public:
	    void clock( uint32 cycle, bool silent);
	    Memory( FuncMemory* mem);
	    ~Memory();
	    void load(FuncInstr& instr) const;
        void store(const FuncInstr& instr);
	    void load_store(FuncInstr& instr);
	    void init_ports();
	private:
		FuncMemory* mem_;
		FuncInstr instr;
		ReadPort< bool>*        rp_writeback_2_memory_stall;
		ReadPort< FuncInstr>*   rp_execute_2_memory;
		WritePort< bool>*       wp_memory_2_execute_stall;
		WritePort< FuncInstr>*  wp_memory_2_writeback;
};

class Writeback
{
	public:
		bool clock( uint32 cycle, bool silent);
		Writeback( Reg* PC, RF* rf);
		~Writeback();
	    void wb(const FuncInstr& instr);
		void init_ports();
	private:
		Reg* PC_;
		RF* rf_;
		FuncInstr instr;
		ReadPort< FuncInstr>*   rp_memory_2_writeback;
		WritePort< bool>*       wp_writeback_2_memory_stall;
};
            
#endif
