#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdio>
using namespace std;



int main(int argc, char* argv[]){
    
    //assert(argc == 4) ;
    
    int address_bits, block_size, cache_sets, associativity ;
    
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
    
    //test purpose
    //cout << address_bits << block_size << cache_sets << associativity ;
    

    
    
    
    
    
    string lst_first_line ;
    
    ifstream read_lst(argv[2]) ;
    assert(read_lst); ;
    
    if (read_lst)
    {
        getline (read_lst, lst_first_line) ;
        
        while (!cin.fail()) {
            <#statements#>
        }
    }
    
    
    
    return 0;
}

























/*
 c++ read file
 
 
 string temp ;
 
 ifstream readCache(argv[1]) ;
 if (readCache.is_open())
 {
     readCache >> temp >> address_bits ;
     readCache >> temp >> block_size ;
     readCache >> temp >> cache_sets ;
     readCache >> temp >> associativity ;
     
     readCache.close() ;
 }
 
 //test purpose
 cout << address_bits << block_size << cache_sets << associativity ;
 
 
 */
