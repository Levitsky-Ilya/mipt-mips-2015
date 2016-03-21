/**
 * cache_tag_array.h - CacheTagArray header
 * @author Ilya Levitsky ilya.levitskiy@phystech.edu
 * Copyright 2016 MIPT-MIPS 
 */
 
 #ifndef CACHE_TAG_ARRAY
 #define CACHE_TAG_ARRAY
 
 
#include <deque>
#include <vector>
 
#include <types.h>

using namespace std;
 
class CacheTagArray
{
public:
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits,
                   bool is_full);
    ~CacheTagArray();
     
    bool read( uint64 addr);
    void write( uint64 addr);
private:
	const unsigned size_in_bytes_;
    const unsigned ways_;
    const unsigned short block_size_in_bytes_;
    const unsigned short addr_size_in_bits_;
    const unsigned tag_way_size_;
    uint64** const tag_array_;
    
    deque< unsigned short>* LRU_info_;
    vector< bool> LRU_info_full_;
    
    const unsigned short offset_size_in_bits_;
    const unsigned short set_size_in_bits_;
       
    unsigned num_LRU_tags_full_; 
    bool is_full_;
    
    bool readFull( uint64 addr);
    void writeFull( uint64 addr);
    
    uint64 getTagFull( uint64 addr) const;
    uint64 getTag( uint64 addr) const;
    uint64 getSet( uint64 addr) const;
    uint64 getOffset( uint64 addr) const;
    
    unsigned getTagPlaceFull( uint64 tag) const;
    void makeTagLRU( unsigned short way, uint64 set);
    void makeTagLRUFull( unsigned tag_place);
};

#endif //CACHE_TAG_ARRAY
