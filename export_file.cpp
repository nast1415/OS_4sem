#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "export_file.h"


using namespace std;

void export_f(string dir, string target_file, string local_file) {
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

	int res = parse_string(target_file, directories, sizes, real_directory_name, size_of_real_name);
	if (res != 0) {
		return;
	}

	int number = number_of_the_root_block; //First we're going to search in root
	
	for (int i = 0; i < directories.size(); i++) {
		number = find_directory_name(dir, number, directories[i], sizes[i], alloc_table_array);
		if (number <= 0) {
			cerr << "export: target path is incorrect" << endl;
			return;
		}
	}

	int number_of_last_dir = number; //Number of block, where del file announced
	
	number = 0;
	number = find_file_next_block(dir, number_of_last_dir, real_directory_name, size_of_real_name, alloc_table_array);
	if (number == 0) {
		cerr << "export: there is no target_file with this name on this path" << endl;
		return;
	}

	int file_size = find_file_size(dir, number_of_last_dir, real_directory_name, size_of_real_name, alloc_table_array);

	int next_block_of_last_directory;
	int first_block_of_deleting_dir = number; //Number of the block, where our file is placed
	
	int next_block_number = 0;

    const char * block_name1 = local_file.c_str();
    ofstream outfile(block_name1, ios::binary);
    
	while (true) {
		next_block_number = alloc_table_array[number + 1];
		next_block_number--;

		char buffer_block[100];
		sprintf (buffer_block, "%d", number);
		string name2 = dir + "/block" + buffer_block;
		const char * block_name2 = name2.c_str();

		if (next_block_number == number) {
			char buffer[file_size];
			ifstream infile(block_name2, ios::binary);

			infile.read(buffer, file_size);
			outfile.write(buffer, file_size);			

		} else {
			char buffer[4096];

			ifstream infile(block_name2, ios::binary);

			infile.read(buffer, 4096);
			outfile.write(buffer, 4096);
		}


		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}
}

int main (int argc, char* argv[])
{
	string dir = argv[1];
	string target_file = argv[2];
	string local_file = argv[3];
	export_f(dir, target_file, local_file);
	return 0;
}