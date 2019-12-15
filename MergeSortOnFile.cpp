#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <cmath>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>
#include <cstring>
#include <cassert>
#include "Threads.h"
#include "MergeSortOnFile.h"

#define RAM 10000



namespace Generate {
	void numbers(uint64_t bytes = 64*1000) {
		//std::vector<uint64_t> data(bytes / sizeof(uint64_t));
		//std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });
		FILE* readFile;
		errno_t err;
		err = fopen_s(&readFile, "file.binary", "wb");
		if (!readFile) {
			return;
		}
		std::random_device rd;
		std::mt19937_64 e2(rd());
		std::uniform_int_distribution<uint64_t> res(0, 0xFFFFFFFFFFFFFFFF-1);
		//int n = static_cast<int>(number);
		

		for (uint64_t i = 0; i < bytes; i++) {
			uint64_t random = res(e2);
			fwrite((&random), sizeof(uint64_t), 1, readFile); // problems with I64d
		}
		
		fclose(readFile);

	}
} // GeneratedNumbers.txt
namespace Check {
	bool sorted() {
		FILE* readFile;
		errno_t err;
		err = fopen_s(&readFile, "Sorted.txt", "rb");
		if (!readFile) {
			return false;
		}
		if (fseek(readFile, 0, SEEK_END) != 0) {
			fclose(readFile);
			return false;
		}

		uint64_t first;
		uint64_t next;

		fread(&first, sizeof(uint64_t), 1, readFile);
		while (fread(&next, sizeof(uint64_t), 1, readFile) == 1) {
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
			err = fopen_s(&readFile, "file.binary", "rb");
			if (!readFile) {
				return false;
			}
			if (fseek(readFile, 0, SEEK_END) != 0) {
				fclose(readFile);
				return false;
			}
			const uint64_t bytes = ftell(readFile);
			const uint64_t intCount = bytes / sizeof(uint64_t);
			if (bytes < 0 || intCount * sizeof(uint64_t) != bytes) {
				fclose(readFile);
				return false;
			}
			int64_t d;
			fclose(readFile);
			fileDivideNumber = ceil(intCount / (sizeof(uint64_t) * RAM));
			if (fileDivideNumber == 0) {
				fileDivideNumber++;
				fileSize = intCount;
			}
			else {
				fileSize = sizeof(uint64_t) * RAM;
			}
			MergeSortOnFile m = MergeSortOnFile(fileDivideNumber, fileSize);
			m.sortThread();
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
		}
		if (argv[1] == "check") {
			std::cout << Check::sorted();
		}
		return 0;
	}
	//Generate::numbers(sizeof(uint64_t)*100000);
	auto start = std::chrono::high_resolution_clock::now();
	FILE* readFile;
	errno_t err;
	err = fopen_s(&readFile, "file.binary", "rb");
	if (!readFile) {
		return false;
	}
	if (fseek(readFile, 0, SEEK_END) != 0) {
		fclose(readFile);
		return false;
	}
	const uint64_t bytes = ftell(readFile);
	const uint64_t intCount = bytes / sizeof(uint64_t);
	if (bytes < 0 || intCount * sizeof(uint64_t) != bytes) {
		fclose(readFile);
		return false;
	}
	int64_t d;

	//system("pause");
	//while (fread((&d), sizeof(d), 1, readFile) == 1) {
	//	fileSize++;
	//}
	//std::cout << sizeof(uint64_t);
	//printf("%I64u\n", intCount);
	//system("pause");
	fclose(readFile);
	fileDivideNumber = ceil(intCount / (sizeof(uint64_t)*RAM));
	if (fileDivideNumber == 0) {
		fileDivideNumber++;
		fileSize = intCount;
	}
	else {
		fileSize = sizeof(uint64_t)*RAM;
	}
	MergeSortOnFile m = MergeSortOnFile(fileDivideNumber, fileSize);
	//m.sort();
	m.sortThread();
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

	std::cout << Check::sorted();
	
}