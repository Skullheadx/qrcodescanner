#include <cstdlib>
#include <iostream>
#include <cassert>


std::string generate_id(unsigned int seed);


int main() {
	std::cout << generate_id(69);
	return 0;

}

std::string generate_id(unsigned int seed){
	srand(seed);
	const unsigned int ID_LENGTH = 34;
	std::string ID{};
	std::cout << ID << std::endl;
	for (int i{0}; i < ID_LENGTH; ++i) {
		unsigned int digit{};
		digit = 10 * (((double) rand()) / RAND_MAX);
		ID += std::to_string(digit);
	}
	assert(ID.length() == ID_LENGTH);
	return ID;
}
