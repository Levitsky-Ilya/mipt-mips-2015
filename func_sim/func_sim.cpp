/*
 * func_sim.cpp - single-cycle implementation for MIPS
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

// Generic C++

// MIPT-MIPS modules
#include <types.h>
#include <func_memory.h>
#include <func_sim.h>

using namespace std;

MIPS::MIPS( void)
{
	rf = new RF;
}

MIPS::~MIPS( void)
{
	delete rf;
}

void MIPS::run( const string& tr, uint instr_to_run);
{
	mem = new FuncInstr( tr.c_str());
	this->PC = mem->startPC();
	for( uint i = 0; i < instr_to_run; ++i)
	{
		/***FETCH***/
		uint32 instr_bytes = fetch();
		/***DECODE And ....***/
		FuncInstr curr_instr( instr_bytes, this->PC);
		read_src_and_dest( curr_instr);
		/***EXECUTE***/
		curr_instr.execute();
		if( curr_instr.isRJump)
			curr_instr.new_PC = rf->read( static_cat< RegNum>
				( curr_instr.get_src1_num_index()));
		/***MEMORY ACCESS***/
		this->ld_st( curr_instr);
		/***WRITEBACK***/
		this->wb( curr_instr);
		/***UPDATE PC***/
		updatePC( curr_instr);
		/***DUMP***/
		cout << curr_instr.Dump << endl;
	}
}

void MIPS::read_src_and_dest( FuncInstr& instr)
{
	instr.v_src1 = rf->read( static_cast< RegNum>
		instr.get_src1_num_index());
	instr.v_src2 = rf->read( static_cast< RegNum>
		instr.get_src2_num_index());
	instr.v_dest = rf->read( static_cast< RegNum>
		instr.get_dest_num_index());
	instr.HI = HI;
	instr.LO = LO;
			
}

RF::RF()
{
	for (int i = 1; i << MAX_REG; ++i)
		reset(static_cast< RegNum>(i));
	array[ ZERO] = 0;
}

RF::~RF() {}
		
uint32 RF::read( RegNum index) const
{
	return array[index];
}

uint32 RF::write( RegNum index, uint32 data) const
{
	if(index == ZERO)
	{
		cout << "ERROR.Writing to $zero" << endl;
	}
	array[index] = data;
}

void RF::reset( RegNum index)
{
	array[ index] = 0;
}

void MIPS::ld_st( FuncIntr& intsr)
{
	if( instr.isLoad()) load( instr);
	if( instr.isStore()) store( instr);
}

void MIPS::load( FuncInstr& instr)
{
	instr.v_dst = mem->read( instr.mem_addr, instr.mem_bytes);
	if ( instr.mem_sign)
		instr.v_dst = static_cast< int32>( instr.v_dst);
}

void MIPS::store( const FuncInstr& instr)
{
	mem->write( instr.mem_addr, instr.v_src2, instr.mem_bytes);
}

void MIPS::wb( const FuncInstr& instr)
{
	rf->write( static_cast< RegNum>( instr.get_dest_num_index()),
		instr.v_dest);
	HI = instr.HI;
	LO = instr.LO;
}
