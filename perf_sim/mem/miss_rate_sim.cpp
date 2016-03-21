/**
 * Miss rate simulator
 * @author Ilya Levitsky <ilya.levitskiy@phystech.edu>	
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//C++ headers
#include <iostream>
#include <fstream>
#include <chrono>

//C headers
#include <cstdlib>

//MIPT-MIPS modules
#include <cache_tag_array.h>

unsigned const KB = 1024;
unsigned const BLOCK_SIZE_IN_BYTES = 4;
unsigned const ADDR_SIZE_IN_BITS = 32;

int main( int argc, char* argv[])
{
	if ( argc != 3)
    {
        cout << "Using: [memory trace file] [output file (*.csv)]" << endl;
        exit ( EXIT_FAILURE);
    }
	
	ofstream out_file;
	out_file.open( argv[2]);
	out_file << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1MB"
			 << endl;
	
	ofstream time_data;
	time_data.open( "time_data.csv");
	time_data << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1MB"
			 << endl;
			 
	for ( unsigned short ways = 0; ways <= 256; 
						( ways == 0)? ways = 1 : ways *= 2)
	// 0 ways means that cache is fully associative.
	{
		cout << "Sim at way " << ways << endl;
		if( ways == 0)
        {
            out_file << "fully,";
            time_data << "fully,";
        }
        else
        {
            out_file << ways << "way(s),";
            time_data << ways << "way(s),";
        }
        
        for ( unsigned size_in_bytes = KB; size_in_bytes <= 1024*KB;
                size_in_bytes *= 2)
        {
			cout << "  Sim at size " << size_in_bytes << endl;
			CacheTagArray cacheTagArr( size_in_bytes,
									   ways, 
									   BLOCK_SIZE_IN_BYTES, 
									   ADDR_SIZE_IN_BITS, 
									   ways == 0);
			
			ifstream mem_trace;
			mem_trace.open( argv[1]);
			
			unsigned long misses = 0;
			unsigned long total = 0;
			
			chrono::time_point< chrono::system_clock> start, end;
			start = chrono::system_clock::now();
			
			uint32 addr;
			bool is_in_cache;
			while ( !mem_trace.eof())
            {
                mem_trace >> hex >> addr;
                is_in_cache = cacheTagArr.read( addr);
                if ( !is_in_cache)
                {
                    cacheTagArr.write( addr);
                    misses++;
				}
                total++;
            }
			
			end = chrono::system_clock::now();
			
			mem_trace.close();

            chrono::duration< double> spent_time = end - start;			
			double miss_rate = ( ( double)misses / total);
            out_file << miss_rate;
            time_data << spent_time.count();
            if ( size_in_bytes != 1024*KB)
            {
                out_file << ",";
                time_data << ",";
            }
		}
		out_file << endl;
        time_data << endl;
	}
	out_file.close();
	time_data.close();
	
	return 0;
}
