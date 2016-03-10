/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

<<<<<<< HEAD
#include <func_instr.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>

using namespace std;

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation      micromethod
    { "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::add},
    { "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::add},
    { "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sub},
    { "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sub},
    { "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::addi},
    { "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::addi},
    { "mult",   0x0, 0x18,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mul},    
    { "multu",  0x0, 0x19,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mul},
    { "div",    0x0, 0x1A,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::div},
    { "divu",   0x0, 0x1B,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::div},
    { "mfhi",   0x0, 0x10,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mfhi},
    { "mthi",   0x0, 0x11,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mthi},
    { "mflo",   0x0, 0x12,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mflo},
    { "mtlo",   0x0, 0x13,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::mtlo},
    { "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::sll},
    { "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::srl},
    { "sra",    0x0, 0x3,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::sra},
    { "sllv",   0x0, 0x4,   FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sll},
    { "srlv",   0x0, 0x6,   FORMAT_R, OUT_R_ARITHM,  &FuncInstr::srl},
    { "srav",   0x0, 0x7,   FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sra},
    { "lui",    0xF, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::lui},
    { "slt",    0x0, 0x2A,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::less},
    { "sltu",   0x0, 0x2B,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::less},
    { "slti",   0xA, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::lessi},
    { "sltiu",  0xB, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::band},
    { "or",     0x0, 0x25,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::bor},
    { "xor",    0x0, 0x26,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::bxor},
    { "nor",    0x0, 0x27,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::bnor},
    { "andi",   0xC, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::bandi},
    { "ori",    0xD, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::bori},
    { "xori",   0xE, 0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::bxori},
    { "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::beq},
    { "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::bne},
    { "blez",   0x6, 0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::blez},
    { "bgtz",   0x7, 0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::bgtz},
    { "jal",    0x3, 0x0,   FORMAT_J, OUT_J_JUMP,    &FuncInstr::jal},
    { "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP,    &FuncInstr::j},
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP,    &FuncInstr::jr},
    { "jalr",   0x0, 0x9,   FORMAT_R, OUT_R_JUMP,    &FuncInstr::jalr},
    { "lb",     0x20,0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::lb},
    { "lh",     0x21,0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::lh},
    { "lw",     0x23,0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::lw},
    { "lbu",    0x24,0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::lbu},
    { "lhu",    0x25,0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::lhu},
    { "sb",     0x28,0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::sb},
    { "sh",     0x29,0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::sh},
    { "sw",     0x2b,0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::sw},
    { "syscall",0x0, 0xC,   FORMAT_R, OUT_R_SPECIAL, &FuncInstr::syscall},
    { "break",  0x0, 0xD,   FORMAT_R, OUT_R_SPECIAL, &FuncInstr::breaking},
    { "trap",   0x1A,0x0,   FORMAT_J, OUT_J_SPECIAL, &FuncInstr::trap},
};
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

const char *FuncInstr::regTable[] = 
=======
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <func_instr.h>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name    funct    format    operation   memsize           pointer
    { "add",    0x20,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_add},
    { "addu",   0x21,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_addu},
    { "sub",    0x22,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_sub},
    { "subu",   0x23,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_subu},
    { "addi",   0x8,   FORMAT_I, OUT_I_ARITHM,  0, &FuncInstr::execute_addi},
    { "addiu",  0x9,   FORMAT_I, OUT_I_ARITHM,  0, &FuncInstr::execute_addiu},

    // name    funct    format    operation   memsize           pointer
    { "mult",   0x18,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_mult}, 
    { "multu",  0x19,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_multu},

    // name    funct    format    operation   memsize           pointer
    { "div",    0x1A,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_div},
    { "divu",   0x1B,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_divu},

    // name    funct    format    operation   memsize           pointer
    { "mfhi",   0x10,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_mfhi},
    { "mflo",   0x12,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_mflo},
    { "mthi",   0x11,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_mthi},
    { "mtlo",   0x13,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_mtlo},
    
    // name    funct    format    operation   memsize           pointer
    { "sll",    0x0,   FORMAT_R, OUT_R_SHAMT,   0, &FuncInstr::execute_sll},
    { "srl",    0x2,   FORMAT_R, OUT_R_SHAMT,   0, &FuncInstr::execute_srl},
    { "sra",    0x3,   FORMAT_R, OUT_R_SHAMT,	0, &FuncInstr::execute_sra},
    { "sllv",   0x4,   FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_sllv},
    { "srlv",   0x6,   FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_srlv},
    { "srav",   0x7,   FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_srav},
    { "lui",    0xF,   FORMAT_I, OUT_I_CONST,   0, &FuncInstr::execute_lui},
    { "slt",    0x2A,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_slt},
    { "sltu",   0x2B,  FORMAT_R, OUT_R_ARITHM,	0, &FuncInstr::execute_sltu},
    { "slti",   0xA,   FORMAT_I, OUT_I_ARITHM,	0, &FuncInstr::execute_slti},
    { "sltiu",  0xB,   FORMAT_I, OUT_I_ARITHM,	0, &FuncInstr::execute_sltiu},

    // name    funct    format    operation   memsize           pointer
    { "and",    0x24,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_and},
    { "or",     0x25,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_or},
    { "xor",    0x26,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_xor},
    { "nor",    0x27,  FORMAT_R, OUT_R_ARITHM,  0, &FuncInstr::execute_nor},
    
    // name    funct    format    operation   memsize           pointer
    { "andi",   0xC,   FORMAT_I, OUT_I_ARITHM,  0, &FuncInstr::execute_andi},
    { "ori",    0xD,   FORMAT_I, OUT_I_ARITHM,  0, &FuncInstr::execute_ori},
    { "xori",   0xE,   FORMAT_I, OUT_I_ARITHM,  0, &FuncInstr::execute_xori},
    
    // name    funct    format    operation   memsize           pointer
    { "beq",    0x4,   FORMAT_I, OUT_I_BRANCH,  0, &FuncInstr::execute_beq},
    { "bne",    0x5,   FORMAT_I, OUT_I_BRANCH,  0, &FuncInstr::execute_bne},
    { "blez",   0x6,   FORMAT_I, OUT_I_BRANCH,  0, &FuncInstr::execute_blez},
    { "bgtz",   0x7,   FORMAT_I, OUT_I_BRANCH,  0, &FuncInstr::execute_bgtz},
    { "jal",    0x3,   FORMAT_J, OUT_J_JUMP,    0, &FuncInstr::execute_jal},
    { "j",      0x2,   FORMAT_J, OUT_J_JUMP,    0, &FuncInstr::execute_j},
    { "jr",     0x8,   FORMAT_R, OUT_R_JUMP,    0, &FuncInstr::execute_jr},
    { "jalr",   0x9,   FORMAT_R, OUT_R_JUMP,    0, &FuncInstr::execute_jalr},
    
    // name    funct    format    operation   memsize           pointer
    { "lb",     0x20,  FORMAT_I, OUT_I_LOAD,    1, &FuncInstr::calculate_load_addr},
    { "lbu",    0x24,  FORMAT_I, OUT_I_LOADU,   1, &FuncInstr::calculate_load_addr},
    { "lh",     0x21,  FORMAT_I, OUT_I_LOAD,    2, &FuncInstr::calculate_load_addr},
    { "lhu",    0x25,  FORMAT_I, OUT_I_LOADU,   2, &FuncInstr::calculate_load_addr},
    { "lw",     0x23,  FORMAT_I, OUT_I_LOAD,    4, &FuncInstr::calculate_load_addr},
    
    // name    funct    format    operation   memsize           pointer
    { "sb",     0x28,  FORMAT_I, OUT_I_STORE,   1, &FuncInstr::calculate_store_addr},
    { "sh",     0x29,  FORMAT_I, OUT_I_STORE,   2, &FuncInstr::calculate_store_addr},
    { "sw",     0x2B,  FORMAT_I, OUT_I_STORE,   4, &FuncInstr::calculate_store_addr},
    
    // name    funct    format    operation   memsize           pointer
    { "break",  0xD,   FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_break},
    { "syscall",0xC,   FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_syscall},
    { "trap",   0x1A,  FORMAT_J, OUT_J_SPECIAL, 0, &FuncInstr::execute_trap},
};                                              
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

const char *FuncInstr::regTable[REG_NUM_MAX] = 
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", 
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

<<<<<<< HEAD
FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), PC( PC)
{
=======
const char *FuncInstr::regTableName(RegNum reg) {
    return regTable[static_cast<size_t>(reg)];
}

FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr(bytes), PC(PC)
{
    src1 = src2 = dst = REG_NUM_ZERO;
    complete = false;
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
    initFormat(); 
    switch ( format)
    {
        case FORMAT_R:
            initR();
            break;
        case FORMAT_I:
            initI();
            break;
        case FORMAT_J:
            initJ();
            break;
        case FORMAT_UNKNOWN:
            initUnknown();
            break;
    }
<<<<<<< HEAD
}

string FuncInstr::Dump( string indent) const
{
    return indent + disasm;
=======
    new_PC = PC + 4;
}

std::string FuncInstr::Dump( std::string indent) const
{
    ostringstream oss;
    oss << indent << disasm;
    
    if ( dst != REG_NUM_ZERO && complete)
    {
        oss << "\t [ $" << regTableName(dst) 
            << " = 0x" << std::hex << v_dst << "]" << std::dec;
    }
 
    return oss.str();
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
}

void FuncInstr::initFormat()
{
<<<<<<< HEAD
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            operation = isaTable[i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
=======
    bool is_R = ( instr.asR.opcode == 0x0);
    uint8 ident = is_R ? instr.asR.funct : instr.asR.opcode;

    for ( size_t i = 0; i < isaTableSize; i++)
    {
        bool is_i_R = ( isaTable[i].format == FORMAT_R);
        if ( isaTable[i].opcode == ident && ( is_i_R == is_R))
        {
            format    = isaTable[i].format;
            operation = isaTable[i].operation;
            mem_size  = isaTable[i].mem_size;
            isaNum    = i;
            if ( FORMAT_R == format)
               assert( instr.asR.opcode == 0x0);
            return;
        }
    }
    initUnknown();
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
}


void FuncInstr::initR()
{
<<<<<<< HEAD
    // find instr by functor
    for (isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[isaNum].opcode) &&
            ( instr.asR.funct == isaTable[isaNum].funct))
        {
            operation = isaTable[isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't find funct
    {
        initUnknown();
        return;
    }
    
    ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_R_ARITHM:
			oss  << regTable[instr.asR.rd] << ", $"
                 << regTable[instr.asR.rs] << ", $"
                 << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            oss << regTable[instr.asR.rd] << ", $" 
                << regTable[instr.asR.rt] << ", " 
                << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << regTable[instr.asR.rs];
=======
    ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_R_ARITHM:
            src2 = (RegNum)instr.asR.rt;
            src1 = (RegNum)instr.asR.rs;
            dst  = (RegNum)instr.asR.rd;

            oss <<  " $" << regTableName(dst) 
                << ", $" << regTableName(src1)
                << ", $" << regTableName(src2);
            break;
        case OUT_R_SHAMT:
            src1  = (RegNum)instr.asR.rs;
            dst   = (RegNum)instr.asR.rd;
            v_imm = instr.asR.shamt;

            oss <<  " $" << regTableName(dst)
                << ", $" << regTableName(src1)
                <<  ", " << dec << v_imm;
            break;
        case OUT_R_JUMP:
            src1  = (RegNum)instr.asR.rs;

            oss << " $" << regTableName(src1);
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
            break;
        case OUT_R_SPECIAL:
            break;
    }
    disasm = oss.str();
}


void FuncInstr::initI()
{
<<<<<<< HEAD
    ostringstream oss;
=======
    v_imm = instr.asI.imm;

    std::ostringstream oss;
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
<<<<<<< HEAD
            oss << regTable[instr.asI.rt] << ", $"
                << regTable[instr.asI.rs] << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << dec << "($" << regTable[instr.asI.rs] << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << dec << "($" << regTable[instr.asI.rs] << ")";
=======
            src1 = (RegNum)instr.asI.rs;
            dst  = (RegNum)instr.asI.rt;

            oss << regTable[dst] << ", $"
                << regTable[src1] << ", "
                << std::hex << "0x" << v_imm << std::dec;
            
            break;
        case OUT_I_BRANCH:
            src1 = (RegNum)instr.asI.rs;
            src2 = (RegNum)instr.asI.rt;

            oss << regTable[src1] << ", $"
                << regTable[src2] << ", "
                << std::hex << "0x" << v_imm << std::dec;
            break;

        case OUT_I_CONST:
            dst  = (RegNum)instr.asI.rt;

            oss << regTable[dst] << std::hex 
                << ", 0x" << v_imm << std::dec;
            break;

        case OUT_I_LOAD:
        case OUT_I_LOADU:
            src1 = (RegNum)instr.asI.rs;
            dst  = (RegNum)instr.asI.rt;
            
            oss << regTable[dst] << ", 0x"
                << std::hex << v_imm
                << "($" << regTable[src1] << ")" << std::dec;
            break;
        
        case OUT_I_STORE:
            src2 = (RegNum)instr.asI.rt;
            dst  = (RegNum)instr.asI.rs;
            
            oss << regTable[dst] << ", 0x"
                << std::hex << v_imm
                << "($" << regTable[src2] << ")" << std::dec;
>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
<<<<<<< HEAD
    ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_J_JUMP:
            oss << " " << hex << "0x" << instr.asJ.imm;
            break;
        case OUT_J_SPECIAL:
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    ostringstream oss;
    oss << hex << setfill( '0')
        << "0x" << setw( 8) << instr.raw << '\t' << "Unknown" << endl;
    disasm = oss.str();
    cerr << "ERROR.Incorrect instruction: " << disasm << endl;
    exit( EXIT_FAILURE);
}

int FuncInstr::get_src1_num_index() const
{
	switch ( operation)
    {
        case OUT_R_ARITHM:
			return instr.asR.rs;
        case OUT_R_SHAMT:
			return instr.asR.rt;
        case OUT_R_JUMP:
			return instr.asR.rs;
        case OUT_I_ARITHM:
			return instr.asI.rs;
        case OUT_I_BRANCH:
			return instr.asI.rs;
        case OUT_I_LOAD:
			return instr.asI.rs;
        case OUT_I_STORE:
			return instr.asI.rs;
		default:
			return -1;
    }
}

int FuncInstr::get_src2_num_index() const
{
	switch ( operation)
    {
        case OUT_R_ARITHM:
			return instr.asR.rt;
        case OUT_I_BRANCH:
			return instr.asI.rt;
        case OUT_I_STORE:
			return instr.asI.rt;
		default:
			return -1;
    }
}

int FuncInstr::get_dest_num_index() const
{
	switch ( operation)
    {
        case OUT_R_ARITHM:
			return instr.asR.rd;
        case OUT_R_SHAMT:
			return instr.asR.rd;
		case OUT_R_JUMP:
			return 31;
        case OUT_I_ARITHM:
			return instr.asI.rt;
		case OUT_I_LOAD:
			return instr.asI.rt;
		case OUT_J_JUMP:
			return 31;	//ra
		default:
			return -1;
    }
}

void FuncInstr::execute()
{
	(this->*isaTable[isaNum].func)();
	this->setNewPC();
	this->setDump();
}

void FuncInstr::setNewPC()
{
	this->new_PC = this->PC + 4;
	if( operation == OUT_I_BRANCH && brch_taken)
	{
		this->new_PC += instr.asI.imm << 2;
	}
	if( operation == OUT_J_JUMP)
	{
		this->new_PC = (this->PC & 0xf0000000) | (instr.asJ.imm << 2);
	}
}

void FuncInstr::setDump()
{
	ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
		case OUT_R_ARITHM:
            oss << " $" << regTable[instr.asR.rd] << '[' << v_dst << ']'
				<< ", $"<< regTable[instr.asR.rs] << '[' << v_src1 << ']'
				<< ", $"<< regTable[instr.asR.rt] << '[' << v_src2 << ']';
            break;
        case OUT_R_SHAMT:
            oss << " $" << regTable[instr.asR.rd] << '[' << v_dst << ']'
				<< ", $"<< regTable[instr.asR.rt] << '[' << v_src1 << ']'
                << ", " << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << " $" << regTable[instr.asR.rs] << '[' << v_src1 << ']';
            break;
        case OUT_R_SPECIAL:
            break;
        case OUT_I_ARITHM:
            oss << regTable[instr.asI.rt] << '[' << v_dst << ']'
				<< ", $"<< regTable[instr.asI.rs] << '[' << v_src1 << ']'
                << ", " << hex << "0x" << static_cast< signed int>
                ( instr.asI.imm) << dec;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << '[' << v_src1 << ']' << ", $"
				<< regTable[instr.asI.rt] << '[' << v_src2 << ']' << ", " 
				<< hex << "0x" << static_cast< signed int>
                ( instr.asI.imm) << dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << '[' << v_dst << ']' << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << dec << "($" << regTable[instr.asI.rs]
                << '[' << v_src1 << ']' << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << '[' << v_src2 << ']' << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << dec << "($" << regTable[instr.asI.rs]
                << '[' << v_src1 << ']' << ")";
            break;
        case OUT_J_JUMP:
            oss << " " << hex << "0x" <<instr.asJ.imm;
            break;
        case OUT_J_SPECIAL:
            break;
    }
    disasm = oss.str();
}

ostream& operator<< ( ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}
=======
    dst   = REG_NUM_ZERO;
    v_imm = instr.asJ.imm;

    std::ostringstream oss;
    oss << isaTable[isaNum].name << " "
        << std::hex << "0x" <<instr.asJ.imm;
    disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

>>>>>>> e96268d10ec83fcf7698c7d8d5149b7820db4594
