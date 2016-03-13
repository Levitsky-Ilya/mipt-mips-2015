/*
 * main.cpp - mips functional simulator
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <iostream>
#include <cstdlib>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and amount of instrs to run" << endl;
        std::exit(EXIT_FAILURE);
    }

    PerfMIPS* perfMips = new PerfMIPS();
    perfMips->run(std::string(argv[1]), atoi(argv[2]), true);
    delete perfMips;

    return 0;
}
