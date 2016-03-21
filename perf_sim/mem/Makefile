#
# Building the cache tag array simulator
# @author Ilya Levitsky <ilya.levitskiy@phystech.com>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/

# specifying flags for C++ compiler
CXXFLAGS= -g -std=c++11 -O3 -Ofast -Wall -Werror

#
# Enter for building miss_rate_sim.exe
#
miss_rate_sim.exe: cache_tag_array.o miss_rate_sim.o
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCL)

miss_rate_sim.o: miss_rate_sim.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCL)

clean:
	@-rm *.o
	@-rm miss_rate_sim.exe
