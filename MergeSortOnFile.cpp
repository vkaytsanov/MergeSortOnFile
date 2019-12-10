#include <iostream>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>

namespace Generate {
	void numbers() {
		int n = 100000;
		std::ofstream out;
		out.open("GeneratedNumbers.txt");
		std::random_device rd;
		std::mt19937_64 e2(rd());
		std::uniform_int_distribution<uint64_t> res(0, 0xFFFFFFFFFFFFFFFF);
		//int n = static_cast<int>(number);
		for (int i = 0; i < n; i++) {

			//uint64_t res = (num30 << 34) + (num30 << 4) + t4;
			out << res(e2)  << "\n";
		}
		out.close();
		
	}
}

namespace Check {
	bool sorted() {
		std::ifstream fin;
		fin.open("Sorted.txt");
		uint64_t first;
		uint64_t next;
		fin >> first;
		while (!fin.eof()) {
			fin >> next;
			if (next <= first) {
				fin.close();
				return false;
			}
			first = next;
		}
		fin.close();
		return true;
	}
}
int main(){
	//Generate::numbers();
	std::vector<uint64_t> arr(100001);
	arr.reserve(100000);
	std::ifstream fin;
	fin.open("GeneratedNumbers.txt");
	int i = 0;
	while (!fin.eof()) {
		fin >> arr[i];
		i++;
	}
	fin.close();
	std::sort(arr.begin(), arr.end());
	std::ofstream out;
	out.open("Sorted.txt");
	i = 0;
	while (i < 100000) {
		out << arr[i] << "\n";
		i++;
	}
	out.close();
	
	std::cout << Check::sorted();
}

