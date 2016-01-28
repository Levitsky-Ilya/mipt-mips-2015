/*
 * main.cpp - single-cycle implementation for MIPS
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

#include <iostream>
#include <func_sim.h>

using namespace std;

int main (int argc, char** argv)
{
	if ( argc != 3)
	{
		cout << "Using: [mips_exe filename] [num of functions to run]"
			<< endl;
		exit( EXIT_FAILURE);
	}
	
	MIPS* mips = new MIPS;
	mips->run( argv[1], strtol( argv[2], NULL, 10));
	return 0;
}
