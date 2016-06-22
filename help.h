#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std;

struct file_information {
	char name[256];
	int type;
	int size;
	int next_block;
}; //Structure for storing file or directory

void read_info_from_block(FILE* block, struct file_information data_info[], int data_info_size);
void add_info_to_block(string dir, int number_of_block, string info, int size_of_name, int info_type, int size, int next_block, int allocation_table[], int table_size);
int find_directory_name(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]);
int find_file_next_block(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]);
int find_file_size(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]);
void create_update_block_with_info_data(string dir, int number, struct file_information data[], int size);
void create_update_block_with_int_data(string dir, int number, int data[], int size);
void read_data_from_block(int* data, const char* block_name, int size);
int parse_string(string str, vector<string> &directories, vector<int> &sizes, string &real_directory_name, int &size_of_real_name);
void create_data_array(int data[], int size, int size_of_first_part, int filler);