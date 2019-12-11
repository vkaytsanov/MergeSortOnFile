#pragma once
struct Heap {
	struct HeapNode {
		uint64_t value;
		uint64_t index;
	};
	HeapNode* arr;
	uint64_t size;
	Heap(HeapNode* = nullptr, uint64_t = 0);
	HeapNode root();
	uint64_t left(uint64_t);
	uint64_t right(uint64_t);
	void heapify(uint64_t);
	void replaceMin(HeapNode);
	void swap(HeapNode, HeapNode);
};

struct MergeSortOnFile : public Heap{
	short int number_of_files;
	uint64_t single_file_length;
	std::vector<FILE*> files;
	MergeSortOnFile(short int, uint64_t);
	FILE* fileOpen(char*, char);
	void mergeFiles();
	void divideFiles();
	void sort();
	void mergeSort(uint64_t*, uint64_t, uint64_t);
	void merge(uint64_t*, uint64_t, uint64_t, uint64_t);

};

/* Heap Functions */
void Heap::swap(HeapNode a, HeapNode b) {
	HeapNode tmp = a;
	a = b;
	b = tmp;
}
Heap::HeapNode Heap::root() {
	return this->arr[1];
}
uint64_t Heap::left(uint64_t pos) {
	return (2 * pos);
}

uint64_t Heap::right(uint64_t pos) {
	return (2 * pos + 1);
}

Heap::Heap(HeapNode* arr, uint64_t size) {
	this->arr = arr;
	this->size = size;
	for (uint32_t i = (size - 1) / 2; i >= 0; i--) {
		heapify(i);
	}
}

void Heap::heapify(uint64_t idx) {
	uint64_t left = Heap::left(idx);
	uint64_t right = Heap::right(idx);
	uint64_t smallest = idx;
	if (left < size && arr[left].value < arr[idx].value) {
		smallest = left;
	}
	if (right < size && arr[right].value < arr[idx].value) {
		smallest = right;
	}
	if (smallest != idx) {
		Heap::swap(arr[idx], arr[smallest]);
		heapify(smallest);
	}
}

void Heap::replaceMin(HeapNode val) {
	arr[1] = val;
	heapify(1);
}

/* MergeSort Functions */

void MergeSortOnFile::merge(uint64_t* arr, const uint64_t start, const uint64_t mid, uint64_t end) {
	uint64_t a = mid - start + 1;
	uint64_t b = end - mid;
	std::vector<uint64_t> vec1;
	vec1.reserve(a);
	std::vector<uint64_t> vec2;
	vec2.reserve(b);
	for (int i = 0; i < a; i++) {
		vec1.push_back(arr[start+i]);
	}
	for (int i = 0; i < b; i++) {
		vec2.push_back(arr[start+a+i+1]);
	}
	uint64_t i = 0;
	uint64_t j = 0;
	uint64_t p = start;
	while (i < a && j < b) {
		if (vec1[i] <= vec2[j]) {
			arr[p] = vec1[i];
			p++;
			i++;
		}
		else {
			arr[p] = vec2[j];
			p++;
			j++;
		}
	}
	for (; i < a; i++) {
		arr[p] = vec1[i];
		p++;
	}
	for (; j < b; j++) {
		arr[p] = vec2[j];
		p++;
	}
}
void MergeSortOnFile::mergeSort(uint64_t* arr, uint64_t start, uint64_t end) {
	if (start < end) {
		uint64_t mid = (end - start) / 2 + start;
		mergeSort(arr, start, mid);
		mergeSort(arr, mid, end);
		merge(arr, start, mid, end);
	}
}

void MergeSortOnFile::divideFiles() {
	for (short int i = 0; i < number_of_files; i++) {
		//make file with uniq i
		char file[7];
		snprintf(file, sizeof(file), "%d.txt", i);
		files.push_back(MergeSortOnFile::fileOpen(file, 'w'));
		//write to it
		
		//close?
	}
	char inpfile[] = "GeneratedNumbers.txt";
	FILE* input = MergeSortOnFile::fileOpen(inpfile, 'r');
	short int k = 0;
	uint64_t dummy;
	uint16_t j = 0;
	uint64_t* arr = new uint64_t[single_file_length];
	
	while (!feof(input)) { // tuka se chupi neshto
		fscanf_s(input, "%llu", &arr[j]);
		
		j++;
		if (j == single_file_length) {
			j = 0;
			MergeSortOnFile::mergeSort(arr, 0, single_file_length);
			for (; j < single_file_length; j++) {
				fprintf_s(files[k], "%llu", arr[j]);
			}
			k++;
			j = 0;
		}
		if (k == number_of_files) {
			break;
		}
		
	}
	
	for (short int i = 0; i < number_of_files; i++) {
		fclose(files[i]);
	}
	fclose(input);
}
void MergeSortOnFile::mergeFiles() {
	for (short int i = 0; i < number_of_files; i++) {
		char file[7];
		snprintf(file, sizeof(file), "%d.txt", i);
		files[i] = MergeSortOnFile::fileOpen(file, 'r');
	}
	char file[] = "GeneratedNumbers.txt";
	FILE* output = MergeSortOnFile::fileOpen(file, 'w');
	HeapNode* arr = new HeapNode[number_of_files];
	for (short int i = 0; i < number_of_files; i++) {
		fscanf_s(files[i], "%llu", &arr[i]);
		arr[i].index = i;
	}
	Heap pyramid(arr, number_of_files);
	while (1) {
		bool empty = true;
		for (int i = 0; i < number_of_files; i++) {
			if (!feof(files[i])) {
				empty = false;
				break;
			}
		}
		if (empty) break;
		HeapNode next = pyramid.root();
		fprintf_s(output, "%llu\n", next);
		fscanf_s(files[next.index], "%llu", next.value);
		pyramid.replaceMin(next);
	}
	for (short int i = 0; i < number_of_files; i++) {
		fclose(files[i]);
	}
	fclose(output);

}

void MergeSortOnFile::sort() {
	MergeSortOnFile::divideFiles();
	MergeSortOnFile::mergeFiles();
}
MergeSortOnFile::MergeSortOnFile(short int numFiles, uint64_t parts) {
	this->number_of_files = numFiles ;
	this->single_file_length = parts/ numFiles;
}

FILE* MergeSortOnFile::fileOpen(char* filename, char io) {
	FILE* file;
	errno_t err;
	err = fopen_s(&file, filename, &io);
	return file;
}