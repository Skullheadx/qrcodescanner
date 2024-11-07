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
	input_data_vector.insert(input_data_vector.begin(), mode_indicator.begin(), mode_indicator.end());

	// terminator
	std::vector<bool> terminator{0,0,0,0};
	input_data_vector.insert(input_data_vector.end(), terminator.begin(), terminator.end());
	
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
	std::cout << "Primitive: " << primitive << std::endl;
	//unsigned int log_table[255]{}, antilog_table[510]{};
	std::vector<unsigned int> log_table, antilog_table(510,0);


	//std::cout << "Log table: ";
	for (unsigned int i{0}; i < 255; ++i){
		if (i == 0){
			log_table.push_back(1);
		}
		else if (log_table[i-1] * 2 > 255){
			log_table.push_back((log_table[i-1] * 2) ^ primitive);
		}
		else {
			log_table.push_back((log_table[i-1] * 2));
		}
		//std::cout << log_table[i] << " ";
	}
	//std::cout << std::endl;

	//std::cout << "Antilog table: ";
	for (unsigned int i{0}; i < 255; ++i){
		antilog_table[log_table[i]] = i;
		antilog_table[510-log_table[i]-1] = i;
	}
	for (unsigned int i{0}; i < 510;++i){
		//std::cout << antilog_table[i] << " ";
	}
	//std::cout << std::endl;

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
	for (auto i : data_polynomial){
		std::cout << i << " ";
	}
	std::cout << std::endl;

	// Generator Polynomial
	std::vector<unsigned int> generator_polynomial = {1, antilog_table[251], antilog_table[67], antilog_table[46], antilog_table[61], antilog_table[118], antilog_table[70], antilog_table[64], antilog_table[94], antilog_table[32], antilog_table[45]};
	for (auto i : generator_polynomial){
		std::cout << i << " ";
	}
	std::cout << std::endl;

	// remainder polynomial
	std::vector<unsigned int> remainder;


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

