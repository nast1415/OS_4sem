#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "format.h"

using namespace std;

void format (string dir1, int blocks_count) {
	
	const char * dir = dir1.c_str();
	int number_of_info_blocks= ceil(double((blocks_count + 1) * sizeof(int)) / 4096);
	int number_of_ints_in_block = floor(4096 / sizeof(int));
	int size_of_last_info_block =number_of_ints_in_block - ceil(double(number_of_info_blocks * 4096 / sizeof(int) - (blocks_count + 1)));	
	
	
	if (number_of_info_blocks < 1022) { 
		
		int information_data[number_of_ints_in_block] = {0, }; //Array size of one block, filled with nulls

		create_data_array(information_data, number_of_ints_in_block, number_of_info_blocks, -1);
		create_update_block_with_int_data(dir, 0, information_data, number_of_ints_in_block); //Create null block

	} else {
		
		int real_size = ceil(double((number_of_info_blocks + 2) * sizeof(int)) / 4096); //Real size shows us the real number of blocks we need to fill 
		int size_of_array = (real_size * number_of_ints_in_block); //How many blocks we need to use for first number_of_info_blocks elements of our allocation table

		int information_data[size_of_array] = {0, }; //Create  an array with size that divide by number of ints blocks

		create_data_array(information_data, size_of_array, number_of_info_blocks, -1);
		

		int itog_data[number_of_ints_in_block] = {0, };
		for (int i = 0; i < real_size; i++) {
			int index = 0;
			for (int j = i * number_of_ints_in_block; j < (i + 1) * number_of_ints_in_block; j++) {
				itog_data[index] = information_data[j];
				index++;
			}
			create_update_block_with_int_data(dir, i, itog_data, number_of_ints_in_block);
		}
		
	}
	

	int null_data[number_of_ints_in_block] = {0, };
	int real_size = ceil(double(number_of_info_blocks * sizeof(int)) / 1047551); //Real size shows us the real number of blocks we need to fill 
		
	//Fill all blocks with 0
	for (int i = real_size; i < blocks_count; i++) {
		create_update_block_with_int_data(dir, i, null_data, number_of_ints_in_block);
	}

	
	//Fill all alloc table blocks exept first real_size and last

	int alloc_table_data[number_of_ints_in_block] = {0, };
	for (int i = 0; i < number_of_ints_in_block; i++) {
		alloc_table_data[i] = -1;
	}
	
	for (int i = real_size; i < number_of_info_blocks - 1; i++) {
		create_update_block_with_int_data(dir, i, alloc_table_data, number_of_ints_in_block);
	}

	// Fill last alloc table block
	
	if (number_of_info_blocks != 1) {
		int last_alloc_table_data[number_of_ints_in_block] = {0, };
		for (int i = 0; i < size_of_last_info_block; i++) {
			last_alloc_table_data[i] = -1;
		}
		

		create_update_block_with_int_data(dir, number_of_info_blocks - 1, last_alloc_table_data, size_of_last_info_block);
	}	
}

int main (int argc, char* argv[])
{
	string dir = argv[1];
	string blocks_count_str = argv[2];
	const char * blocks_count_ch = blocks_count_str.c_str();
	int blocks_count = atoi(blocks_count_ch);
	format(dir, blocks_count);
	return 0;
}
