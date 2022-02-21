#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>
//#include <stdlib.h>
#include <cstdlib>


using namespace std;

#define BASIC 0
#define OPTIMIZE 1
#define EXHAUSTIVE 1 // OPTIMIZE has to = 1
#define MANUAL 0
#define MAX_COUNT 150


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
    void reset() ;
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



void address_class::reset()
{
    hit = 0 ;
    set_index_reversed = "" ;
    set_index_reversed_dec = 0 ;
    tag_index_reversed = "" ;
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
    int cache_sets ;
    cache_class(int associativity, int cache_sets) ;
    bool access(address_class &address) ;
    void reset() ;
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





bool cache_class::access(address_class &address)
{
    int first1 = -1 ;
    
    for (int i = 0 ; i < associativity ; i++)
    {
        if ( set->at(address.set_index_reversed_dec)->at(i)->tag == address.tag_index_reversed )
        {
            address.hit = 1 ;
            set->at(address.set_index_reversed_dec)->at(i)->nru_bit = 0 ;
        }
        
        if (first1 == -1)
            if (set->at(address.set_index_reversed_dec)->at(i)->nru_bit == 1)
                first1 = i ;
    }
    if (!address.hit)
    {
        if (first1 == -1)
        {
            for (int i = 0 ; i < associativity ; i++)
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
    
    return address.hit ;
}


void cache_class::reset()
{
    for (int i = 0; i < set->size(); i++)
    {
        for (int j = 0; j < set->at(i)->size(); j++)
        {
            set->at(i)->at(j)->nru_bit = 1;
            set->at(i)->at(j)->tag = "";
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
    

    
    if (BASIC)
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
            address_list[i].set_tag_selection_not_optimized(set_index_start, set_index_length, tag_index_start, tag_index_length) ;

    }
    
    
    
    if (OPTIMIZE)
    {
        float C[address_bits][address_bits] ;
        int Z[address_bits] ;
        int O[address_bits] ;
        float Q[address_bits] ;

        
        
        //Correlation table
        int left_right_difference_combination_count[address_bits][address_bits][4] ;
        
        for (int j = 0; j < address_bits; j++)
            for (int k = 0 ; k < address_bits; k++)
            {
                left_right_difference_combination_count[j][k][0] = 0 ;
                left_right_difference_combination_count[j][k][1] = 0 ;
                left_right_difference_combination_count[j][k][2] = 0 ;
                left_right_difference_combination_count[j][k][3] = 0 ;
            }
        
                                
        for (int i = 0 ; i < address_list_optimize_process.size() ; i++)
        {
            for (int j = block_index_length ; j < address_bits ; j++)
            {
                for (int k = block_index_length ; k < address_bits ; k++)
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
        

        
        for (int i = 0; i < address_bits; i++)
        {
            for (int j = 0 ; j < address_bits; j++)
            {
                vector<int> min_to_max ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][0]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][1]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][2]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][3]) ;
                sort(min_to_max.begin(), min_to_max.end());
                C[i][j] = ( (float)min_to_max[0] * (float)min_to_max[1] ) / ( (float)min_to_max[2] * (float)min_to_max[3] );
            }
        }
        
        
        //Quality table
        for (int i = 0; i < address_bits; i++)
        {
            Z[i] = 0 ;
            O[i] = 0 ;
        }
        
        for (int k = 0 ; k < address_list_optimize_process.size() ; k++)
            for (int i = block_index_length; i < address_bits; i++)
                if (address_list_optimize_process[k][i] == '0')
                    Z[i]++ ;
                else
                    O[i]++ ;
        
        for (int i = 0 ; i < address_bits; i++)
            Q[i] = (float)min(Z[i], O[i]) / (float)max(Z[i], O[i]) ;


        
        // optimize alg.
        if (!EXHAUSTIVE)
        {
            //select bits
            while (indexing_bits.size() < set_index_length)
            {
                float largest = 0 ;
                int index = -1 ;
                for (int i = block_index_length; i < address_bits; i++)
                {
                    if(Q[i] > largest && !indexing_bits.count(i))
                    {
                        largest = Q[i] ;
                        index = i ;
                    }
                }
                if (index == -1)
                {
                    for (int i = block_index_length; i<address_bits && indexing_bits.size() < set_index_length; i++)
                        indexing_bits.insert(i) ;
                    break ;
                }
                
                indexing_bits.insert(index) ;
                Q[index] = -1 ;
                for (int i = block_index_length; i < address_bits; i++)
                    Q[i] *= C[index][i] ;
            }
            
            
            set<int> tag_bits ;
            for (int i = block_index_length ; i < address_bits ; i++)
                if (!indexing_bits.count(i))
                    tag_bits.insert(i) ;
            

            for (int i = 0; i < address_list.size(); i++)
                address_list[i].set_tag_selection_optimized(set_index_length, indexing_bits, tag_index_length, tag_bits) ;
        }
        
        
        
        

        
        
        
        if (EXHAUSTIVE)
        {
            //exhaustive method
            vector<int> candidate_index_bits ;
            
            for (int i = block_index_length; i < address_bits; i++)
                for (int j = block_index_length; j < address_bits; j++)
                    if (C[i][j] > 0 && find(candidate_index_bits.begin(), candidate_index_bits.end(), j) == candidate_index_bits.end() )
                        candidate_index_bits.push_back(j) ;
                        
            
            // add bits if there is not enough
            for (int i = block_index_length; i<address_bits && candidate_index_bits.size() < set_index_length ; i++)
            {
                int largest = 0 ;
                int temp_index = block_index_length ;
                for (int j = block_index_length; j < address_bits; j++)
                {
                    if (Q[j] > largest) {
                        largest = Q[j] ;
                        temp_index = j ;
                    }
                }
                Q[temp_index] = 0 ;
                
                if (find(candidate_index_bits.begin(), candidate_index_bits.end(), temp_index) == candidate_index_bits.end() )
                    candidate_index_bits.push_back(temp_index) ;
            }
            
            
            // add bits sequentially if there is still not enough
            for (int i = block_index_length; i<address_bits && candidate_index_bits.size() < set_index_length ; i++)
                if (find(candidate_index_bits.begin(), candidate_index_bits.end(), i) == candidate_index_bits.end() )
                    candidate_index_bits.push_back(i) ;

            
            
            sort(candidate_index_bits.begin(), candidate_index_bits.end()) ;
            
            
            
            
            vector< set<int> > candidate_index_bits_combination ;
            
            vector<bool> v(candidate_index_bits.size());
            fill(v.begin(), v.begin() + set_index_length, true);
            do {
                vector<int> temp ;
                for (int i = 0; i < candidate_index_bits.size(); i++) {
                    if (v[i]) {
                        temp.push_back(candidate_index_bits[i]) ;
                    }
                }
                set<int> temp_set ;
                for (int i = 0 ; i < temp.size(); i++)
                    temp_set.insert(temp[i]) ;
                
                
                candidate_index_bits_combination.push_back(temp_set) ;
                
            } while (prev_permutation(v.begin(), v.end())) ;
            
            

            
            cache_class cache_exhaustive(associativity, cache_sets) ;
            int exhasutive_miss_count = 0 ;
            int exhasutive_miss_count_least = INFINITY ;
            
            int loop_count ;
            
            if (candidate_index_bits_combination.size() > MAX_COUNT)
                loop_count = MAX_COUNT ;
            else
                loop_count = candidate_index_bits_combination.size() ;
            
            for (int i = 0; i < loop_count ; i++)
            {
                exhasutive_miss_count = 0 ;
                                
                set<int> tag_bits ;
                for (int j = block_index_length ; j < address_bits ; j++)
                    if (!candidate_index_bits_combination[i].count(j))
                        tag_bits.insert(j) ;
                                
                
                
                for (int j = 0; j < address_list.size(); j++)
                    address_list[j].set_tag_selection_optimized(set_index_length, candidate_index_bits_combination[i], tag_index_length, tag_bits) ;
                
                
                for (int j = 0; j < address_list.size(); j++)
                {
                    if(!cache_exhaustive.access(address_list[j]) )
                        exhasutive_miss_count++ ;

                    if (exhasutive_miss_count > exhasutive_miss_count_least)
                        break ;
                }
                
                if (exhasutive_miss_count < exhasutive_miss_count_least)
                {
                    exhasutive_miss_count_least = exhasutive_miss_count ;
                    indexing_bits = candidate_index_bits_combination[i] ;
                }
                

                
                for (int j = 0; j < address_list.size(); j++)
                    address_list[j].reset() ;
                
                cache_exhaustive.reset() ;
                
            }
            
            
            
            set<int> tag_bits ;
            for (int i = block_index_length ; i < address_bits ; i++)
                if (!indexing_bits.count(i))
                    tag_bits.insert(i) ;
            

            for (int i = 0; i < address_list.size(); i++)
                address_list[i].set_tag_selection_optimized(set_index_length, indexing_bits, tag_index_length, tag_bits) ;
                        
        }
        
            
    }
    
    
    
    
    
    
    if (MANUAL)
    {
        
        indexing_bits.insert(6) ;
        indexing_bits.insert(7) ;
        indexing_bits.insert(8) ;
        indexing_bits.insert(9) ;
        indexing_bits.insert(12) ;
        
        
        set<int> tag_bits ;
        for (int i = block_index_length ; i < address_bits ; i++)
            if (!indexing_bits.count(i))
                tag_bits.insert(i) ;

        

        for (int i = 0; i < address_list.size(); i++)
            address_list[i].set_tag_selection_optimized(set_index_length, indexing_bits, tag_index_length, tag_bits) ;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //create cache
    cache_class cache(associativity, cache_sets) ;
    

    
    
    //access cache process
    for (int i = 0 ; i < address_list.size() ; i++)
    {
        cache.access(address_list[i]) ;
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
        
        
        cout <<"miss: "<< miss_count << endl ;
        for (rit = indexing_bits.begin() ; rit != indexing_bits.end() ; rit++)
          cout << *rit << " " ;
        cout << endl ;
        
    }
    
    
    return 0;
}
