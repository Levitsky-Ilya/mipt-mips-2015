/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Generic C++
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

// uArchSim modules
#include <func_memory.h>

using namespace std;

void FuncMemory::getCoordsByAddr( uint64  addr, uint64& set,
								  uint64& page, uint64& offset) const
{
	clog << "Begin getCoords, addr: " << addr << endl;
	//(temp) offset = ( 1 << offset_bits_priv) - 1 & addr;
	page = ( 1 <<  page_bits_priv) - 1 & addr >> offset_bits_priv;
	set = addr >> page_bits_priv + offset_bits_priv;
	
	offset = offset_bits_priv;
	clog << "offset_bits_priv: "  << offset << endl;
	offset = offset_bits_priv - 1 ;
	clog << "offset_bits_priv - 1: "  << offset << endl;
	offset = ( 1 << offset_bits_priv ) - 1 ;
	clog << "( 1 << offset_bits_priv ) - 1: "  << offset << endl;
	offset = 1 << offset_bits_priv - 1;
	clog << "1 << offset_bits_priv - 1: "  << offset << endl;
	offset = ( 1 << offset_bits_priv - 1) - 1 ;
	clog << "( 1 << offset_bits_priv - 1) - 1: "  << offset << endl;
	offset = ( 1 << offset_bits_priv - 1) - 1 & addr;
	clog << "( 1 << offset_bits_priv - 1) - 1 & addr: "  << offset << endl;
	offset = ( 1 << offset_bits_priv) - 1 & addr;
	
	clog << "End getCoords, offset: " << set <<
		", page : " << page << ", offset : " << offset << endl;
}
									

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
	clog << "Begin FuncMemory, addr size: " << addr_size <<
		", page bits: " << page_bits << ", offset bits: " <<
		offset_bits << endl;
		
	num_of_sets_priv = 1 << ( addr_size - page_bits - offset_bits);
	num_of_pages_priv = 1 << page_bits;
	num_of_bytes_priv = 1 << offset_bits;
	offset_bits_priv = offset_bits;
	page_bits_priv = page_bits;
	set_bits_priv = addr_size - page_bits - offset_bits;
	
	arr_of_sets_priv = new uint8**[num_of_sets_priv];
	
	memset( arr_of_sets_priv, 0,
		num_of_sets_priv*sizeof( *arr_of_sets_priv));
		
	vector< ElfSection> sections_array;
	ElfSection::getAllElfSections( executable_file_name,
		sections_array);
	
	for( unsigned long i = 0; i < sections_array.size(); ++i)
	{
		if ( strcmp( sections_array[i].name, ".text") == 0)
		{
			addr_of_text_section_priv = sections_array[i].start_addr;
		}
		clog <<  i << "th section, start addr: " << 
			sections_array[i].start_addr << endl;
		for (uint64 j = 0; j < sections_array[i].size; ++j)
		{
			uint64 real_set, real_page, real_offset;
			getCoordsByAddr( sections_array[i].start_addr + j,
				real_set, real_page, real_offset);
			
			if( arr_of_sets_priv[ real_set] == NULL)
			{
				arr_of_sets_priv[ real_set] =
					new uint8*[ num_of_pages_priv];
				memset( arr_of_sets_priv[ real_set], 0,
					num_of_pages_priv*
					sizeof( arr_of_sets_priv[ real_set][ 0]));
			}
			
			if( arr_of_sets_priv[ real_set][ real_page] == NULL)
			{
				arr_of_sets_priv[ real_set][ real_page] =
					new uint8[ num_of_bytes_priv];
				memset( arr_of_sets_priv[ real_set][ real_page],
					0, num_of_bytes_priv *
					sizeof( arr_of_sets_priv[ real_set][ real_page][ 0]));
			}
			
			arr_of_sets_priv[ real_set][ real_page][ real_offset] = 
				sections_array[i].content[j];
			clog << "Func Mem: set: " << real_set <<
					", page: " << real_page << 
					", offset: " << real_offset <<
					", addr: " << sections_array[i].start_addr + j <<
					", val: " <<
				( uint16)arr_of_sets_priv[ real_set][ real_page][ real_offset] << endl;
		}
	}
	
	fprintf(stderr, "FuncMemory dumps:\n\n%s",dump().c_str());
	fprintf(stderr, "End FuncMemory\n");
}

FuncMemory::~FuncMemory()
{
	for( unsigned long i = 0; i < num_of_sets_priv; ++i)
	{
		if ( arr_of_sets_priv[ i] != NULL)
		{
			for( unsigned long j = 0; j < num_of_pages_priv; ++j)
			{
				if ( arr_of_sets_priv[ i][ j] != NULL)
				{
					delete [] arr_of_sets_priv[ i][ j];
				}
			}
			delete [] arr_of_sets_priv[ i];
		}
	}
	delete [] arr_of_sets_priv;
	num_of_sets_priv = NO_VAL64;
	num_of_pages_priv = NO_VAL64;
	num_of_bytes_priv = NO_VAL64;
	num_of_bytes_priv = NO_VAL64;
    offset_bits_priv = page_bits_priv = set_bits_priv = NO_VAL64;
}

