#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>

void read_words_from_file(char const *filename, std::vector<std::vector<bool>> &symbol);
void print(std::vector<std::vector<bool>> &symbol);

int main(){
	std::vector<std::vector<bool>> symbol(21, std::vector<bool>(21,0));

	read_words_from_file("output.pbm", symbol);
	print(symbol);

	
	return 0;
}


void print(std::vector<std::vector<bool>> &symbol){
	for (const auto &i : symbol){
		for (auto j : i){
			std::cout << j;
	      }
		std::cout << std::endl;
	}
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
