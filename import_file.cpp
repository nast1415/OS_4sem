#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "import_file.h"
#include "mkfile.h"



using namespace std;

void import_f(string dir, string local_file, string target_file) {

	const char * real_file_name = local_file.c_str();
	
	fstream file(real_file_name);
    int size = 0;
    file.seekg (0, std::ios::end);
    size = file.tellg();
    file.close();

    int number_of_file_blocks = ceil(double(size) / 4096); //Number of blocks used to write our file
	int size_of_last_block = size % 4096;
	if (size_of_last_block == 0) {
		size_of_last_block = 4096;
	}

	int new_file = mkfile(dir, target_file, size_of_last_block);

	if (new_file == -1) {
		return;
	}

	
	//Load allocation table
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

	int number = new_file;
	int table_size = number_of_ints_in_block * number_of_the_root_block;
	for (int i = 1; i < number_of_file_blocks; i++) {
		
		int first_free_block = -2;
		for (int i = 0; i < table_size; i++){
			if (alloc_table_array[i] == -1) {
				first_free_block = i;
				break;
			}
		}

		if (first_free_block == -2) {
			cerr << "import: there is not enough memory for this operations in the file system" << endl;
		}

		alloc_table_array[number + 1] = first_free_block;
		alloc_table_array[first_free_block] = first_free_block;
		number = first_free_block - 1;

	}

	number = new_file;
	ifstream infile(real_file_name, ios::binary);
	
	for (int i = 0; i < number_of_file_blocks; i++) {
		char buffer2[100];
		sprintf (buffer2, "%d", number);
		string name2 = dir + "/block" + buffer2;
		const char * block_name2 = name2.c_str();

		if (i != number_of_file_blocks - 1) {
			char buffer[4096];
			ofstream outfile(block_name2, ios::binary);

			infile.read(buffer, 4096);
			outfile.write(buffer, 4096);
		} else {
			char buffer[size_of_last_block];
			ofstream outfile(block_name2, ios::binary);

			infile.read(buffer, size_of_last_block);
			outfile.write(buffer, size_of_last_block);
		}

		number = alloc_table_array[number + 1];
		number--;
	}

	int number_of_changed_blocks = ceil(double((number + 2) * sizeof(int)) / 4096);
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

}

int main (int argc, char* argv[])
{
	string dir = argv[1];
	string local_file = argv[2];
	string target_file = argv[3];
	import_f(dir, local_file, target_file);
	return 0;
}