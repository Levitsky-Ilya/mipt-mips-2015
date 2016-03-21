/**
 * cache_tag_array.cpp - CacheTagArray implementation
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS 
 */

//C++ headers
#include <iostream>
#include <fstream>
#include <cmath>

//C headers
#include <cstdlib>

//MIPT-MIPS modules
#include <cache_tag_array.h>

#define DEBUG 0

CacheTagArray::CacheTagArray( unsigned size_in_bytes,
							  unsigned ways,
							  unsigned short block_size_in_bytes, 
							  unsigned short addr_size_in_bits,
							  bool is_full)
    :size_in_bytes_( size_in_bytes),
     ways_( is_full? 1 : ways),
     block_size_in_bytes_( block_size_in_bytes),
     addr_size_in_bits_( addr_size_in_bits),
     tag_way_size_( size_in_bytes / ways_ / block_size_in_bytes),
     tag_array_ ( new uint64*[ ways]),
     LRU_info_( is_full? NULL : 
						new deque< unsigned short>[ tag_way_size_]),
	 LRU_info_full_( is_full? vector<bool>( tag_way_size_, false):
							  vector<bool>()),
	 offset_size_in_bits_( ( unsigned short)
					ceil( log2( ( double)block_size_in_bytes_))),
	 set_size_in_bits_( ( unsigned short)
					ceil( log2( ( double)tag_way_size_))),
	 num_LRU_tags_full_(0),
	 is_full_( is_full)
{
	#if DEBUG
	cout << "Constructor start" << endl;
	#endif
	if( !is_full)
		for( unsigned set = 0; set < tag_way_size_; ++set)
			for( unsigned short way = 0; way < ways_; way++)
				LRU_info_[ set].push_back( way);
	
	for( unsigned short way = 0; way < ways_; way++)
		tag_array_[ way] = new uint64[ tag_way_size_];
	
	#if DEBUG	
	cout << "Contructed cache s:" << size_in_bytes_
		 << " w:" << ways_ << " b:" << block_size_in_bytes_
		 << " as:" << addr_size_in_bits_ << endl;
	#endif
}
                   
CacheTagArray::~CacheTagArray()
{
	#if DEBUG
	cout << endl << "Deconstructor start";
	#endif
	if( !is_full_)
		delete [] LRU_info_;
	
	for( unsigned way = 0; way < ways_; way++)
        delete tag_array_[ way];
        
    delete [] tag_array_;
    #if DEBUG
	cout << " -> finished" << endl;
	#endif
}
 
/************************READ****************************/    
bool CacheTagArray::read( uint64 addr)
{
	if( is_full_)
		return readFull( addr);
	else
	{
		uint64 tag = getTag( addr);				
		uint64 set = getSet( addr);				
		
		for( unsigned short way = 0; way < ways_; way++)
		{
			if ( tag_array_[ way][ set] == tag)
			{
				makeTagLRU( way, set);
				return true;
			}
		}
		return false;
	}
}

bool CacheTagArray::readFull( uint64 addr)
{
	uint64 tag = getTagFull( addr);
	for ( unsigned long curr_tag = 0;
		  curr_tag < tag_way_size_;
		  curr_tag++)
		if (tag_array_[0][curr_tag] == tag)
		{
			makeTagLRUFull( curr_tag);
			return true;
		}
	return false;
}
/********************************************************/

/************************WRITE***************************/
void CacheTagArray::write( uint64 addr)
{
	if( is_full_)
		writeFull( addr);
	else
	{
		uint64 tag = getTag( addr);
		uint64 set = getSet( addr);
		
		unsigned short way = 0;
		if ( !LRU_info_[ set].empty())
		{
			way = LRU_info_[ set].front();
		}
		
		tag_array_[ way][ set] = tag;

		makeTagLRU( way, set);
	}	
}

void CacheTagArray::writeFull( uint64 addr)
{
	uint64 tag = getTagFull( addr); 
    unsigned tag_place = getTagPlaceFull( tag);
    tag_array_[0][ tag_place] = tag;

    makeTagLRUFull( tag_place);
}
/********************************************************/

/*******************GET_OPERATIONS***********************/
uint64 CacheTagArray::getTagFull( uint64 addr) const
{
    return addr >> offset_size_in_bits_;
}

uint64 CacheTagArray::getTag( uint64 addr) const
{
    addr >>= offset_size_in_bits_;
    addr >>= set_size_in_bits_;
    return addr;
}

uint64 CacheTagArray::getSet( uint64 addr) const
{
    addr >>= offset_size_in_bits_;
    uint64 mask_for_set = (1 << set_size_in_bits_) - 1;
    //cout << hex << "maks hex " << mask_for_set << endl;
    addr = addr & mask_for_set;
    return addr;
}

uint64 CacheTagArray::getOffset( uint64 addr) const
{
    uint64 mask_for_offset = (1 << offset_size_in_bits_)  - 1;
    return (addr & mask_for_offset);
}
/********************************************************/

/*******************LRU_OPERATIONS***********************/
unsigned CacheTagArray::getTagPlaceFull( uint64 tag) const
{ 
    for( unsigned tag_place = 0; tag_place < tag_way_size_; tag_place++)
        if( !LRU_info_full_[ tag_place])
            return tag_place;
            
    cerr << "ERROR. No place for writing found." << endl;
    exit( EXIT_FAILURE);
}

void CacheTagArray::makeTagLRU( unsigned short way, uint64 set)
{
    deque< unsigned short>::iterator it;
    for( it = LRU_info_[ set].begin(); it != LRU_info_[ set].end(); it++)
    {
        if( *it == way)
        {
            LRU_info_[ set].erase( it);
            break;
        }
    }
    LRU_info_[ set].push_back( way);
}

void CacheTagArray::makeTagLRUFull( unsigned tag_place)
{
	//Algorithm is pseudo-LRU. 
    if( !LRU_info_full_[ tag_place])
    {
        num_LRU_tags_full_++;
		LRU_info_full_[ tag_place] = true;
	}
        
    if ( num_LRU_tags_full_ == tag_way_size_)
    {
        LRU_info_full_.assign( tag_way_size_, false);
        num_LRU_tags_full_ = 0;
    }
}
/********************************************************/
