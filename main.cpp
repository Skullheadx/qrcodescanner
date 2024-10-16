#include <iostream>
#define ROWS 4
#define COLS 3


void print(int (*image)[ROWS][COLS]);

int main() {
	int image[ROWS][COLS];

	for (int i{0}; i < ROWS; ++i) {
		for (int j{0}; j < COLS; ++j) {
			image[i][j] = 0;
		}
	}
	
	print(&image);
		
	return 0;
}


void print(int (*image)[ROWS][COLS]){ 
	for (int i{0}; i < ROWS; ++i) {
		for (int j{0}; j < COLS; ++j) {
      			std::cout << (*image)[i][j];
		}
		std::cout << std::endl;
	}
}
