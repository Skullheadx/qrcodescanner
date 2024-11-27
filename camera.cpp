#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>


void print(std::vector<bool> &vec);
void print(std::vector<std::vector<bool>> &symbol);
void print(std::vector<std::vector<unsigned int>> &grid);
void print_codewords(std::vector<bool> &vec);
void vec_xor(std::vector<bool> &v1, const std::vector<bool> v2);
unsigned int distance(std::vector<bool> &v1, const std::vector<bool> &v2);
unsigned int convert_uint(std::vector<bool> &vec);
std::string add_preceding_zeros(unsigned int n);
void read_from_file(std::string filename, std::vector<std::vector<bool>> &symbol);
void retrieve_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, std::size_t row, std::size_t col);
void retrieve_special_codewords(std::vector<std::vector<bool>> &symbol, std::vector<bool> &codewords, bool is_upwards, int row, int col);

void apply_global_threshold(std::vector<std::vector<unsigned int>> &grid, std::vector<std::vector<bool>> &symbol);
std::vector<std::vector<int>> get_finder_coords(std::vector<std::vector<bool>> &symbol);
std::vector<bool> get_format_info(std::vector<std::vector<bool>> &symbol);
void decode_mask(std::vector<std::vector<bool>> &symbol, std::size_t mask_pattern);
std::vector<bool> get_codewords_from_symbol(std::vector<std::vector<bool>> &symbol);
std::string get_input_data(std::vector<bool> &input_data, unsigned int character_count_indicator);
std::string decode_symbol(std::vector<std::vector<bool>> &symbol);

