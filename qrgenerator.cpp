#include <bitset>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <vector>
#define ID_LENGTH 34
#define MODE_INDICATOR_LENGTH 4

std::string id_generator(unsigned int seed);

int main() {
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
	int k{0};
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
	for (std::size_t i{0}; i < mode_indicator.size(); ++i){
		input_data_vector.insert(input_data_vector.begin(), mode_indicator[mode_indicator.size() - i - 1]);
	}

	// terminator
	std::vector<bool> terminator{0,0,0,0};
	for (std::size_t i{0}; i < terminator.size(); ++i){
		input_data_vector.push_back(terminator[i]);
	}
	
	int k = 0;
	for (auto i : input_data_vector){

		if ((k == 4 || k == input_data_vector.size() - 4) || (k > 4 && k < input_data_vector.size() - 4 && (k-4) % 10== 0)){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;

	// padding bits
	for (std::size_t i{0}; i < input_data_vector.size() % 8; ++i){
		input_data_vector.push_back(0);
	}
	
	k = 0;
	for (auto i : input_data_vector){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}
	std::cout << std::endl;

	std::cout << "Length: " << input_data_vector.size() << std::endl;
	std::cout << "Number of data codewords: " << input_data_vector.size() / 8 << std::endl;


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


	k = 0;
	for (auto i : input_data_vector){
		if (k != 0 && k % 8 == 0){std::cout << " ";}
		std::cout << i;
		k++;
	}

	return 0;

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

