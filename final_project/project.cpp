#include <cassert>
#include <iostream>
#include <fstream>
//#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
//#include <string>
#include <set>
#include <stdlib.h>
#include <cstdlib>


using namespace std;


#define OPTIMIZE 1


int string_to_dec(string &text, int &length)
{
    int dec = 0 ;
    for (int i = 0 ; i < length ; i++)
        dec += pow(2.0, (double)i) * (text[i]-48) ;
    
    return dec ;
}




class address_class
{
public:
    string original ;
    string reversed ;
    string set_index_reversed ;
    int set_index_reversed_dec ;
    string tag_index_reversed ;
    bool hit ;
    address_class(string &input_address) ;
    void set_tag_selection_not_optimized(int &set_index_start, int &set_index_length, int &tag_index_start, int &tag_index_length) ;
    void set_tag_selection_optimized(int &set_index_length, set<int> &indexing_bits, int &tag_index_length, set<int>&tag_bits) ;

} ;


address_class::address_class(string &input_address)
{
    hit = 0 ;
    original = input_address ;
    reversed = input_address ;
    reverse(reversed.begin(), reversed.end()) ;
}



void address_class::set_tag_selection_not_optimized(int &set_index_start, int &set_index_length, int &tag_index_start, int &tag_index_length)
{
    set_index_reversed = reversed.substr(set_index_start, set_index_length) ;
    set_index_reversed_dec = string_to_dec(set_index_reversed, set_index_length) ;
    tag_index_reversed = reversed.substr(tag_index_start, tag_index_length) ;
}



void address_class::set_tag_selection_optimized(int &set_index_length, set<int> &indexing_bits, int &tag_index_length, set<int>&tag_bits)
{
    for (set<int>::iterator it = indexing_bits.begin() ; it != indexing_bits.end() ; ++it)
        set_index_reversed.push_back(reversed[*it]) ;

    set_index_reversed_dec = string_to_dec(set_index_reversed, set_index_length) ;
    
    for (set<int>::iterator it = tag_bits.begin() ; it != tag_bits.end() ; ++it)
        tag_index_reversed.push_back(reversed[*it]) ;
    
}





struct cell_struct
{
    string tag ;
    bool nru_bit ;
} ;




class cache_class
{
public:
    vector<vector<cell_struct*>*>* set ;
    int associativity ;
    cache_class(int associativity, int cache_sets) ;
    void access(address_class &address) ;
    int first1, i ;
} ;


cache_class::cache_class(int associativity, int cache_sets)
{
    this->associativity = associativity ;
    
    vector<vector<cell_struct*>* >* setptr = new vector<vector<cell_struct*>* > ; ;
    this->set = setptr ;
    
    for (int i = 0; i < cache_sets; i++)
    {
        vector<cell_struct*>* way = new vector<cell_struct*> ;
                
        for (int j = 0; j < associativity ; j++)
        {
            way->push_back(new cell_struct) ;
            way->at(j)->nru_bit = 1 ;
        }
        this->set->push_back(way) ;
    }
}


void cache_class::access(address_class &address)
{
    first1 = -1 ;
    
    for (i=0 ; i < associativity ; i++)
    {
        if ( set->at(address.set_index_reversed_dec)->at(i)->tag == address.tag_index_reversed )
        {
            address.hit = 1 ;
            set->at(address.set_index_reversed_dec)->at(i)->nru_bit = 0 ;
            break ;
        }
        
        //if (first1 == -1)
        //    if (set->at(address.set_index_reversed_dec)->at(i)->nru_bit == 1)
        //        first1 = i ;
    }

    if (!address.hit)
    {
        for (i=0 ; i < associativity ; i++)
            if (set->at(address.set_index_reversed_dec)->at(i)->nru_bit == 1)
            {
                first1 = i ;
                break ;
            }

        
        if (first1 == -1)
        {
            for (i=0 ; i < associativity ; i++)
                set->at(address.set_index_reversed_dec)->at(i)->nru_bit = 1 ;
            
            set->at(address.set_index_reversed_dec)->at(0)->tag = address.tag_index_reversed ;
            set->at(address.set_index_reversed_dec)->at(0)->nru_bit = 0 ;
        }
        else
        {
            set->at(address.set_index_reversed_dec)->at(first1)->tag = address.tag_index_reversed ;
            set->at(address.set_index_reversed_dec)->at(first1)->nru_bit = 0 ;
        }
    }
    
}












