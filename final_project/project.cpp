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

/*
void find_candidate_index_bits(vector< set<int> > &candidate_index_bits, int *Q, int **C, int address_bits, int block_index_length, int set_index_length, set<int>index_combination)
{
    if (index_combination.size() == set_index_length)
    {
        candidate_index_bits.push_back(index_combination) ;
        return ;
    }
    else
    {
        for (int trys = 0; trys < 3; trys++)
        {
            float largest = 0 ;
            int index = -1 ;
            for (int i = block_index_length; i < address_bits; i++)
            {
                if(Q[i] > largest && !index_combination.count(i))
                {
                    largest = Q[i] ;
                    index = i ;
                }
            }
            index_combination.insert(index) ;
            for (int i = block_index_length; i < address_bits; i++)
            {
                Q[i] *= C[index][i] ;
            }
            find_candidate_index_bits(vector< set<int> > candidate_index_bits, int *Q, int **C, int address_bits, int block_index_length, int set_index_length, set<int>index_combination)
        }
        

    }
}
*/



int max4 (int a, int b, int c, int d)
 {
     int max = -INFINITY ;
     if (a > max)
         max = a ;
     if (b > max)
         max = b ;
     if (c > max)
         max = c ;
     if (d > max)
         max = d ;
     return max ;
 }

 int min4 (int a, int b, int c, int d)
 {
     int min = INFINITY ;
     if (a < min)
         min = a ;
     if (b < min)
         min = b ;
     if (c < min)
         min = c ;
     if (d < min)
         min = d ;
     return min ;
 }

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
        }
        
        if (first1 == -1)
            if (set->at(address.set_index_reversed_dec)->at(i)->nru_bit == 1)
                first1 = i ;
    }
    if (!address.hit)
    {
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
            //cout << address_list_optimize_process[address_list_optimize_process.size()-1] << endl ;
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
        int E[address_bits][address_bits] ;
        int D[address_bits][address_bits] ;
        float C[address_bits][address_bits] ;
        int Z[address_bits] ;
        int O[address_bits] ;
        float Q[address_bits] ;

        
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
            for (int j = 0 ; j < address_bits; j++)
            {
                E[i][j] = 0 ;
                D[i][j] = 0 ;
            }
        
        for (int k = 0 ; k < address_list_optimize_process.size() ; k++)
            for (int i = block_index_length; i < address_bits; i++)
                for (int j = block_index_length ; j < address_bits; j++)
                    if (address_list_optimize_process[k][i] != address_list_optimize_process[k][j])
                        D[i][j]++ ;
                    else
                        E[i][j]++ ;
        
        for (int i = 0; i < address_bits; i++)
        {
            printf("%2d ", i) ;
            for (int j = 0 ; j < address_bits; j++)
            {
                vector<int> min_to_max ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][0]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][1]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][2]) ;
                min_to_max.push_back(left_right_difference_combination_count[i][j][3]) ;
                sort(min_to_max.begin(), min_to_max.end());
                C[i][j] = ( (float)min_to_max[0] * (float)min_to_max[1] ) / ( (float)min_to_max[2] * (float)min_to_max[3] );
                
                //C[i][j] = (float)min(E[i][j], D[i][j]) / (float)max(E[i][j], D[i][j]) ;
                //C[i][j] = (float)min4(left_right_difference_combination_count[i][j][0], left_right_difference_combination_count[i][j][1] , left_right_difference_combination_count[i][j][2], left_right_difference_combination_count[i][j][3]) / (float)max4(left_right_difference_combination_count[i][j][0], left_right_difference_combination_count[i][j][1] , left_right_difference_combination_count[i][j][2], left_right_difference_combination_count[i][j][3]) ;
                if (isnan(C[i][j]))
                    printf("%2.5lf ", 0.0) ;
                else
                    printf("%2.5lf ", C[i][j]) ;
                //cerr << C[i][j] << "  " ;
            }
            cout << endl ;
        }
        
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
        
        cout << "   " ;
        for (int i = 0 ; i < address_bits; i++)
        {
            Q[i] = (float)min(Z[i], O[i]) / (float)max(Z[i], O[i]) ;
            if (isnan(Q[i]))
                printf("%2.5lf ", 0.0) ;
            else
            printf("%2.5lf ", Q[i]) ;
        }
        cout << endl ;
        cout << "   " ;
        for (int i = 0 ; i < address_bits; i++)
            printf("%7d ", i) ;
        cout << endl ;
        
        
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
                for (int i = block_index_length; i<address_bits && indexing_bits.size() < set_index_length; i++) {
                    indexing_bits.insert(i) ;
                }
                break ;
            }
            
            indexing_bits.insert(index) ;
            Q[index] = -1 ;
            cout << index << endl ;
            for (int i = block_index_length; i < address_bits; i++)
            {
                Q[i] *= C[index][i] ;
            }
        }

        
        
        
        //vector< set<int> > candidate_index_bits ;
        
        //find_candidate_index_bits(&candidate_index_bits, Q, C, address_bits, block_index_length, set_index_length, set<int>index_combination) ;

        
        
        
        
        
        
        
        
        //indexing_bits.insert(6) ;
        //indexing_bits.insert(7) ;
        //indexing_bits.insert(8) ;
        //indexing_bits.insert(9) ;
        //indexing_bits.insert(11) ;
        
        //for (set<int>::iterator it = indexing_bits.begin() ; it != indexing_bits.end() ; it++)
        //    cout << *it <<' ' ;
        //cout << endl ;

        

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
        
        
        cout <<"miss: "<< miss_count << endl ;
        for (rit = indexing_bits.begin() ; rit != indexing_bits.end() ; rit++)
          cout << *rit << " " ;
        cout << endl ;
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}














