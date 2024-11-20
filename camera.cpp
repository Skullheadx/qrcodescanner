#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>

void print(std::vector<bool> &vec);
void print(std::vector<std::vector<bool>> &symbol);
void vec_xor(std::vector<bool> &v1, const std::vector<bool> v2);
unsigned int distance(std::vector<bool> &v1, const std::vector<bool> &v2);
void read_words_from_file(char const *filename, std::vector<std::vector<bool>> &symbol);

std::vector<bool> get_format_info(std::vector<std::vector<bool>> &symbol);

int main(){
	std::vector<std::vector<bool>> symbol(21, std::vector<bool>(21,0));

	read_words_from_file("output.pbm", symbol);
	print(symbol);
	
	std::vector<bool> format_info = get_format_info(symbol);

	print(format_info);
	
	return 0;
}


void print(std::vector<std::vector<bool>> &symbol){
	for (const auto &i : symbol){
		for (auto j : i){
			std::cout << j;
	      }
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void print(std::vector<bool> &vec){
	for (auto i : vec){
		std::cout << i;
	}
	std::cout << std::endl;
}

void vec_xor(std::vector<bool> &v1, const std::vector<bool> v2){
	for (std::size_t i{0}; i < v1.size(); ++i){
		v1[i] = v1[i] ^ v2[i];
	}
}

unsigned int distance(std::vector<bool> &v1, const std::vector<bool> &v2){
	unsigned int dist{0};
	for (std::size_t i{0}; i < v1.size(); ++i){
		if (v1[i] != v2[i]){
			++dist;
		}

		if (dist > 3){
			return dist;
		}
	}
	return dist;
}
		

std::vector<bool> get_format_info(std::vector<std::vector<bool>> &symbol){
	std::vector<bool> format_info(15,0);
	for (std::size_t row{0}, counter{14}; row < 21; ++row){
		if (row == 6 || (row >= 9 && row <= 13)){
			continue;
		}
		format_info[counter] = (symbol[row][8]);
		--counter;
	}

	std::vector<std::vector<bool>> valid_bit_format_info = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,1,0,0,1,1,0,1,1,1},
		{0,0,0,1,0,1,0,0,1,1,0,1,1,1,0},
		{0,0,0,1,1,1,1,0,1,0,1,1,0,0,1},
		{0,0,1,0,0,0,1,1,1,1,0,1,0,1,1},
		{0,0,1,0,1,0,0,1,1,0,1,1,1,0,0},
		{0,0,1,1,0,1,1,1,0,0,0,0,1,0,1},
		{0,0,1,1,1,1,0,1,0,1,1,0,0,1,0},
		{0,1,0,0,0,1,1,1,1,0,1,0,1,1,0},
		{0,1,0,0,1,1,0,1,1,1,0,0,0,0,1},
		{0,1,0,1,0,0,1,1,0,1,1,1,0,0,0},
		{0,1,0,1,1,0,0,1,0,0,0,1,1,1,1},
		{0,1,1,0,0,1,0,0,0,1,1,1,1,0,1},
		{0,1,1,0,1,1,1,0,0,0,0,1,0,1,0},
		{0,1,1,1,0,0,0,0,1,0,1,0,0,1,1},
		{0,1,1,1,1,0,1,0,1,1,0,0,1,0,0},
		{1,0,0,0,0,1,0,1,0,0,1,1,0,1,1},
		{1,0,0,0,1,1,1,1,0,1,0,1,1,0,0},
		{1,0,0,1,0,0,0,1,1,1,1,0,1,0,1},
		{1,0,0,1,1,0,1,1,1,0,0,0,0,1,0},
		{1,0,1,0,0,1,1,0,1,1,1,0,0,0,0},
		{1,0,1,0,1,1,0,0,1,0,0,0,1,1,1},
		{1,0,1,1,0,0,1,0,0,0,1,1,1,1,0},
		{1,0,1,1,1,0,0,0,0,1,0,1,0,0,1},
		{1,1,0,0,0,0,1,0,1,0,0,1,1,0,1},
		{1,1,0,0,1,0,0,0,1,1,1,1,0,1,0},
		{1,1,0,1,0,1,1,0,0,1,0,0,0,1,1},
		{1,1,0,1,1,1,0,0,0,0,1,0,1,0,0},
		{1,1,1,0,0,0,0,1,0,1,0,0,1,1,0},
		{1,1,1,0,1,0,1,1,0,0,1,0,0,0,1},
		{1,1,1,1,0,1,0,1,1,0,0,1,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};
	
	std::vector<bool> mask = {1,0,1,0,1,0,0,0,0,0,1,0,0,1,0};
	vec_xor(format_info, mask);

	unsigned int min_dist = distance(format_info, valid_bit_format_info[0]);
	std::size_t min_dist_index{0};
	for (std::size_t i{0}; i < valid_bit_format_info.size(); ++i){
		unsigned int curr_dist = distance(format_info, valid_bit_format_info[i]);
		if (curr_dist <= min_dist){
			min_dist = curr_dist;
			min_dist_index = i;
			if (curr_dist == 0){
				break;
			}
		}
	}
	//std::cout << min_dist << " " << std::endl; 
	if (min_dist <= 3){
		format_info = valid_bit_format_info[min_dist_index];
	}
	else{
		throw std::runtime_error("format information incorrectly scanned!");
	}

	return format_info;
}

void read_words_from_file(char const *filename, std::vector<std::vector<bool>> &symbol) {
	std::ifstream file{ filename };
	if (!file.is_open()) {
	std::cout << "[ERROR] " << filename << " not found or could not open file" << std::endl;
	}
	assert( file.is_open() );
	
	file.ignore(9);
	std::size_t row{0};
	for (std::string line; std::getline(file, line);++row){
		for (std::size_t pos{0}; pos < line.size(); pos+=2){
			symbol[row][pos/2] = (line[pos] == '1');
		}
	}
	file.close();
}
