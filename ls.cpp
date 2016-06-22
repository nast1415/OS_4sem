#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "ls.h"


using namespace std;


void ls(string dir, string target_dir) {
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
		return;
	}

	const char * real_name = real_directory_name.c_str();

	if (strcmp(real_name, "") != 0) {
		directories.push_back(real_directory_name);
		sizes.push_back(size_of_real_name);
	}

	int number = number_of_the_root_block; //First we're going to search in root
	
	for (int i = 0; i < directories.size(); i++) {
		number = find_directory_name(dir, number, directories[i], sizes[i], alloc_table_array);
		if (number == 0) {
			cerr << "Error: path is incorrect" << endl;
			return;
		}
	}
	
	int next_block_number = 0;
	
	while (true) {


		char buffer[100];
		sprintf (buffer, "%d", number);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		int number_of_info[1] = {0};

		FILE * block;
		block = fopen(block_name, "rb");

		fread(number_of_info, sizeof(int), 1, block);
		struct file_information data_info[number_of_info[0]];

		fread(data_info, sizeof(struct file_information), number_of_info[0], block);
	
		for (int i = 0; i < number_of_info[0]; i++) {
			int index = 0;
			char ch = data_info[i].name[index];
			
			string file_name = "";

			while (ch != ' ') {
				ch = data_info[i].name[index];
				index++;

				if (ch != ' ') {
					file_name += ch;
				}

			}
			string type = "";

			if (data_info[i].type == 0) {
				type = "dir";
			} else {
				type = "file";
			}
			cout << file_name  << " " << type  << endl; 
			
		}



		next_block_number = alloc_table_array[number + 1];
		next_block_number--;

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
	string target_dir = argv[2];
	ls(dir, target_dir);
	return 0;
}