#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>

void print(std::vector<bool> &vec);
void print(std::vector<std::vector<bool>> &symbol);
void print_codewords(std::vector<bool> &vec);
void vec_xor(std::vector<bool> &v1, const std::vector<bool> v2);
unsigned int distance(std::vector<bool> &v1, const std::vector<bool> &v2);
unsigned int convert_uint(std::vector<bool> &vec);
void read_words_from_file(char const *filename, std::vector<std::vector<bool>> &symbol);
void retrieve_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, std::size_t row, std::size_t col);
void retrieve_special_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, int row, int col);

std::vector<bool> get_format_info(std::vector<std::vector<bool>> &symbol);
void decode_mask(std::vector<std::vector<bool>> &symbol, std::size_t mask_pattern);
std::vector<bool> get_codewords_from_symbol(std::vector<std::vector<bool>> &symbol);
std::string get_input_data(std::vector<bool> &input_data, unsigned int character_count_indicator);

int main(){
	std::vector<std::vector<bool>> symbol(21, std::vector<bool>(21,0));

	read_words_from_file("output.pbm", symbol);
	//print(symbol);
	
	std::vector<bool> format_info = get_format_info(symbol);

	std::cout << "Format Info: "; print(format_info); std::cout << std::endl;
	std::bitset<3> mask_pattern{};	
	mask_pattern[0] = format_info[2];
	mask_pattern[1] = format_info[3];
	mask_pattern[2] = format_info[4];
	decode_mask(symbol, mask_pattern.to_ulong());
	//print(symbol);

	std::vector<bool> codewords = get_codewords_from_symbol(symbol);
	//print_codewords(codewords);
	//std::cout << "Number of codewords: " << codewords.size() / 8.0 << std::endl;
	std::vector<bool> data_codewords(codewords.begin(), codewords.begin() + 16 * 8), error_correction_codewords(codewords.begin() + 16 * 8, codewords.end());

	//std::cout << "Number of data codewords: " << data_codewords.size() / 8.0 << std::endl;
	//std::cout << "Number of error correction codewords: " << error_correction_codewords.size() / 8.0 << std::endl;
	//print_codewords(data_codewords);
	std::vector<bool> mode_indicator(data_codewords.begin(), data_codewords.begin() + 4);
	std::vector<bool> character_count(data_codewords.begin() + 4, data_codewords.begin() + 14);
	unsigned int character_count_indicator = convert_uint(character_count);
	//std::cout << character_count_indicator << std::endl;	
	std::vector<bool> input_data(data_codewords.begin() + 14, data_codewords.end());

	std::string value = get_input_data(input_data, character_count_indicator);
	std::cout << value << std::endl;
	
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

void print_codewords(std::vector<bool> &vec){
	for (std::size_t i{0}; i < vec.size(); ++i){
		if (i != 0 && i % 8 == 0){std::cout << ' ';}
		std::cout << vec[i];
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
unsigned int convert_uint(std::vector<bool> &vec){
	unsigned int result{0};
	for (unsigned int i{0}; i < vec.size(); ++i){
		result += std::pow(2,vec.size() - i - 1) * vec[i];
	}
	return result;
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
	//print(format_info);	
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

void decode_mask(std::vector<std::vector<bool>> &symbol, std::size_t mask_pattern){
	std::vector<std::vector<bool>> data_mask(21, std::vector<bool> (21, 0));
	for (std::size_t i{}; i < 21; ++i){
		for (std::size_t j{}; j < 21; ++j){
			if ((i < 9 && j < 9) || (i > 12 && j < 9) || (i < 9 && j > 12) || i == 6 || j == 6){
				continue;
			}
			// 000
			if (mask_pattern == 0 && (i+j)%2 == 0){
				data_mask[i][j] = 1;
			}
			// 001
			else if (mask_pattern == 1 && i % 2 == 0){
				data_mask[i][j] = 1;
			}
			// 010
			else if (mask_pattern == 2 && j % 3 == 0){
				data_mask[i][j] = 1;
			}
			// 011
			else if (mask_pattern == 3 && (i+j)%3 == 0){
				data_mask[i][j] = 1;
			}
			// 100
			else if (mask_pattern == 4 && (i / 2 + j / 3) % 2 == 0){
				data_mask[i][j] = 1;
			}
			// 101
			else if (mask_pattern == 5 && (i * j) % 2 + (i * j) % 3 == 0){
				data_mask[i][j] = 1;
			}	
			// 110
			else if (mask_pattern == 6 && ((i * j) % 2 + (i * j) % 3 ) % 2 == 0){
				data_mask[i][j] = 1;
			}	
			// 111
			else if (mask_pattern == 7 && (((i + j) % 2 + (i * j) % 3 ) % 2 == 0)){
				data_mask[i][j] = 1;
			}	
		}
	}	 
	
	for (std::size_t i{0}; i < 21; ++i){
		for (std::size_t j{0}; j < 21; ++j){
			symbol[i][j] = symbol[i][j] ^ data_mask[i][j];
		}
	}
}

void retrieve_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, int row, int col){
	if (is_upwards){
		for (int r{row + 3}; r >= row; --r){
			for (int c{col + 1}; c >= col; --c){
				//std::cout << r << ' ' << c << std::endl;
				codewords.push_back(symbol[r][c]);
			}
		}
	}
	else {
		for (int r{row}; r <= row + 3; ++r){
			for (int c{col + 1}; c >= col; --c){
				//std::cout << r << ' ' << c << std::endl;
				codewords.push_back(symbol[r][c]);
			}
		}
	}
}

void retrieve_special_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, int row, int col){
	if (is_upwards){
		for (int r{row + 4}; r >= row; --r){
			for (int c{col + 1}; c >= col; --c){
				if (r == 6){continue;}
				//std::cout << r << ' ' << c << std::endl;
				codewords.push_back(symbol[r][c]);
			}
		}
	}
	else {
		for (int r{row}; r <= row + 4; ++r){
			for (int c{col + 1}; c >= col; --c){
				if (r == 6){continue;}
				//std::cout << r << ' ' << c << std::endl;
				codewords.push_back(symbol[r][c]);
			}
		}
	}
}

std::vector<bool> get_codewords_from_symbol(std::vector<std::vector<bool>> &symbol){
	std::vector<bool> codewords{};
	int c{}, r{};
	// refer to codewords by their top left corner
	
	for (c = 19, r = 17;c >= 13; c-=2){
		for (r = 17;r >= 21 - 3 * 4;r-=4){
			retrieve_codewords(symbol, codewords, true, r, c);
		}
		c -= 2;
		for (r = 9;r < 20; r+=4){
			retrieve_codewords(symbol, codewords, false, r, c);
		}
	}

	for (r = 17;r >= 21 - 3 * 4;r-=4){
		retrieve_codewords(symbol, codewords, true, r, c);
	}
	//std::cout << r << ' ' << c << std::endl;
	// up special 
	r = 4;
	retrieve_special_codewords(symbol, codewords, true, r, c);
	
	r = 0;
	retrieve_codewords(symbol, codewords, true, r, c);
	c-=2;
	retrieve_codewords(symbol, codewords, false, r, c);

	// down special
	r = 4;
	retrieve_special_codewords(symbol, codewords, false, r, c);

	for (r=9;r < 20;r+=4){
		retrieve_codewords(symbol, codewords, false, r, c);
	}

	// data that is between two finder patterns before timing pattern
	c -= 2;
	r = 9;
	retrieve_codewords(symbol, codewords, true, r, c);
	
	// data after timing pattern
	c -= 3;
	retrieve_codewords(symbol, codewords, false, r, c);

	c -= 2;
	retrieve_codewords(symbol, codewords, true, r, c);
	
	c -= 2;
	retrieve_codewords(symbol, codewords, false, r, c);
	return codewords;
}

std::string get_input_data(std::vector<bool> &input_data, unsigned int character_count_indicator){
	std::string data = "";
	unsigned int data_length = character_count_indicator / 3 * 10;
	for (std::size_t i{0}; i < data_length; i += 10){
		std::vector<bool> subvec(input_data.begin() + i, input_data.begin() + i + 10);
		data += std::to_string(convert_uint(subvec));
	}
	if (character_count_indicator % 3 == 1){
		std::vector<bool> subvec(input_data.begin() + data_length, input_data.begin() + data_length + 4);
		data += std::to_string(convert_uint(subvec));
	}
	else if (character_count_indicator % 3 == 2){
		std::vector<bool> subvec(input_data.begin() + data_length, input_data.begin() + data_length + 7);
		data += std::to_string(convert_uint(subvec));
	}
	return data;
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
