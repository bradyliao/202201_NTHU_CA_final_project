#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <set>
using namespace std;


struct address_struct
{
    string original ;
    string reversed ;
    string set_index_reversed ;
    int set_index_reversed_dec ;
    string tag_index_reversed ;
    bool hit ;
} ;


struct cell
{
    string tag ;
    bool nru_bit ;
} ;






class cache_class
{
public:
    vector<cell> way ;
    vector<vector<cell>> set ;
    cache_class(int associativity, int cache_sets) ;
} ;

cache_class::cache_class(int associativity, int cache_sets)
{
    cell temp_cell ;
    vector<cell> way(associativity, temp_cell) ;
    vector<vector<cell>> set(cache_sets, way) ;
}



int main(int argc, char* argv[]){
    
    assert(argc == 4) ;
    
    int address_bits, block_size, cache_sets, associativity ;
    int block_index_length, set_index_length, tag_index_length ;
    int set_index_start /* , set_index_end */ ;
    int tag_index_start /* , tag_index_end */ ;
    
    FILE* read_org = fopen(argv[1], "r");
    assert(read_org) ;
    
    if (read_org)
    {
        fscanf(read_org, "Address_bits: %d\n"  , &address_bits) ;
        fscanf(read_org, "Block_size: %d\n"    , &block_size) ;
        fscanf(read_org, "Cache_sets: %d\n"    , &cache_sets) ;
        fscanf(read_org, "Associativity: %d\n" , &associativity) ;
        
        fclose(read_org) ;
        
        
    }
    

    
    block_index_length = log2(block_size) ;
    set_index_length = log2(cache_sets) ;
    tag_index_length = address_bits - block_index_length - set_index_length ;
    set_index_start = block_index_length ;
    tag_index_start = block_index_length + set_index_length ;
    
    
    // for use of various indexing bit
    /*
    set<int> indexing_bits ;
    
    //default indexing bits
    for(int i = 0 ; i < address_bits ; i++)
    {
        if ( (i > block_index_length - 1) && (i < block_index_length + set_index_length) )
            indexing_bits.insert(i) ;
    }
    
    set<int> tag_bits ;
    
    //default tag bits
    for(int i = 0 ; i < address_bits ; i++)
    {
        if (i > block_index_length + set_index_length - 1)
            tag_bits.insert(i) ;
    }
    */
    


    
    
    
    
    string lst_first_line ;
    string lst_last_line ;
    
    ifstream read_lst(argv[2]) ;
    assert(read_lst) ;
    
    vector<address_struct> address_list ;
    
    if (read_lst)
    {
        getline (read_lst, lst_first_line) ;
        
        //cout << lst_first_line << endl ;
        
        string input_address ;
        
        while (!read_lst.eof())
        {
            read_lst >> input_address ;
            
            if (input_address[0] == '.')
            {
                lst_last_line = input_address ;
                break ;
            }
            
            address_struct current_address ;
            current_address.original = input_address ;
            current_address.reversed = input_address ;
            reverse(current_address.reversed.begin(), current_address.reversed.end()) ;
            
            
            
            current_address.set_index_reversed = current_address.reversed.substr(set_index_start, set_index_length) ;

            current_address.set_index_reversed_dec = stoi(current_address.set_index_reversed, nullptr, 2) ;
            
            current_address.tag_index_reversed = current_address.reversed.substr(tag_index_start, tag_index_length) ;
            //cout << current_address.set_index_reversed << " " << current_address.tag_index_reversed << " " << current_address.set_index_reversed_dec << set_index_start<<  endl ;
            
            
            address_list.push_back(current_address) ;
        }
        
        read_lst.close() ;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}















