#include <iostream>
#define LENGTH 21 


void print(int (*image)[LENGTH][LENGTH]);

int main() {
	int QR[LENGTH][LENGTH];

	for (int i{0}; i < LENGTH; ++i) {
		for (int j{0}; j < LENGTH; ++j) {
			QR[i][j] = 0;
		}
	}
	
	print(&QR);
	

	return 0;
}


void print(int (*image)[LENGTH][LENGTH]){ 
	for (int i{0}; i < LENGTH; ++i) {
		for (int j{0}; j < LENGTH; ++j) {
      			std::cout << (*image)[i][j];
		}
		std::cout << std::endl;
	}
}
