#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"


using namespace std;

void read_info_from_block(FILE* block, struct file_information data_info[], int data_info_size) {
	
	fread(data_info, sizeof(struct file_information), data_info_size, block);
	
	if (block) {
		fclose(block);
	}
} //Read an array of file_information structures (we use this function after we read number of infos in the block)

void add_info_to_block(string dir, int number_of_block, string info, int size_of_name, int info_type, int size, int next_block, int allocation_table[], int table_size) {
	//Create new info object with our parameters
	const char * info_name = info.c_str();
	struct file_information new_info;

	for (int i = 0; i < size_of_name; i++) {
		new_info.name[i] = info_name[i];
	}
	for (int i = size_of_name; i < 256; i++) {
		new_info.name[i] = ' ';
	}

	new_info.type = info_type;
	new_info.size = size;
	new_info.next_block = next_block;

	struct file_information new_data[1] = {new_info};
	
	//Get block name
	char buffer[100];
	sprintf (buffer, "%d", number_of_block);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();

	int number_of_info[1] = {0};

	FILE * block;
	block = fopen(block_name, "rb");
	fread(number_of_info, sizeof(int), 1 , block); //Read number of the infos
	
	if (block) {
		fclose(block);
	}

	if (number_of_info[0] < 15) {
		//cout << "I'm in first case!" << endl;
		block = fopen(block_name, "rb");
		fseek(block, 4, SEEK_SET);
		struct file_information data[number_of_info[0] + 1];
		for (int i = 0; i < number_of_info[0]; i++) {
			fread(data, sizeof(struct file_information), number_of_info[0], block);
		} //Now we have data array with all infos from this block

		if (block) {
			fclose(block);
		}

		data[number_of_info[0]] = new_info;
		number_of_info[0]++;

		block = fopen(block_name, "wb");
		fwrite(number_of_info, sizeof(int), 1, block);
		fwrite(data, sizeof(struct file_information), number_of_info[0], block);


		if (block) {
			fclose(block);
		}


		int read_int_info[1] = {0};

		block = fopen(block_name, "rb");
		fread(read_int_info, sizeof(int), 1, block);
		
		struct file_information data_info[read_int_info[0]];

		read_info_from_block(block, data_info, read_int_info[0]);

	} else {
		//cout << "I'm in second case!" << endl; 
		int first_free_block = -2;
		for (int i = 0; i < table_size; i++){
			if (allocation_table[i] == -1) {
				first_free_block = i;
				break;
			}
		}

		if (first_free_block == -2) {
			cerr << "There is not enough memory in the file system!" << endl;
			return;
		}

		first_free_block--; //Get real block name
		//cout << "I find free block, it's number: " << first_free_block << endl;

		char buffer[100];
		sprintf (buffer, "%d", first_free_block);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		int number_of_info[1] = {1};

		FILE * block;
		block = fopen(block_name, "wb");
		fwrite(number_of_info, sizeof(int), 1, block);
		fwrite(new_data, sizeof(struct file_information), 1, block);

		if (block) {
			fclose(block);
		}

		number_of_block++;

		//cout << "Number of block: " << number_of_block << endl;

		allocation_table[first_free_block + 1] = first_free_block + 1;
		allocation_table[number_of_block] = first_free_block + 1;

	}

} //This function add an info structure to the block (and update the allocation table if it need to be updated)

int find_file_next_block(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[])  {

	int number = number_of_block;
	int next_block_number = 0;
	const char * find_name = dir_name.c_str();
	
	char new_info[256];

	for (int i = 0; i < dir_size; i++) {
		new_info[i] = find_name[i];
	}
	for (int i = dir_size; i < 256; i++) {
		new_info[i] = ' ';
	}

	int flag_name = 0;
	int flag_type = 0;

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
			flag_name = 0;
			flag_type = 0;
			for (int j = 0; j < 256; j++) {
				if (data_info[i].name[j] != new_info[j]) {
					flag_name = 1;
				}
			}
			if (data_info[i].type == 0) {
				flag_type = 1;
			}

			if ((flag_name == 0) && (flag_type == 0)) {
				return data_info[i].next_block;
			}
		}

		next_block_number = allocation_table[number + 1];
		next_block_number--;

		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}

	return 0;

}

