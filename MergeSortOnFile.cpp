#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <cmath>
#include "MergeSortOnFile.h"

#define binaryfile 0
#if binaryfile
template<typename T = uint64_t>
bool operator <(const T & a, const T & b) {
	for (int i = 0; i < 64; i++) {
		if (a[i] ^ b[i]) return b[i];
	}
	return false;
}
#endif
namespace Generate {
	void numbers(uint64_t n = 1000) {
		FILE* readFile;
		errno_t err;
		err = fopen_s(&readFile, "GeneratedNumbers.txt", "w");
		std::random_device rd;
		std::mt19937_64 e2(rd());
		std::uniform_int_distribution<uint64_t> res(0, 0xFFFFFFFFFFFFFFFF-1);
		//int n = static_cast<int>(number);
		for (int i = 0; i < n; i++) {

			//uint64_t res = (num30 << 34) + (num30 << 4) + t4;
			fprintf_s(readFile, "%I64u\n", res(e2)); // problems with I64d

		}
		fclose(readFile);

	}
} // GeneratedNumbers.txt
namespace Check {
	bool sorted() {
		FILE* readFile;
		errno_t err;
		err = fopen_s(&readFile, "Sorted.txt", "r");
		uint64_t first;
		uint64_t next;
		fscanf_s(readFile, "%I64u", &first);
		while (fscanf_s(readFile, "%I64u", &next) == 1) {
			if (next <= first) {
				printf("%I64u , %I64u\n", next, first);
				fclose(readFile);
				return false;
			}
			first = next;
		}
		fclose(readFile);
		return true;
	}
} // Sorted.txt
uint64_t fileDivideNumber;
uint64_t fileSize = 0;

int main(int argc, char** argv) {
	if (argc > 1) {
		if (argv[1] == "generate") {
			if (argc > 2) {
				uint64_t n = atoi(argv[2]);
				Generate::numbers(n);
			}
			else {
				Generate::numbers();
			}

		}
		if (argv[1] == "sort") {
			auto start = std::chrono::high_resolution_clock::now();
			FILE* readFile;
			errno_t err;
			err = fopen_s(&readFile, "GeneratedNumbers.txt", "r");
			int64_t d;

			while (!feof(readFile)) {
				fscanf_s(readFile, "%I64u", &d); // problems with I64d
				//printf("%I64u\n", d);
				fileSize++;
			}
			fileSize--;
			fileSize = fileSize / fileDivideNumber;
			fclose(readFile);
			MergeSortOnFile m = MergeSortOnFile(fileDivideNumber, fileSize);
			m.sort();
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
		}
		if (argv[1] == "check") {
			std::cout << Check::sorted();
		}
		return 0;
	}
	//Generate::numbers(100000);
	auto start = std::chrono::high_resolution_clock::now();
	FILE* readFile;
	errno_t err;
	err = fopen_s(&readFile, "GeneratedNumbers.txt", "r");
	int64_t d;

	while (!feof(readFile)) {
		fscanf_s(readFile, "%I64u", &d); // problems with I64d
		//printf("%I64u\n", d);
		fileSize++;
	}
	//fileSize--;
	//fileSize = fileSize / fileDivideNumber;
	fclose(readFile);
	fileDivideNumber = fileSize / 2000;
	if (fileDivideNumber == 0) {
		fileDivideNumber++;
		fileSize--;
	}
	else {
		fileSize = 2000;
	}
	MergeSortOnFile m = MergeSortOnFile(fileDivideNumber, fileSize);
	m.sort();
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

	//std::cout << Check::sorted();
	
}