int main(int argc, char* argv[]){
    
    assert(argc == 4) ;
    
    
    
    
    //read org
    int address_bits, block_size, cache_sets, associativity ;
    int block_index_length, set_index_length, tag_index_length ;
    int set_index_start /* , set_index_end */ ;
    int tag_index_start /* , tag_index_end */ ;
    
    FILE* read_org = fopen(argv[1], "r");
    assert(read_org) ;
    
    //if (read_org)
    {
        fscanf(read_org, "Address_bits: %d\n"  , &address_bits) ;
        fscanf(read_org, "Block_size: %d\n"    , &block_size) ;
        fscanf(read_org, "Cache_sets: %d\n"    , &cache_sets) ;
        fscanf(read_org, "Associativity: %d\n" , &associativity) ;
        
        fclose(read_org) ;
    }
    

    
    
    //read lst
    string lst_first_line ;
    string lst_last_line ;
    
    ifstream read_lst(argv[2]) ;
    assert(read_lst) ;
    
    vector<address_class> address_list ;
    vector<string> address_list_optimize_process ;
    
    
    if (read_lst)
    {
        getline (read_lst, lst_first_line) ;
                
        string input_address ;
        
        while (!read_lst.eof())
        {
            read_lst >> input_address ;
            
            if (input_address[0] == '.')
            {
                lst_last_line = input_address ;
                break ;
            }
            
            address_class current_address(input_address) ;
            
            address_list.push_back(current_address) ;
            address_list_optimize_process.push_back(current_address.reversed) ;
        }
        
        read_lst.close() ;
    }
    
    
    
    
    
    
    block_index_length = log2(block_size) ;
    set_index_length = log2(cache_sets) ;
    tag_index_length = address_bits - block_index_length - set_index_length ;
    
    
    
    
    // for use of various indexing bit
    set<int> indexing_bits ;
    
    
    
    if (!OPTIMIZE)
    {
        //default indexing bits
        for(int i = 0 ; i < address_bits ; i++)
        {
            if ( (i > block_index_length - 1) && (i < block_index_length + set_index_length) )
                indexing_bits.insert(i) ;
        }
        
        set_index_start = block_index_length ;
        tag_index_start = block_index_length + set_index_length ;
        
        for (int i = 0; i < address_list.size(); i++)
        {
            address_list[i].set_tag_selection_not_optimized(set_index_start, set_index_length, tag_index_start, tag_index_length) ;
        }
    }
    else
        
        
        
    {
        int left_right_difference_combination_count[address_bits][address_bits][4] ;
        double left_right_difference_combination_value[address_bits][address_bits] ;
        int up_down_change_count[address_bits] ;
        double optimize_value[address_bits] ;
        
        
        for (int i = 0 ; i < address_list_optimize_process.size() ; i++)
        {
            for (int j = block_index_length ; j < address_bits ; j++)
            {
                
                for (int k = j+1 ; k < address_bits ; k++)
                {
                    if (address_list_optimize_process[i][j] == '0' && address_list_optimize_process[i][k] == '0')
                        left_right_difference_combination_count[j][k][0]++ ;
                    if (address_list_optimize_process[i][j] == '0' && address_list_optimize_process[i][k] == '1')
                        left_right_difference_combination_count[j][k][1]++ ;
                    if (address_list_optimize_process[i][j] == '1' && address_list_optimize_process[i][k] == '0')
                        left_right_difference_combination_count[j][k][2]++ ;
                    if (address_list_optimize_process[i][j] == '1' && address_list_optimize_process[i][k] == '1')
                        left_right_difference_combination_count[j][k][3]++ ;
                }
            }
        }
        
        
        
        
        for (int i = 0 ; i < address_list_optimize_process.size()-1 ; i++)
        {
            for (int j = 0; j < block_index_length; j++)
                up_down_change_count[j] = 0 ;
            
            for (int j = block_index_length ; j < address_bits; j++) {
                if (address_list_optimize_process[i][j] != address_list_optimize_process[i+1][j])
                {
                    up_down_change_count[j]++ ;
                }
            }
        }
        
        /*
        for (int i = 0 ; i < address_list_optimize_process.size()-associativity*cache_sets ; i++)
        {
            for (int j = 0; j < block_index_length; j++)
                up_down_change_count[j] = 0 ;
            
            for (int j = block_index_length ; j < address_bits; j++)
            {
                for (int k = 0 ; k < associativity*cache_sets; k++) {
                    if (address_list_optimize_process[i][j] != address_list_optimize_process[i+k][j])
                    {
                        up_down_change_count[j]++ ;
                    }
                }
                
            }
        }
        
        */
        
        
        
        
        
        
        for (int j = 0; j < block_index_length; j++)
            optimize_value[j] = 0 ;
        
        
        for (int j = block_index_length ; j < address_bits ; j++)
        {
            optimize_value[j] = 0 ;
            
            for (int k = j+1 ; k < address_bits ; k++)
            {
                left_right_difference_combination_value[j][k] = 1 ;
                for (int t = 0 ; t < 4; t++)
                {
                    
                    left_right_difference_combination_value[j][k] *= (float)left_right_difference_combination_count[j][k][t] /* / (float)address_list_optimize_process.size() */ ;
                }
                
                double temp = left_right_difference_combination_value[j][k] /* * up_down_change_count[j]  */ ;
                if (temp > optimize_value[j])
                {
                    optimize_value[j] = temp ;
                }
            }
        }
        
        
        
        
        
        
        
        for (int i = 0 ; i < set_index_length ; i++)
        {
            double largest = 0 ;
            int index ;
            for (int j = block_index_length ; j < address_bits ; j++)
            {
                if (optimize_value[j] > largest)
                {
                    largest = optimize_value[j] ;
                    index = j ;
                }
            }
            indexing_bits.insert(index) ;
            optimize_value[index] = 0 ;
        }
        
        
        //indexing_bits.insert(6) ;
        //indexing_bits.insert(7) ;
        //indexing_bits.insert(8) ;
        //indexing_bits.insert(9) ;
        //indexing_bits.insert(11) ;
        
        //for (set<int>::iterator it = indexing_bits.begin() ; it != indexing_bits.end() ; it++)
        //    cout << *it ;
        

        

        set<int> tag_bits ;
        for (int i = block_index_length ; i < address_bits ; i++)
        {
            if (!indexing_bits.count(i))
            {
                tag_bits.insert(i) ;
            }
        }
        

        
        
        
        for (int i = 0; i < address_list.size(); i++)
        {
            address_list[i].set_tag_selection_optimized(set_index_length, indexing_bits, tag_index_length, tag_bits) ;
        }
                
        
        
            
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //create cache
    cache_class cache(associativity, cache_sets) ;
    

    
    
    //access cache process
    for (int i = 0 ; i < address_list.size() ; i++)
    {
        cache.access(address_list[i]) ;
        //cout << address_list[i].set_index_reversed << " " << address_list[i].tag_index_reversed << " " << address_list[i].set_index_reversed_dec  << " " << address_list[i].hit << endl ;
    }
    
    
    
    
    //write rpt
    ofstream write_rpt ;
    assert(write_rpt) ;
    write_rpt.open(argv[3], ios::trunc) ;
    
    if(write_rpt.is_open())
    {
        write_rpt << "Address bits: "  << address_bits << endl ;
        write_rpt << "Block size: "    << block_size << endl ;
        write_rpt << "Cache sets: "    << cache_sets << endl ;
        write_rpt << "Associativity: " << associativity << endl << endl ;
        
        write_rpt << "Offset bit count: "    << block_index_length << endl ;
        write_rpt << "Indexing bit count: "  << set_index_length << endl ;
        write_rpt << "Indexing bits: "       ;
        //set<int>::reverse_iterator rit;
        //for (rit = indexing_bits.rbegin() ; rit != indexing_bits.rend() ; rit++)
        //  write_rpt << *rit << " " ;
        set<int>::iterator rit;
        for (rit = indexing_bits.begin() ; rit != indexing_bits.end() ; rit++)
          write_rpt << *rit << " " ;
        
        write_rpt << endl << endl ;
        
        write_rpt << lst_first_line << endl ;
        string hit_or_miss ;
        int miss_count = 0 ;
        for (int i = 0 ; i < address_list.size() ; i++)
        {
            //hit_or_miss = (address_list[i].hit == 1)? "hit":"miss" ;
            if (address_list[i].hit == 0)
            {
                hit_or_miss = "miss" ;
                miss_count++ ;
            }
            else
                hit_or_miss = "hit" ;

            write_rpt << address_list[i].original << " " << hit_or_miss << endl ;
        }
        write_rpt << lst_last_line << endl << endl ;
        
        write_rpt << "Total cache miss count: " << miss_count ;
        
        write_rpt.close() ;
        
        
        cout << miss_count << endl ;
        for (rit = indexing_bits.begin() ; rit != indexing_bits.end() ; rit++)
          cout << *rit << " " ;
        cout << endl ;
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}