int main(){
	std::vector<std::vector<unsigned int>> camera_input(32, std::vector<unsigned int>(24,0)); // 800 x 600
	unsigned int counter{0};
	for (auto &r : camera_input){
		for (auto &col:r){
			col = 255 * (counter / 32.0);
		}
		counter++;
	}
	camera_input[0][0] = 255;
	camera_input[0][1] = 0;
	camera_input[0][2] = 255;
	camera_input[0][3] = 255;
	camera_input[0][4] = 255;
	camera_input[0][5] = 0;
	camera_input[0][6] = 255;
	camera_input[0][7] = 0;

	camera_input[0][8] = 255;
	camera_input[0][9] = 0;
	camera_input[0][10] = 255;
	camera_input[0][11] = 255;
	camera_input[0][12] = 255;
	camera_input[0][13] = 0;
	camera_input[0][14] = 255;

	camera_input[1][0] = 255;
	camera_input[1][1] = 0;
	camera_input[1][2] = 255;
	camera_input[1][3] = 255;
	camera_input[1][4] = 255;
	camera_input[1][5] = 0;
	camera_input[1][6] = 255;
	camera_input[1][7] = 0;

	camera_input[1][8] = 255;
	camera_input[1][9] = 0;
	camera_input[1][10] = 255;
	camera_input[1][11] = 255;
	camera_input[1][12] = 255;
	camera_input[1][13] = 0;
	camera_input[1][14] = 255;

	camera_input[5][0] = 255;
	camera_input[5][1] = 0;
	camera_input[5][2] = 255;
	camera_input[5][3] = 255;
	camera_input[5][4] = 255;
	camera_input[5][5] = 0;
	camera_input[5][6] = 255;
	camera_input[5][7] = 0;

	//print(camera_input);
	std::vector<std::vector<bool>> symbol_noisy(32, std::vector<bool>(24,0));
	apply_global_threshold(camera_input, symbol_noisy);
	print(symbol_noisy);
	
	get_finder_coords(symbol_noisy);
	

	std::vector<std::vector<bool>> symbol(21, std::vector<bool>(21,0));
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

void print(std::vector<std::vector<unsigned int>> &grid){
	for (auto i :grid){for (auto j:i){std::cout << j << '\t';}std::cout << std::endl;}
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

std::string add_preceding_zeros(unsigned int n){
	std::string result = std::to_string(n);
	if (result.size() == 1){
		result.insert(0, "0");
		result.insert(0, "0");
	}
	else if (result.size() == 2){
		result.insert(0, "0");
	}
	return result;
}
	

void apply_global_threshold(std::vector<std::vector<unsigned int>> &grid, std::vector<std::vector<bool>> &symbol){
	unsigned int max{0}, min{255};
	for (auto &r : grid){
		for (auto &col:r){
			if (col > max){max = col;}
			if (col < min){min = col;}
		}
	}
	unsigned int midway_reflectance{(max + min) / 2};
	for (std::size_t i{0}; i < symbol.size(); ++i){
		for (std::size_t j{0}; j < symbol[0].size(); ++j){
			if (grid[i][j] >= midway_reflectance){
				symbol[i][j] = 1;
			}
			else {
				symbol[i][j] = 0;
			}
		}
	}
}
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
bool contains_ratio(std::vector<int> subvec){
	int divisor = gcd(gcd(gcd(gcd(subvec[0], subvec[1]), subvec[2]), subvec[3]), subvec[4]);
	std::vector<int> vec = subvec;
	for (auto &i: vec){
		i /= divisor;
	}
	//std::cout << "check ratio: "; for (auto i:vec){std::cout << i << " ";}std::cout << std::endl;
	if ((vec[0] > 0.5 && vec[0] < 1.5) && (vec[1] > 0.5 && vec[1] < 1.5) && (vec[3] > 0.5 && vec[3] < 1.5) && (vec[4] > 0.5 && vec[4] < 1.5) && (vec[2] > 2.5 && vec[2] < 3.5)){
		return true;
	}
	return false;
}


std::vector<int> get_finder_pattern_coords(std::vector<bool> &row){
	std::vector <int> values{};
	for (std::size_t i{0}; i < row.size(); ++i){
		if (values.size() == 0){
			values.push_back(1);
		}
		else if (row[i] == row[i-1]){
			values[values.size() - 1]++;
		}
		else {
			values.push_back(1);
		}
	}
	//std::cout << "values ";for (auto i:values){std::cout << i << " ";}std::cout << std::endl;
	
	std::vector<int> coords{};
	if (values.size() < 5){return coords;}
	for (std::size_t i{0}; i < values.size() - 5 + 1; ++i){
		std::vector<int> subvec(values.begin() + i, values.begin() + i + 5);
		//for (auto i:subvec){std::cout << i << " ";}std::cout << std::endl;
		if (contains_ratio(subvec)){
			int x{0};
			for (std::size_t j{0}; j < i; ++j){
				x += values[j];
			}
			coords.push_back(x);
		}
	}
	return coords;
}	
	

std::vector<std::vector<int>> get_finder_coords(std::vector<std::vector<bool>> &symbol){
	std::vector<std::vector<int>> coords(3, std::vector<int>(2,0));
	for (std::size_t row{0}; row < symbol.size(); ++row){
		std::vector<int> c = get_finder_pattern_coords(symbol[row]);
		for (auto i : c){std::cout << i << " ";}std::cout <<std::endl;
	}

	return coords;
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
		data += add_preceding_zeros(convert_uint(subvec));
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
		

std::string decode_symbol(std::vector<std::vector<bool>> &symbol){
	std::vector<bool> format_info = get_format_info(symbol);

	//std::cout << "Format Info: "; print(format_info); std::cout << std::endl;
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
	//print_codewords(error_correction_codewords);
	std::vector<bool> mode_indicator(data_codewords.begin(), data_codewords.begin() + 4);
	std::vector<bool> character_count(data_codewords.begin() + 4, data_codewords.begin() + 14);
	unsigned int character_count_indicator = convert_uint(character_count);
	//std::cout << character_count_indicator << std::endl;	
	std::vector<bool> input_data(data_codewords.begin() + 14, data_codewords.end());

	return get_input_data(input_data, character_count_indicator);
}


void error_correct(std::vector<bool> &codewords){
	// Error correction doe per block: (c,k,r)^a = (26,16,4)^b
}

void read_from_file(std::string filename, std::vector<std::vector<bool>> &symbol){
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
