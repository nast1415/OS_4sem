#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "help.h"
#include "rm.h"


using namespace std;

void rm(string dir, string target_dir) {
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
	} //Now we get an allocation table

	

	vector<string> directories;
	vector<int> sizes;

	string real_directory_name;
	int size_of_real_name;

	int res = parse_string(target_dir, directories, sizes, real_directory_name, size_of_real_name);
	if (res != 0) {
		return;
	}

	int number = number_of_the_root_block; //First we're going to search in root
	
	for (int i = 0; i < directories.size(); i++) {
		number = find_directory_name(dir, number, directories[i], sizes[i], alloc_table_array);
		if (number <= 0) {
			cerr << "rm: path is incorrect" << endl;
			return;
		}
	}

	int number_of_last_dir = number; //Number of block, where del file announced
	number = 0;
	number = find_file_next_block(dir, number_of_last_dir, real_directory_name, size_of_real_name, alloc_table_array);
	if (number == 0) {
		cerr << "rm: there is no file with this name on this path" << endl;
		return;
	}
	
	int next_block_of_last_directory;
	int first_block_of_deleting_dir = number; //Number of the block, where our file is placed
	cerr << endl;
	//cerr << "Number of the first block, where our file placed: " << first_block_of_deleting_dir + 1 << endl;
	
	int next_block_number = 0;
	
	while (true) {
		cerr << "We're going to open file number: " << number << endl; 
		char buffer2[100];
		sprintf (buffer2, "%d", number);
		string name2 = dir + "/block" + buffer2;
		const char * block_name2 = name2.c_str();

		int number_of_info[1] = {0};

		FILE * block2;
		block2 = fopen(block_name2, "wb");
		int res_array[number_of_ints_in_block] = {0, };

		fwrite(res_array, sizeof(int), number_of_ints_in_block, block2);

		if (block2) {
			fclose(block2);
		}

		
		next_block_number = alloc_table_array[number + 1];
		next_block_number--;
		alloc_table_array[number + 1] = -1;
		//cerr << "Next block number is: " << next_block_number << endl;

		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}
	
	
	//Delete from announcements
	char buffer3[100];
	sprintf (buffer3, "%d", number_of_last_dir);
	string name3 = dir + "/block" + buffer3;
	const char * block_name3 = name3.c_str();

	FILE * block3 = fopen(block_name3, "rb");

	int number_of_info[1] = {0};

	fread(number_of_info, sizeof(int), 1 , block3); //Read number of the infos
	
	if (block3) {
		fclose(block3);
	}

	int previous_number = 0;

	char new_info[256];

	const char* real_name = real_directory_name.c_str();

	for (int i = 0; i < size_of_real_name; i++) {
		new_info[i] = real_name[i];
	}
	for (int i = size_of_real_name; i < 256; i++) {
		new_info[i] = ' ';
	}
	
	if (number_of_info[0] < 15) {
		block3 = fopen(block_name3, "rb");
		fseek(block3, 4, SEEK_SET);
		struct file_information data[number_of_info[0]];
		fread(data, sizeof(struct file_information), number_of_info[0], block3); //Now we have data array with all infos from this block

		if (block3) {
			fclose(block3);
		}

		int flag_name = 0;
		int deleting_position = 0;

		for (int i = 0; i < number_of_info[0]; i++) {
			flag_name = 0;
			for (int j = 0; j < 256; j++) {
				if (data[i].name[j] != new_info[j]) {
					flag_name = 1;
				}
			}
			if (flag_name == 0) {
				deleting_position = i;
			}
		}

		struct file_information new_data[number_of_info[0] - 1];
		for (int i = 0; i < deleting_position; i++) {
			new_data[i] = data[i];
		}
		for (int i = deleting_position + 1; i < number_of_info[0]; i++) {
			new_data[i - 1] = data[i];
		}

		number_of_info[0]--;

		block3 = fopen(block_name3, "wb");
		fwrite(number_of_info, sizeof(int), 1, block3);
		fwrite(new_data, sizeof(struct file_information), number_of_info[0], block3);

		if (block3) {
			fclose(block3);
		}		
	} else {
		int next_block_number = 0;
		
		number = number_of_last_dir;
		
		vector<struct file_information> result;

		int index2 = 0;
		
		while (true) {

			int number_of_info3[1] = {0};

			char buffer4[100];
			sprintf (buffer4, "%d", number);
			string name4 = dir + "/block" + buffer4;
			const char * block_name4 = name4.c_str();

			FILE * block4;
			block4 = fopen(block_name4, "rb");

			fread(number_of_info3, sizeof(int), 1, block4);
			
			struct file_information data_info3[number_of_info3[0]];
			fread(data_info3, sizeof(struct file_information), number_of_info3[0], block4);

			if (block4) {
				fclose(block4);
			}
			
			int flag_name = 0;
			int deleting_position = -2;
			
			index2 = 0;
			for (int i = 0; i < number_of_info3[0]; i++) {
				flag_name = 0;
				index2++;

				for (int j = 0; j < 256; j++) {
					if (data_info3[i].name[j] != new_info[j]) {
						flag_name = 1;
					}
				}
				if (flag_name == 0) {
					deleting_position = index2;
				}
			}

			if (deleting_position != 0)
				deleting_position--;

			if (deleting_position >= 0) {
				for (int i = 0; i < deleting_position; i++) {
					result.push_back(data_info3[i]);
				}
				for (int i = deleting_position + 1; i < number_of_info3[0]; i++) {
					result.push_back(data_info3[i]);
				}

			} else {
				for (int i = 0; i < number_of_info3[0]; i++) {
					result.push_back(data_info3[i]);
				}
			}

			next_block_number = alloc_table_array[number + 1];
			next_block_number--;

			if (next_block_number == number) {
				break;
			} else {
				previous_number = number;
				number = next_block_number;
			}
		}

		int size_of_res_array = result.size();
		struct file_information res_array[size_of_res_array];

		if (size_of_res_array % 15 == 0) {
			
			char buffer6[100];
			sprintf (buffer6, "%d", number);
			string name6 = dir + "/block" + buffer6;
			const char * block_name6 = name6.c_str();

			FILE * block6;
			block6 = fopen(block_name6, "wb");
			int null_val[1] = {0};

			fwrite(null_val, sizeof(int), 1, block6);

			if (block6) {
				fclose(block6);
			}

			alloc_table_array[number + 1] = -1;
			alloc_table_array[previous_number + 1] = previous_number + 1;
		}

		int blocks = ceil(double(size_of_res_array) / 15);
		int elem_in_last_block = size_of_res_array % 15;
		if (elem_in_last_block == 0) {
			elem_in_last_block = 15;
		}

		number = number_of_last_dir;

		for (int i = 0; i < blocks; i++) {
			char buffer5[100];
			sprintf (buffer5, "%d", number);
			string name5 = dir + "/block" + buffer5;
			const char * block_name5 = name5.c_str();

			FILE * block5;
			block5 = fopen(block_name5, "wb");

			if (i != blocks - 1) {
				int blocks_count[1] = {15};
				fwrite(blocks_count, sizeof(int), 1, block5);

				struct file_information data[15];
				for (int j = 0; j < 15; j++) {
					data[j] = result[(i * 15) + j];
				}
				fwrite(data, sizeof(struct file_information), 15, block5);
			} else {
				int blocks_count[1] = {elem_in_last_block};
				fwrite(blocks_count, sizeof(int), 1, block5);

				struct file_information data[elem_in_last_block];
				for (int j = 0; j < elem_in_last_block; j++) {
					data[j] = result[(i * 15) + j];
				}
				fwrite(data, sizeof(struct file_information), elem_in_last_block, block5);

			}

			if (block5) {
				fclose(block5);
			}

			number = alloc_table_array[number + 1];
			number--;
		}

	}

	//Refreshing the allocation table
	//int number_of_changed_blocks = ceil(double((previous_number + 1) * sizeof(int)) / 4096);
	for (int i = 0; i < number_of_the_root_block; i++) {
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
	string target_file = argv[2];
	rm(dir, target_file);
	return 0;
}