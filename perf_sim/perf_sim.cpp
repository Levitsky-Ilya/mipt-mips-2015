const int PORT_BW = 1;
const int PORT_LATENCY = 1;
const int PORT_FANOUT = 1;

#include <iostream>
#include <queue>
#include <perf_sim.h>

using namespace std;

/*************************PerfMIPS***************************/
PerfMIPS::PerfMIPS()
{
    rf = new RF();
}

void PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool silent)
{
    mem = new FuncMemory(tr.c_str());
    PC.value = mem->startPC();
    uint32 instrNo = 0;
    uint32 cycle = 0;
    
    fetch = new Fetch( mem);
    decode = new Decode( rf);
    execute = new Execute();
    memory = new Memory( mem);
    writeback = new Writeback( &PC, rf);
    
    fetch->init_ports();
    decode->init_ports();
    execute->init_ports();
    memory->init_ports();
    writeback->init_ports();
    
    while (instrNo < instrs_to_run) {
        fetch->clock( PC, cycle, silent);
        decode->clock( PC.value-4, cycle, silent);
        execute->clock( cycle, silent);
        memory->clock( cycle, silent);
        if(writeback->clock( cycle, silent))
        {
			instrNo++;
		}
		cycle++;
		
		if(!silent) {cout << "_____________________________" << endl;}
    }
    if(!silent) {cout << "RUN COMPLETED_____________________" << endl;}
    delete mem;
    delete fetch;
    delete decode;
    delete execute;
    delete memory;
    delete writeback;
    
}

PerfMIPS::~PerfMIPS() {
    delete rf;
}
/*************************************************************/

/*************************FETCH*******************************/
Fetch::Fetch( const FuncMemory* mem)
{
	mem_ = mem;
	rp_decode_2_fetch_stall = new ReadPort< bool> ( "fetch<-decode", PORT_LATENCY); 
	wp_fetch_2_decode = new WritePort< uint32> ( "fetch->decode", PORT_BW, PORT_FANOUT);
}

void Fetch::clock( Reg& PC, uint32 cycle, bool silent)
{
	if(!silent) {cout << "Fetching. Cycle: " << cycle << ",   ";}
	bool isStall;
	rp_decode_2_fetch_stall->read( &isStall, cycle);
	if( isStall)
	{ 
		if(!silent) {cout << "stall" << endl;}
		return;
	}
	if( !PC.isValid)
	{ 
		if(!silent) {cout << "invalid PC" << endl;}
		return;
	}
	instr_code = mem_->read(PC.value);
	wp_fetch_2_decode->write( instr_code, cycle);
	
	FuncInstr instr;
	instr = FuncInstr(instr_code, PC.value);
	if(!instr.is_jump())
	{
		PC.value += 4;
	}
	else
	{
		PC.isValid = false;
	}
	
	if(!silent) {cout << hex << showbase << instr_code << dec << endl;}
}

void Fetch::init_ports()
{
	rp_decode_2_fetch_stall->init();
	wp_fetch_2_decode->init();
}

Fetch::~Fetch()
{
	delete rp_decode_2_fetch_stall;
	delete wp_fetch_2_decode;
}
/**************************************************************/

/*************************DECODE*******************************/
Decode::Decode( RF *rf)
{
	rf_ = rf;
	rp_execute_2_decode_stall = new ReadPort< bool>
		( "decode<-execute", PORT_LATENCY);
	rp_fetch_2_decode = new ReadPort< uint32>
		( "fetch->decode", PORT_LATENCY);
	wp_decode_2_fetch_stall = new WritePort< bool>
		( "fetch<-decode", PORT_BW, PORT_FANOUT);   
	wp_decode_2_execute = new WritePort< FuncInstr>
		( "decode->execute", PORT_BW, PORT_FANOUT);
}

void Decode::clock( uint32 PC, uint32 cycle, bool silent)
{
	if(!silent) {cout << "Decoding. Cycle:" << cycle << ",   ";}
	bool isStall;
	rp_execute_2_decode_stall->read( &isStall, cycle);
	if( isStall)
	{
		if(!silent) {cout << "stall" << endl;}
		wp_decode_2_fetch_stall->write( true, cycle);
		return;
	}
	if(rp_fetch_2_decode->read( &instr_code, cycle))
	{
		queueDecode.push(instr_code);
	}
	if(queueDecode.empty())
	{
		if(!silent) {cout << "not readen" << endl;}
		return;
	}
	instr_code = queueDecode.front();
	instr = FuncInstr(instr_code, PC);
	
	if(rf_->check(instr.get_src1_num()) && rf_->check(instr.get_src2_num()))
	{	
		read_src(instr);
		rf_->invalidate(instr.get_dst_num());
	
		wp_decode_2_execute->write( instr, cycle);
		wp_decode_2_fetch_stall->write( false, cycle);
		queueDecode.pop();
		if(!silent) {cout << instr << endl;}
	}
	else
	{
		if(!silent) {cout << "data dependancy" << endl;}
		wp_decode_2_fetch_stall->write( true, cycle);
	}
	
}

void Decode::read_src(FuncInstr& instr) const {
    rf_->read_src1(instr); 
    rf_->read_src2(instr); 
}

void Decode::init_ports()
{
	rp_execute_2_decode_stall->init();
	rp_fetch_2_decode->init();
	wp_decode_2_fetch_stall->init();
	wp_decode_2_execute->init();
}