int find_file_size(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]) {
	int number = number_of_block;
	int next_block_number = 0;
	const char * find_name = dir_name.c_str();
	
	char new_info[256];

	for (int i = 0; i < dir_size; i++) {
		new_info[i] = find_name[i];
	}
	for (int i = dir_size; i < 256; i++) {
		new_info[i] = ' ';
	}

	int flag_name = 0;
	int flag_type = 0;

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
			flag_name = 0;
			flag_type = 0;
			for (int j = 0; j < 256; j++) {
				if (data_info[i].name[j] != new_info[j]) {
					flag_name = 1;
				}
			}
			if (data_info[i].type == 0) {
				flag_type = 1;
			}

			if ((flag_name == 0) && (flag_type == 0)) {
				return data_info[i].size;
			}
		}

		next_block_number = allocation_table[number + 1];
		next_block_number--;

		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}

	return 0;

}
int find_directory_name(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]) {

	int number = number_of_block;
	int next_block_number = 0;
	const char * find_name = dir_name.c_str();
	
	char new_info[256];

	for (int i = 0; i < dir_size; i++) {
		new_info[i] = find_name[i];
	}
	for (int i = dir_size; i < 256; i++) {
		new_info[i] = ' ';
	}

	int flag_name = 0;
	int flag_type = 0;

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
			flag_name = 0;
			flag_type = 0;
			for (int j = 0; j < 256; j++) {
				if (data_info[i].name[j] != new_info[j]) {
					flag_name = 1;
				}
			}
			if (data_info[i].type == 1) {
				flag_type = 1;
			}

			if ((flag_name == 0) && (flag_type == 0)) {
				return data_info[i].next_block;
			}
			if ((flag_name == 0) && (flag_type == 1)) {
				return -data_info[i].next_block;
			}
		}

		next_block_number = allocation_table[number + 1];
		next_block_number--;

		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}

	return 0;
} //This function checks, is it true that the directory named find_name is placed in this directory (which first block we give as a parameter)

void create_update_block_with_info_data(string dir, int number, struct file_information data[], int size) {
	//Get block name
	char buffer[100];
	sprintf (buffer, "%d", number);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();
	
	
	//Create block with block_name and fill it with data
	FILE * block;
	block = fopen(block_name, "wb");
	fwrite(data, sizeof(struct file_information), size, block);
	fclose(block);
}

void create_update_block_with_int_data(string dir, int number, int data[], int size) {
	//Get block name
	char buffer[100];
	sprintf (buffer, "%d", number);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();
	
	
	//Create block with block_name and fill it with data
	FILE * block;
	block = fopen(block_name, "wb");
	fwrite(data, sizeof(int), size, block);
	fclose(block);
} //This function helps us with the format function


void read_data_from_block(int* data, const char* block_name, int size) {
	FILE * block;
	block = fopen (block_name, "rb");
	fread(data, sizeof(int), size , block);
	if (block) {
		fclose(block);
	}
} //We need this function for format tests and mkdir


int parse_string(string str, vector<string> &directories, vector<int> &sizes, string &real_directory_name, int &size_of_real_name) {
	string part_of_the_path = "";
	int size_of_the_string = 0;
	string work_str = str + " "; //Add space at the end of our string (because we know that our path don't contain spaces)

	string::iterator first = work_str.begin(); //Create string iterator and now it is in the beginning of our string
	
	if (*first != '/') {
		cerr << "Error: path need to begin with /" << endl;
		return -1;
	}

	first++;
	
	while (*first != ' ') {
		while ((*first != '/') && (*first != ' ')) {
			part_of_the_path += *first;
			size_of_the_string++;
			first++;
		}

		directories.push_back(part_of_the_path);
		sizes.push_back(size_of_the_string);


		part_of_the_path = "";
		size_of_the_string = 0;

		if (*first != ' ') {
			first++;
		}
	}
	
	if (directories.size() != 0) {
		string new_dir_name = directories[directories.size() - 1];
		directories.pop_back();
		int new_dir_size = sizes[sizes.size() - 1];
		sizes.pop_back();

		real_directory_name = new_dir_name;
		size_of_real_name = new_dir_size;
	}

	return 0;
} //We need this function for many other functions such as mkdir

void create_data_array(int data[], int size, int size_of_first_part, int filler) {
	data[0] = size_of_first_part + 1;

	for (int i = 1; i < size_of_first_part; i++) {
		data[i] = i + 1;
	}

	data[size_of_first_part] = size_of_first_part;
	data[size_of_first_part + 1] = size_of_first_part + 1;

	for (int i = size_of_first_part + 2; i < size; i++) {
		data[i] = filler;
	}
}
