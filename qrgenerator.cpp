#include <bitset>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <vector>
#define ID_LENGTH 34
#define MODE_INDICATOR_LENGTH 4
#define grid_length 21

std::vector<unsigned int> log_table(256, 0), antilog_table(255,0);

std::string id_generator(unsigned int seed);
std::size_t get_degree(std::vector<unsigned int> polynomial);
std::vector<unsigned int> field_multiply(std::vector<unsigned int> polynomial, unsigned int value);
std::vector<std::vector<bool>> set_square(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::size_t length, bool value);
std::vector<std::vector<bool>> set_rect(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::size_t length, std::size_t height, bool value);
std::vector<std::vector<bool>> symbol_placement_up(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character);
std::vector<std::vector<bool>> symbol_placement_down(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character);
std::vector<std::vector<bool>> symbol_placement_up_special(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character);
std::vector<std::vector<bool>> symbol_placement_down_special(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character);
unsigned int evaluate_symbol(std::vector<std::vector<bool>> grid);
void print_grid(std::vector<std::vector<bool>> grid);

int main() {
	int k;
	//std::cout << id_generator(69) << std::endl;
	std::string input_data{"01234567"};
	const unsigned int input_length = 8;
	std::vector<bool> input_data_vector{};
	for (unsigned int i{0}; i < input_length; i+=3){
/* 8 - 6 = 2 
 * 7 - 6 = 1
 * */	
		if (input_length - i == 1){
			std::bitset<4> three_digits = std::stoul(input_data.substr(i, 3));
			for (std::size_t j{0}; j < three_digits.size(); ++j){
				input_data_vector.push_back(three_digits[three_digits.size() - j - 1]);
			}	
		}
		else if (input_length - i == 2) {
			std::bitset<7> three_digits = std::stoul(input_data.substr(i, 3));
			for (std::size_t j{0}; j < three_digits.size(); ++j){
				input_data_vector.push_back(three_digits[three_digits.size() - j - 1]);
			}	
		}
		else {
			std::bitset<10> three_digits = std::stoul(input_data.substr(i, 3));
			for (std::size_t j{0}; j < three_digits.size(); ++j){
				input_data_vector.push_back(three_digits[three_digits.size() - j - 1]);
			}	
		}
	}
	/*
	k{0};
	for (auto i : input_data_vector){
		if (k++ % 10== 0){std::cout << " ";}
		std::cout << i;
	}
	std::cout << std::endl;
	*/
	// Convert character count indicator to binary (10 bits for version 1-H):
	std::bitset<10> character_count_indicator {input_length};
	for (std::size_t i{0}; i < character_count_indicator.size(); ++i){
		input_data_vector.insert(input_data_vector.begin(), character_count_indicator[i]);
	}
	/*
	k = 0;
	for (auto i : input_data_vector){
		if (k++ % 10== 0){std::cout << " ";}
		std::cout << i;
	}
	std::cout << std::endl;
	*/
	// Numeric data type
	std::vector<bool> mode_indicator{0,0,0,1};
	input_data_vector.insert(input_data_vector.begin(), mode_indicator.begin(), mode_indicator.end());

	// terminator
	std::vector<bool> terminator{0,0,0,0};
	input_data_vector.insert(input_data_vector.end(), terminator.begin(), terminator.end());
	/*
	k = 0;
	for (auto i : input_data_vector){

		if ((k == 4 || k == input_data_vector.size() - 4) || (k > 4 && k < input_data_vector.size() - 4 && (k-4) % 10== 0)){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;
	*/
	// padding bits
	for (std::size_t i{0}; i < input_data_vector.size() % 8; ++i){
		input_data_vector.push_back(0);
	}
	/*
	k = 0;
	for (auto i : input_data_vector){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;
	*/
	//std::cout << "Length: " << input_data_vector.size() << std::endl;
	//std::cout << "Number of data codewords: " << input_data_vector.size() / 8 << std::endl;

	// padding bits for codewords for V1 QR code with M-level error correction (table 8)
	std::vector<bool> pad_codeword1 = {1,1,1,0,1,1,0,0};
	std::vector<bool> pad_codeword2 = {0,0,0,1,0,0,0,1};
	std::size_t code_words_to_add =16 - (input_data_vector.size() / 8); 
	for (std::size_t i{0}; i < code_words_to_add; ++i){
		if (i % 2 == 0){
			input_data_vector.insert(input_data_vector.end(), pad_codeword1.begin(), pad_codeword1.end());
		}
		else {
			input_data_vector.insert(input_data_vector.end(), pad_codeword2.begin(), pad_codeword2.end());
		}
	}

	//std::cout << "Bit Length: " << input_data_vector.size() << std::endl;
	std::cout << "Data Codewords (size = " << input_data_vector.size() / 8 << "): ";


	k = 0;
	for (auto i : input_data_vector){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;

	/* Table 9: V1 QR code M-level Error Correction:
	 * Total Number of Codewords: 26
	 * Number of error correction codewords: 10
	 * value of p = 2
	 * Number of error correction blocks: 1
	 * Error correction doe per block: (c,k,r)^a = (26,16,4)^b
	 * Thus only one block for data codewords
		*/
	
	// Reed-Solomon Error Correction
	//std::vector<bool> primitive = {1,0,0,0,1,1,1,0,1};
	unsigned int primitive = 0b100011101;
	//std::cout << "Primitive: " << primitive << std::endl;
	//unsigned int log_table[255]{}, antilog_table[510]{};

	for (unsigned int i{0}; i < 255; ++i){
		if (i == 0){
			antilog_table[0] = 1;
		}
		else {
			unsigned int antilog_value = antilog_table[i-1] * 2;
			if (antilog_value >= 255){
				antilog_value ^= primitive;
			}
			antilog_table[i] = antilog_value;
		}
	}
	//std::cout << "Antilog table: ";for (unsigned int i{0}; i <255;++i){std::cout << antilog_table[i] << " ";}std::cout << std::endl;
	

	for (unsigned int i{0}; i < 256; ++i){
		if (i == 0){
			log_table[0] = -1;
		}
		else {
			log_table[antilog_table[i]] = i;
		}
			
	}

	//std::cout << "Log table: ";for (auto i: log_table){std::cout << i << " ";}std::cout << std::endl;
	
	// print out important values in log table
	/*
	std::cout << log_table[1] << std::endl;
	std::cout << log_table[2] << std::endl;
	std::cout << log_table[4] << std::endl;
	std::cout << log_table[8] << std::endl;
	std::cout << log_table[16] << std::endl;
	std::cout << log_table[32] << std::endl;
	std::cout << log_table[64] << std::endl;
	std::cout << log_table[128] << std::endl;
	std::cout << log_table[29] << std::endl;
	std::cout << log_table[58] << std::endl;
	*/

	// Data polynomial
	std::vector<unsigned int> data_polynomial(16,0);
	for (std::size_t i{0}; i < input_data_vector.size(); i+=8){
		for(std::size_t j{0}; j < 8;++j){
			data_polynomial[i/8] += pow(2,8-1-j) * input_data_vector[i+j];
		}
	}
	for (unsigned int i{0}; i < 10; ++i){
		data_polynomial.push_back(0);
	}
	/*
	std::cout << "Data Polynomial (size: "<< data_polynomial.size() << "): ";
	for (auto i : data_polynomial){
		std::cout << i << " ";
	}
	std::cout << std::endl;
	*/
	// Generator Polynomial
	std::vector<unsigned int> generator_polynomial = {1, antilog_table[251], antilog_table[67], antilog_table[46], antilog_table[61], antilog_table[118], antilog_table[70], antilog_table[64], antilog_table[94], antilog_table[32], antilog_table[45]};

	std::size_t generator_shift_amount = data_polynomial.size() - generator_polynomial.size();
	for (std::size_t i{0}; i < generator_shift_amount; ++i){
		generator_polynomial.push_back(0);
	}
	
	/*
	std::cout << "Generator Polynomial (size: " << generator_polynomial.size() << "): ";
	for (auto i : generator_polynomial){
		std::cout << i << " ";
	}
	std::cout << std::endl;
	*/
	// remainder polynomial
	std::vector<unsigned int> remainder = data_polynomial;
	for (unsigned int c{0}; c <= generator_shift_amount; ++c){	
		// find scaling factor for generator polynomial
		unsigned int scaling_factor = antilog_table[log_table[remainder[remainder.size() - 1 - get_degree(remainder)]] - log_table[generator_polynomial[generator_polynomial.size() - 1 - get_degree(generator_polynomial)]]];
		if (scaling_factor < 0){
				scaling_factor += 255;
			}
		generator_polynomial = field_multiply(generator_polynomial, scaling_factor);
		//std::cout << scaling_factor << std::endl;
		//std::cout << "after field multiply: "; for (auto val: generator_polynomial){std::cout << val << " ";}std::cout<<std::endl;
		for (std::size_t i{0}; i < remainder.size(); ++i){
			remainder[i] ^= generator_polynomial[i];
			if (remainder[i] > 255){
					remainder[i] ^= primitive;
				}
			//std::cout << remainder[i] << " ";
		}
		//std::cout << std::endl;
		unsigned int unscaling_factor = ((255 - log_table[scaling_factor]) % 255 + 255) % 255;
		generator_polynomial = field_multiply(generator_polynomial, antilog_table[unscaling_factor]);
		//std::cout << "after unmultiply: "; for (auto val: generator_polynomial){std::cout << val << " ";}std::cout<<std::endl;

		generator_polynomial.insert(generator_polynomial.begin(), 0);
		generator_polynomial.pop_back();
	}

	//std::cout << get_degree(remainder) << std::endl;
	//std::cout << "Error correction codewords: ";for (auto i: remainder){std::cout << i << " ";}std::cout << std::endl;

	remainder.erase(remainder.begin(), remainder.begin() + remainder.size() - get_degree(remainder) - 1);
	//std::cout << "Error correction polynomial coefficients: ";for (auto i: remainder){std::cout << i << " ";}std::cout << std::endl;

	
	std::vector<bool> error_correction_codewords{};
	for (std::size_t i{0}; i < remainder.size(); ++i) {
		std::bitset<8> val = remainder[i];
		for (std::size_t j{0}; j < 8; ++j){
			error_correction_codewords.push_back(val[j]);
		}
	}


	std::cout << "Error Correction Codewords (size = " << remainder.size()<<"): ";
	k = 0;
	for (auto i : error_correction_codewords){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;
	
	// final message codeword sequence
	std::vector<bool> codeword_message = input_data_vector;
	codeword_message.insert(codeword_message.begin(), error_correction_codewords.begin(), error_correction_codewords.end());
	std::cout << "Codeword Message (size = " << codeword_message.size()/8<<"): ";
	k = 0;
	for (auto i : codeword_message){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;
	
	std::vector<std::vector<bool>> grid(21, std::vector<bool> (21, 0));
	// top left finder pattern
	grid = set_square(grid, 0,0,7,1);
	grid = set_square(grid, 1,1,5,0);
	grid = set_square(grid, 2,2,3,1);

	// top right finder pattern
	grid = set_square(grid, 14,0,7,1);
	grid = set_square(grid, 15,1,5,0);
	grid = set_square(grid, 16,2,3,1);

	// bottom left finder pattern
	grid = set_square(grid, 0,14,7,1);
	grid = set_square(grid, 1,15,5,0);
	grid = set_square(grid, 2,16,3,1);


	// timing pattern horiz
	for (std::size_t i{8}; i < 8+5; ++i){
		grid[6][i] = (i+1) % 2;
	}
	// timing pattern vert 
	for (std::size_t i{8}; i < 8+5; ++i){
		grid[i][6] = (i+1) % 2;
	}


	// add codeword message
	std::size_t x{21-2}, y{21-4};
	std::size_t codeword_counter{0};
	for (;x >= 13; x-=2){
		for (;y >= 21 - 3 * 4;y-=4){
			std::vector<bool> character_symbol(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
			codeword_counter++;
			//for (auto i : character_symbol) {std::cout <<  i;}std::cout<<std::endl;
			//std::cout << "x=" << x << ",y=" << y << std::endl;
			grid = symbol_placement_up(grid, x, y, character_symbol);
		}
		x -= 2;
		y = 21 - 2 * 4 -1;
		//std::cout << "x=" << x << ",y=" << y << std::endl;
		for (;y <= 21; y+=4){
			std::vector<bool> character_symbol(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
			codeword_counter++;
			//for (auto i : character_symbol) {std::cout <<  i;}std::cout<<std::endl;
			//std::cout << "x=" << x << ",y=" << y << std::endl;
			grid = symbol_placement_down(grid, x, y, character_symbol);
		}
		y = 21 - 4;
	}


	for (;y >= 21 - 3 * 4;y-=4){
		std::vector<bool> character_symbol(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
		codeword_counter++;
		//for (auto i : character_symbol) {std::cout <<  i;}std::cout<<std::endl;
		//std::cout << "x=" << x << ",y=" << y << std::endl;
		grid = symbol_placement_up(grid, x, y, character_symbol);
	}
	
	// up special
	std::vector<bool> character_symbol_special1(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_special1) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_up_special(grid, x, y, character_symbol_special1);
	
	y = 0;
	std::vector<bool> character_symbol_top1(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_top1) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_up(grid, x, y, character_symbol_top1);
	
	// down special
	x -= 2;	
	y = 3;
	std::vector<bool> character_symbol_top2(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_top2) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_down(grid, x, y, character_symbol_top2);

	
	y = 6 +2;
	std::vector<bool> character_symbol_special2(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_special2) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_down_special(grid, x, y, character_symbol_special2);
	

	// printing down long col
	y += 4;	
	for (;y < 21;y+=4){
		std::vector<bool> character_symbol(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
		codeword_counter++;
		//for (auto i : character_symbol) {std::cout <<  i;}std::cout<<std::endl;
		//std::cout << "x=" << x << ",y=" << y << std::endl;
		grid = symbol_placement_down(grid, x, y, character_symbol);
	}

	// fill in data that is between two finder patterns
	x -= 2;
	y = 9;
	std::vector<bool> character_symbol_horiz1(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_horiz1) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_up(grid, x, y, character_symbol_horiz1);
	
	x -= 3;
	y = 12;
	std::vector<bool> character_symbol_horiz2(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_horiz2) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_down(grid, x, y, character_symbol_horiz2);

	x -= 2;
	y = 9;
	std::vector<bool> character_symbol_horiz3(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_horiz3) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_up(grid, x, y, character_symbol_horiz3);
	
	x -= 2;
	y = 12;
	std::vector<bool> character_symbol_horiz4(codeword_message.begin() + codeword_counter * 8, codeword_message.begin() + (codeword_counter + 1) * 8);
	codeword_counter++;
	//for (auto i : character_symbol_horiz4) {std::cout <<  i;}std::cout<<std::endl;
	//std::cout << "x=" << x << ",y=" << y << std::endl;
	grid = symbol_placement_down(grid, x, y, character_symbol_horiz4);

	//print_grid(grid);
	
	
	std::vector<std::vector<std::vector<bool>>> mask_patterns(8, std::vector<std::vector<bool>>(21, std::vector<bool> (21, 0)));
	for (std::size_t i{}; i < 21; ++i){
		for (std::size_t j{}; j < 21; ++j){
			if ((i < 9 && j < 9) || (i > 12 && j < 9) || (i < 9 && j > 12) || i == 6 || j == 6){
				continue;
			}
			// 000
			if ((i+j)%2 == 0){
				mask_patterns[0][i][j] = 1;
			}
			// 001
			if (i % 2 == 0){
				mask_patterns[1][i][j] = 1;
			}
			// 010
			if (j % 3 == 0){
				mask_patterns[2][i][j] = 1;
			}
			// 011
			if ((i+j)%3 == 0){
				mask_patterns[3][i][j] = 1;
			}
			// 100
			if ((i / 2 + j / 3) % 2 == 0){
				mask_patterns[4][i][j] = 1;
			}
			// 101
			if ((i * j) % 2 + (i * j) % 3 == 0){
				mask_patterns[5][i][j] = 1;
			}	
			// 110
			if (((i * j) % 2 + (i * j) % 3 ) % 2 == 0){
				mask_patterns[6][i][j] = 1;
			}	
			// 111
			if ((((i + j) % 2 + (i * j) % 3 ) % 2 == 0)){
				mask_patterns[7][i][j] = 1;
			}	
		}
	}	 

	//print_grid(mask_patterns[7]);
	
	
	std::vector<std::vector<bool>> test_grid(21, std::vector<bool> (21, 0));
	// top left finder pattern
	test_grid = set_square(test_grid, 0,0,7,1);
	test_grid = set_square(test_grid, 1,1,5,0);
	test_grid = set_square(test_grid, 2,2,3,1);

	// top right finder pattern
	test_grid = set_square(test_grid, 14,0,7,1);
	test_grid = set_square(test_grid, 15,1,5,0);
	test_grid = set_square(test_grid, 16,2,3,1);

	// bottom left finder pattern
	test_grid = set_square(test_grid, 0,14,7,1);
	test_grid = set_square(test_grid, 1,15,5,0);
	test_grid = set_square(test_grid, 2,16,3,1);

	test_grid = set_rect(test_grid, 9, 0, 3, 21, 1);
	test_grid = set_rect(test_grid, 0, 9, 6, 4, 1);
	test_grid = set_rect(test_grid, 14, 9, 7, 1, 1);
	test_grid = set_rect(test_grid, 14, 11, 7, 3, 1);
	test_grid = set_rect(test_grid, 14, 15, 7, 1, 1);


	// timing pattern horiz
	for (std::size_t i{8}; i < 8+5; ++i){
		test_grid[6][i] = (i+1) % 2;
	}
	// timing pattern vert 
	for (std::size_t i{8}; i < 8+5; ++i){
		test_grid[i][6] = (i+1) % 2;
	}
	print_grid(test_grid);
	
	std::vector<std::vector<std::vector<bool>>> mask_pattern_results(8, std::vector<std::vector<bool>>(21, std::vector<bool> (21, 1)));
	
	for (std::size_t ind{}; ind < 8; ++ind){
		for (std::size_t i{}; i < 21; ++i){
			for (std::size_t j{}; j < 21; ++j){
				mask_pattern_results[ind][i][j] = (mask_pattern_results[ind][i][j] && test_grid[i][j]); // CHANGE test_grid BACK TO grid for REAL USE
				if (mask_patterns[ind][i][j]){
					mask_pattern_results[ind][i][j] = !mask_pattern_results[ind][i][j];
				}
			}
		}
	}

	//print_grid(mask_pattern_results[7]);
	
	std::cout << evaluate_symbol(test_grid);


	return 0;

}





void print_grid(std::vector<std::vector<bool>> grid){
	for (std::size_t i{}; i < 21; ++i){
		for (std::size_t j{}; j < 21; ++j){
			std::cout << grid[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

std::vector<std::vector<bool>> set_square(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::size_t length, bool value){
	for (std::size_t i{x}; i < x + length; ++i){
		for (std::size_t j{y}; j < y + length; ++j){
			grid[i][j] = value;
	      }
	}
	return grid;
}
std::vector<std::vector<bool>> set_rect(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::size_t length, std::size_t height, bool value){
	for (std::size_t i{x}; i < x + length; ++i){
		for (std::size_t j{y}; j < y + height; ++j){
			grid[i][j] = value;
	      }
	}
	return grid;
}

unsigned int evaluate_symbol(std::vector<std::vector<bool>> grid){
	unsigned int points{0};
	for (int row{0}; row < 21; ++row){
		for (int col{0}; col < 21; ++col){
			unsigned int total_adjacent{0};
			for (int i{-1}; i <= 1; ++i){
				for (int j{-1}; j <= 1; ++j){
					if ((i == 0 && j == 0) || (i + row < 0 || i + row > 20) || (j + col < 0 || j + col > 20)){continue;}
					if (grid[i + row][j+col] == grid[row][col]){
						total_adjacent++;
					}
				}
			}
			points += 3;
			if (total_adjacent > 5){
				points += total_adjacent - 5;
			}
		}
	}
	
	std::vector<std::vector<bool>> seen(21, std::vector<bool>(21,0));
	for (int length{20}, height{20}; length > 1 && height > 1; --length, --height){
		//std::cout << length << " " << height << std::endl;
		for (std::size_t row{0}; row + height < 21; ++row){
			for (std::size_t col{0}; col + length < 21; ++col){
				
				//std::cout << row << "," << col << std::endl;
				bool square_exists = true;
				bool value = grid[row][col];
				for (std::size_t y{row}; y < row + height && square_exists; ++y){
					for (std::size_t x{col}; x < col + length; ++x){
						if (grid[y][x] != value || seen[y][x]){
							square_exists = false;
							break;
						}
					}
				}
				if (square_exists){
					points += 3 * length * height;
					for (std::size_t y{row}; y < row + height; ++y){
						for (std::size_t x{col}; x < col + length; ++x){
							seen[y][x] = true;
						}
					}

				}
							
			}
		}
		//std::cout << std::endl;
	}
	
	print_grid(seen);
	return points;
}

std::vector<std::vector<bool>> symbol_placement_up(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character){
	std::size_t counter{7};
	for (std::size_t row{y}; row < y + 4; ++row){
		for (std::size_t col{x}; col < x + 2; ++col){
			//std::cout << "(" << row << "," << col << ")" <<std::endl;
			grid[row][col] = character[counter];
			--counter;
	      }
	}
	return grid;
}

std::vector<std::vector<bool>> symbol_placement_down(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character){
	std::size_t counter{7};
	for (std::size_t row{y}; row >= y - 3; --row){
		for (std::size_t col{x}; col < x + 2; ++col){
			//std::cout << "(" << row << "," << col << ")" <<std::endl;
			grid[row][col] = character[counter];
			--counter;
	      }
	if (row == 0){break;}
	}
	return grid;
}

std::vector<std::vector<bool>> symbol_placement_up_special(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character){
	std::size_t counter{7};
	for (std::size_t row{y-1}; row < y + 4; ++row){
		if (row == 6) {continue;}
		for (std::size_t col{x}; col < x + 2; ++col){
			//std::cout << "(" << row << "," << col << ")" <<std::endl;
			grid[row][col] = character[counter];
			--counter;
	      }
	}
	return grid;
}

std::vector<std::vector<bool>> symbol_placement_down_special(std::vector<std::vector<bool>> grid, std::size_t x, std::size_t y, std::vector<bool> character){
	std::size_t counter{7};
	for (std::size_t row{y}; row > y - 5; --row){
		if (row == 6) {continue;}
		for (std::size_t col{x}; col < x + 2; ++col){
			//std::cout << "(" << row << "," << col << ")" <<std::endl;
			grid[row][col] = character[counter];
			--counter;
	      }
	}
	return grid;
}

// do not use until main function has defined log and antilog tables;
std::vector<unsigned int> field_multiply(std::vector<unsigned int> polynomial, unsigned int value){
	if (value == 0){
		return std::vector<unsigned int>(polynomial.size(), 0);
	}
	for (std::size_t i{0}; i < polynomial.size(); ++i){
		if (polynomial[i] == 0) {
			polynomial[i] = 0;
		}
		else{
			unsigned int log_sum = (log_table[value] + log_table[polynomial[i]]) % 255;
			polynomial[i] = antilog_table[log_sum];
		}
	}
	return polynomial;
}


std::size_t get_degree(std::vector<unsigned int> polynomial){
	for (std::size_t i{0}; i < polynomial.size(); ++i){
		if (polynomial[i] != 0){
			return polynomial.size() - i - 1;
		}
	}
	return polynomial.size() - 1;
}


std::string id_generator(unsigned int seed){
	srand(seed);
	std::string ID{};
	for (int i{0}; i < ID_LENGTH; ++i) {
		unsigned int digit{};
		digit = 10 * (((double) rand()) / RAND_MAX);
		ID += std::to_string(digit);
	}
	assert(ID.length() == ID_LENGTH);
	return ID;
}