uint64 FuncMemory::startPC() const
{
    return addr_of_text_section_priv;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
	clog << "Begin read, addr: " << addr <<
			", num of bytes: " << num_of_bytes << endl;
	if( num_of_bytes == 0)
    {
		cerr << "Tried to read 0 bytes" << endl;
		exit( EXIT_FAILURE);
	}
	if( num_of_bytes > 8) 
	{
		cerr << "Tried to read more than 8 bytes" << endl;
		exit( EXIT_FAILURE);
	}
	
	uint64 curr_set, curr_page, curr_offset;	
	
	getCoordsByAddr( addr , curr_set,
							curr_page,
							curr_offset);
	const int sizeof_byte = sizeof( uint8);
	uint64 ret = 0;

	for ( unsigned short i = 0; i < num_of_bytes; ++i)
	{
		if( curr_offset + i >= num_of_bytes_priv) 
		{
			curr_page++;
			curr_offset -= num_of_bytes_priv;
		}	
		
		if( curr_page >= num_of_pages_priv)
		{
			curr_set++;
			curr_page -= num_of_pages_priv;
		}	
		if( curr_set >= num_of_sets_priv)
		{
			cerr << "End of memory reached" << endl;
			exit( EXIT_FAILURE);
		}
		if( arr_of_sets_priv[ curr_set] == NULL)
		{
			cerr << "Read from uninit-ed set" << endl;
			exit( EXIT_FAILURE);
		}
		if( arr_of_sets_priv[ curr_set][ curr_page] == NULL)
		{
			cerr << "Read from uninit-ed page" << endl;
			exit( EXIT_FAILURE);
		}

		ret = ( ret << sizeof_byte) + 
			( uint8)arr_of_sets_priv[ curr_set][ curr_page][ curr_offset + i];
			
			clog << "read  set: " << curr_set <<
					", page: " << curr_page << 
					", offset: " << curr_offset + i << ", val: " << ret << ", return: " << 
				( uint16)arr_of_sets_priv[ curr_set][ curr_page][ curr_offset + i] << endl;
	}

    return ret;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
	if( num_of_bytes == 0)
    {
		cerr << "Tried to write 0 bytes" << endl;
		exit( EXIT_FAILURE);
	}
	if( num_of_bytes > 8) 
	{
		cerr << "Tried to write more than 8 bytes" << endl;
		exit( EXIT_FAILURE);
	}
	
	uint64 curr_set, curr_page, curr_offset;
	
	getCoordsByAddr( addr , curr_set,
							curr_page,
							curr_offset);				
							
	const int sizeof_byte = sizeof( uint8);
    
    for( unsigned short i = 0; i < num_of_bytes; ++i)
    {
		if( curr_offset + i >= num_of_bytes_priv) 
		{
			curr_page++;
			curr_offset -= num_of_bytes_priv;
		}
		if( curr_page >= num_of_pages_priv)
		{
			curr_set++;
			curr_page -= num_of_pages_priv;
		}
		if( curr_set >= num_of_sets_priv)
		{
			cerr << "End of memory reached" << endl;
			exit( EXIT_FAILURE);
		}
		
		if( arr_of_sets_priv[ curr_set] == NULL)
			{
				arr_of_sets_priv[ curr_set] =
					new uint8*[ num_of_pages_priv];
				memset( arr_of_sets_priv[ curr_set], 0,
					num_of_pages_priv*
					sizeof( arr_of_sets_priv[ curr_set][ 0]));
				clog << "Initialized new set no. " << curr_set << endl;
			}
			
			if( arr_of_sets_priv[ curr_set][ curr_page] == NULL)
			{
				arr_of_sets_priv[ curr_set][ curr_page] =
					new uint8[ num_of_bytes_priv];
				memset( arr_of_sets_priv[ curr_set][ curr_page],
					0, num_of_bytes*
					sizeof( arr_of_sets_priv[ curr_set][ curr_page][ 0]));
				clog << "Initialized new page no. " << curr_set << 
					",set no. " << curr_set << endl;
			}
		arr_of_sets_priv[ curr_set][ curr_page][ curr_offset + i] =
			( uint8)( value >> ( num_of_bytes - i) *
			sizeof_byte & 1 << sizeof_byte - 1);
	}
}

string FuncMemory::dump( string indent) const
{
	clog << "Began dump" << endl;
    ostringstream oss;

    oss << indent << "Dump of FuncMemory" << endl
        << indent << "  Content:" << endl;

    // split the contents into words of 4 bytes
    bool skip_was_printed = false;
    
    for( unsigned long i = 0; i < num_of_sets_priv; ++i)
	{
		if ( arr_of_sets_priv[ i] != NULL)
		{
			for( unsigned long j = 0; j < num_of_pages_priv; ++j)
			{
				if ( arr_of_sets_priv[ i][ j] != NULL)
				{
					for(unsigned long k = 0; k < num_of_bytes_priv; k+=4)
					{
						uint64 curr_addr = (i << page_bits_priv + offset_bits_priv) +
							(j << offset_bits_priv) + k;
						uint64 value = read(curr_addr, 4);
						if ( value  == 0)
						{   
							if ( !skip_was_printed)
							{
								oss << indent << "  ....  " << endl;
								skip_was_printed = true;
							}
						}
						else
						{ 
							oss << indent << "    0x" << hex << curr_addr 
								<< indent << ":    " << value << endl;
							skip_was_printed = false;
						}
					}
				}
			}
		}
	}

    return oss.str();
}
