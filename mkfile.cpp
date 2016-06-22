#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "mkfile.h"

using namespace std;

int mkfile(string dir, string target_dir, int target_file_size) {
	int number_of_ints_in_block = floor (4096 / sizeof(int));

	//First we need data from allocation table
	int data[number_of_ints_in_block] = {0, };
	
	char buffer[100];
	sprintf (buffer, "%d", 0);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();

	read_data_from_block(data, block_name, number_of_ints_in_block); //Now we have an array of data from null block

	//Next we need to have first int from this alloc_table
	int number_of_the_root_block = data[0] - 1; //Real number of the root block
	
	//Next we need to read all alloc_table
	int alloc_table_array[number_of_ints_in_block * number_of_the_root_block] = {0, };
	for (int i = 0; i < number_of_the_root_block; i++) {
		int data[number_of_ints_in_block] = {0, };

		char buffer[100];
		sprintf (buffer, "%d", i);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		read_data_from_block(data, block_name, number_of_ints_in_block);

		for (int j = 0; j < number_of_ints_in_block; j++) {
			alloc_table_array[number_of_ints_in_block * i + j] = data[j];
		}
	}

	vector<string> directories;
	vector<int> sizes;

	string real_directory_name;
	int size_of_real_name;

	int res = parse_string(target_dir, directories, sizes, real_directory_name, size_of_real_name);
	if (res != 0) {
		return -1;
	}

	int number = number_of_the_root_block; //First we're going to search in root
	
	for (int i = 0; i < directories.size(); i++) {
		number = find_directory_name(dir, number, directories[i], sizes[i], alloc_table_array);
		if (number <= 0) {
			cerr << "import: path is incorrect" << endl;
			return -1;
		}
	}

	int number_of_last_dir = number;
	
	number = 0;
	number = find_file_next_block(dir, number_of_last_dir, real_directory_name, size_of_real_name, alloc_table_array);
	if (number != 0) {
		cerr << "import: directory or file with this name already exists" << endl;
		return -1;
	}

	int next_block_of_last_directory;
	number_of_last_dir++;

	while (true) {
		next_block_of_last_directory = alloc_table_array[number_of_last_dir];

		if (next_block_of_last_directory == number_of_last_dir) {
			break;
		} else {
			number_of_last_dir = next_block_of_last_directory;
		}
	}
	number_of_last_dir--;


	char buffer2[100];
	sprintf (buffer2, "%d", number_of_last_dir);
	string name2 = dir + "/block" + buffer2;
	const char * block_name2 = name2.c_str();

	FILE * last_block = fopen(block_name2, "rb");
	int numb[1] = {0};

	fread(numb, sizeof(int), 1, last_block);
	if (last_block) {
		fclose(last_block);
	}


	
	int table_size = number_of_ints_in_block * number_of_the_root_block;

	int first_free_block = -2;
	for (int i = 0; i < table_size; i++){
		if (alloc_table_array[i] == -1) {
			first_free_block = i;
			break;
		}
	}

	if (first_free_block == -2) {
		cerr << "import: there is not enough memory for this operations in the file system" << endl;
		return -1;
	}

	first_free_block--; //Get real block name
	
	alloc_table_array[first_free_block + 1] = first_free_block + 1;

	//write info about our directory
	add_info_to_block(dir, number_of_last_dir, real_directory_name, size_of_real_name, 1, target_file_size, first_free_block, alloc_table_array, table_size);
	int number_of_changed_blocks = ceil(double((first_free_block + 2) * sizeof(int)) / 4096);
	for (int i = 0; i < number_of_changed_blocks; i++) {
		int data[number_of_ints_in_block] = {0, };
		for (int j = 0; j < number_of_ints_in_block; j++) {
			data[j] = alloc_table_array[(i * number_of_ints_in_block) + j];
		}

		char buffer[100];
		sprintf (buffer, "%d", i);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		FILE * block = fopen(block_name, "wb");
		fwrite(data, sizeof(int), number_of_ints_in_block, block);

		if (block) {
			fclose(block);
		}
	}
	//cout << "Wow! We done all this stuff!" << endl;
	return first_free_block;
}