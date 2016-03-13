/*
 * rf.h - mips register file
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>



class RF
{
        struct Reg
        {
			uint32 value;
			bool isValid;
			Reg() : value(0), isValid(true)
			{}
		} array[REG_NUM_MAX];
		
    public:
		inline void invalidate( RegNum reg_num)
		{
			if(reg_num != REG_NUM_ZERO)
				array[(uint32)reg_num].isValid = false;
		}
		
		inline bool check( RegNum reg_num) const
		{
			return array[(uint32)reg_num].isValid;
		}
		
		inline uint32 read( RegNum reg_num) const
		{
			assert(array[(uint32)reg_num].isValid);
			return array[(uint32)reg_num].value;
		}
		
		inline void write( RegNum reg_num, uint32 val)
		{
			assert(!array[(uint32)reg_num].isValid);
			array[reg_num].value = val;
			array[reg_num].isValid = true;
		}
		
        inline void read_src1( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src1_num();
           instr.set_v_src1( read((RegNum)reg_num));
        }

        inline void read_src2( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src2_num();
           instr.set_v_src2( read((RegNum)reg_num));
        }

        inline void write_dst( const FuncInstr& instr)
        {
            size_t reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
                write( (RegNum)reg_num, instr.get_v_dst());
        }
 
        RF()
        {}
};
          
#endif
 
