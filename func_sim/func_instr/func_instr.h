/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <iostream>
#include <cassert>
#include <string>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>


class FuncInstr
{
public:
    FuncInstr( uint32 bytes, uint32 PC = 0);
    std::string Dump( std::string indent = " ") const;
    int get_src1_num_index() const;
    int get_src2_num_index() const;
    int get_dest_num_index() const;
    
    int isLoad() const { return operation == OUT_I_LOAD;}
    int isStore() const { return operation == OUT_I_STORE;}
    int isRJump() const { return operation == OUT_R_JUMP;}
    
	void execute();
	void setDump();
	void setNewPC();
    
	uint32 v_src1;
	uint32 v_src2;
	uint32 v_dst;
	uint32 mem_addr;
	uint32 HI;
	uint32 LO;
	uint8 mem_bytes;
	int mem_sign;
	uint32 new_PC;
	const uint32 PC;

private:
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J,
        FORMAT_UNKNOWN
    } format;

    enum OperationType
    {
        OUT_R_ARITHM,
        OUT_R_SHAMT,
        OUT_R_JUMP,
        OUT_R_SPECIAL,
        OUT_I_ARITHM,
        OUT_I_BRANCH,
        OUT_I_LOAD,
        OUT_I_STORE,
        OUT_J_JUMP,
        OUT_J_SPECIAL
    } operation;

    union _instr
    {
        struct
        {
            unsigned funct  :6;
            unsigned shamt  :5;
            unsigned rd     :5;
            unsigned rt     :5;
            unsigned rs     :5;
            unsigned opcode :6;
        } asR;
        struct
        {
            unsigned imm    :16;
            unsigned rt     :5;
            unsigned rs     :5;
            unsigned opcode :6;
        } asI;
        struct
        {
            unsigned imm    :26;
            unsigned opcode :6;
        } asJ;
        uint32 raw;

        _instr(uint32 bytes) {
             raw = bytes;
        }
    } instr;

    struct ISAEntry
    {
        std::string name;

        uint8 opcode;
        uint8 funct;

        Format format;
        OperationType operation;
        void (FuncInstr::*func)(void);
    };
    uint32 isaNum;

    static const ISAEntry isaTable[];
    static const uint32 isaTableSize;
    static const char *regTable[];
    
	int brch_taken;

    std::string disasm;
                                                               
    void initFormat();
    void initR();
    void initI();
    void initJ();
    void initUnknown();

	void add()		{ v_dst = v_src1 + v_src2;}
	void addi()		{ v_dst = v_src1 + instr.asI.imm;}
	void sub()		{ v_dst = v_src1 - v_src2;}
	void mul()		{ LO = ( v_src1 * v_src2) & 0xffffffff;
						HI = ( static_cast< uint64>(v_src1) *
						static_cast< uint64>(v_src2)) >> 0x20;}
	void div()		{ LO = v_src1 / v_src2; HI = v_src1 % v_src2;}
	void mfhi()		{ v_dst = HI;}
	void mthi()		{ HI = v_src1;}
	void mflo()		{ v_dst = LO;}
	void mtlo()		{ LO = v_src1;}
	void sllv()		{ v_dst = v_src1 << v_src2;}
	void srlv()		{ v_dst = v_src1 >> v_src2;}
	void srav()		{ v_dst = (int32)v_src1 >> (int32)v_src2;}
	void sll()		{ v_dst = v_src1 << instr.asR.shamt;}
	void srl()		{ v_dst = v_src1 >> instr.asR.shamt;}
	void sra()		{ v_dst = (int32)v_src1 >> (int32)instr.asR.shamt;}
	void lui()		{ v_dst = instr.asI.imm << 16;}
	void less()		{ v_dst = v_src1 < v_src2;}
	void lessi()	{ v_dst = v_src1 < instr.asI.imm;}
	void band()		{ v_dst = v_src1 && v_src2;}
	void bandi()	{ v_dst = v_src1 && instr.asI.imm;}
	void bor()		{ v_dst = v_src1 || v_src2;}
	void bori()		{ v_dst = v_src1 || instr.asI.imm;}
	void bxor()		{ v_dst = v_src1 ^ v_src2;}
	void bxori()	{ v_dst = v_src1 ^ instr.asI.imm;}
	void bnor()		{ v_dst = !(v_src1 || v_src2);}
	void beq()		{ brch_taken = ( instr.asI.rs == instr.asI.rt);}
	void bne()		{ brch_taken = ( instr.asI.rs != instr.asI.rt);}
	void blez()		{ brch_taken = ( instr.asI.rs <= 0);}
	void bgtz()		{ brch_taken = ( instr.asI.rs > 0);}
	void j()		{ }
	void jal()		{ v_dst = PC + 4;}
	void jr()		{ }
	void jalr()		{ v_dst = PC + 4;}
	void clc_addr()	{ mem_addr = instr.asI.rs + instr.asI.imm;}
	void lb()		{ this->clc_addr(); mem_bytes = 1; mem_sign = 1;}
	void lh()		{ this->clc_addr(); mem_bytes = 2; mem_sign = 1;}
	void lw()		{ this->clc_addr(); mem_bytes = 4; mem_sign = 1;}
	void lbu()		{ this->clc_addr(); mem_bytes = 1; mem_sign = 0;}
	void lhu()		{ this->clc_addr(); mem_bytes = 2; mem_sign = 0;}
	void sb()		{ this->clc_addr(); mem_bytes = 1;}
	void sh()		{ this->clc_addr(); mem_bytes = 2;}
	void sw()		{ this->clc_addr(); mem_bytes = 4;}
	void syscall()	{ }
	void breaking() { }
	void trap()		{ }
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H