Decode::~Decode()
{
	delete rp_execute_2_decode_stall;
	delete rp_fetch_2_decode;
	delete wp_decode_2_fetch_stall;
	delete wp_decode_2_execute;
}
/**************************************************************/

/*************************EXECUTE******************************/
Execute::Execute()
{
	rp_memory_2_execute_stall = new ReadPort< bool>
		( "execute<-memory", PORT_LATENCY);    
	rp_decode_2_execute = new ReadPort< FuncInstr>
		( "decode->execute", PORT_LATENCY);
	wp_execute_2_decode_stall = new WritePort< bool>
		( "decode<-execute", PORT_BW, PORT_FANOUT);  
	wp_execute_2_memory = new WritePort< FuncInstr>
		( "execute->memory", PORT_BW, PORT_FANOUT);
}

void Execute::clock( uint32 cycle, bool silent)
{
	if(!silent) {cout << "Executing. Cycle:" << cycle << ",   ";}
	bool isStall;
	rp_memory_2_execute_stall->read( &isStall, cycle);
	if( isStall)
	{
		if(!silent) {cout << "stall" << endl;}
		wp_execute_2_decode_stall->write( true, cycle);
		return;
	}
	if(!rp_decode_2_execute->read( &instr, cycle))
	{
		if(!silent) {cout << "not readen" << endl;}
		return;
	}
	
	instr.execute();
	
	wp_execute_2_memory->write( instr, cycle);
	if(!silent) {cout << instr << endl;}
}

void Execute::init_ports()
{
	rp_memory_2_execute_stall->init();
	rp_decode_2_execute->init();
	wp_execute_2_decode_stall->init();
	wp_execute_2_memory->init();
}

Execute::~Execute()
{
	delete rp_memory_2_execute_stall;
	delete rp_decode_2_execute;
	delete wp_execute_2_decode_stall;
	delete wp_execute_2_memory;
}
/**************************************************************/

/*************************MEMORY*******************************/
Memory::Memory( FuncMemory* mem)
{
	mem_ = mem;
	rp_writeback_2_memory_stall = new ReadPort< bool>
		( "memory<-writeback", PORT_LATENCY);   
	rp_execute_2_memory = new ReadPort< FuncInstr>
		( "execute->memory", PORT_LATENCY);
	wp_memory_2_execute_stall = new WritePort< bool>
		( "execute<-memory", PORT_BW, PORT_FANOUT);    
	wp_memory_2_writeback = new WritePort< FuncInstr>
		( "memory->writeback", PORT_BW, PORT_FANOUT);
}

void Memory::clock( uint32 cycle, bool silent)
{
	if(!silent) {cout << "Memory. Cycle:" << cycle << ",   ";}
	bool isStall;
	rp_writeback_2_memory_stall->read( &isStall, cycle);
	if( isStall)
	{
		if(!silent) {cout << "stall" << endl;}
		wp_memory_2_execute_stall->write( true, cycle);
		return;
	}
	if(!rp_execute_2_memory->read( &instr, cycle))
	{
		if(!silent) {cout << "not readen" << endl;}
		return;
	}
	
	load_store(instr);
	
	wp_memory_2_writeback->write( instr, cycle);
	if(!silent) {cout << instr << endl;}
}

void Memory::load(FuncInstr& instr) const {
    instr.set_v_dst(mem_->read(instr.get_mem_addr(), instr.get_mem_size()));
}

void Memory::store(const FuncInstr& instr) {
    mem_->write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
}

void Memory::load_store(FuncInstr& instr) {
    if (instr.is_load())
        load(instr);
    else if (instr.is_store())
        store(instr);
}

void Memory::init_ports()
{
	rp_writeback_2_memory_stall->init();
	rp_execute_2_memory->init();
	wp_memory_2_execute_stall->init();
	wp_memory_2_writeback->init();
}

Memory::~Memory()
{
	delete rp_writeback_2_memory_stall;
	delete rp_execute_2_memory;
	delete wp_memory_2_execute_stall;
	delete wp_memory_2_writeback;
}
/**************************************************************/

/*************************WRITEBACK****************************/
Writeback::Writeback( Reg* PC, RF* rf)
{   
	PC_ = PC;
	rf_ = rf;
	rp_memory_2_writeback = new ReadPort< FuncInstr>
		( "memory->writeback", PORT_LATENCY);
	wp_writeback_2_memory_stall = new WritePort< bool>
		( "memory<-writeback", PORT_BW, PORT_FANOUT);
}

bool Writeback::clock( uint32 cycle, bool silent)
{
	if(!silent) {cout << "Writeback. Cycle:" << cycle << ",   ";}
	if(!rp_memory_2_writeback->read( &instr, cycle))
	{
		if(!silent) {cout << "not readen" << endl;}
		return false;
	}
	
	if(instr.is_jump())
	{
		assert( PC_->isValid == false);
		PC_->value = instr.get_new_PC();
		PC_->isValid = true;
	}
	
	wb( instr);
	
	std::cout << instr << std::endl;
	return true;
}

void Writeback::wb(const FuncInstr& instr) {
	rf_->write_dst(instr);
}

void Writeback::init_ports()
{
	rp_memory_2_writeback->init();
	wp_writeback_2_memory_stall->init();
}

Writeback::~Writeback()
{
	delete rp_memory_2_writeback;
	delete wp_writeback_2_memory_stall;
}
/**************************************************************/
