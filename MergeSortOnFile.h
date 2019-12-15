#define NUMBERS_PER_THREAD = (10000 / 8);
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


std::mutex m;
std::condition_variable cv;
std::atomic<uint8_t> finishedThreads = 0;
std::atomic<bool> allThreadsFinished = false;

struct HeapNode {
	uint64_t value;
	uint64_t index;
};
struct Heap {
	HeapNode* arr;
	uint64_t size;
	Heap(HeapNode*, uint64_t);
	HeapNode root();
	uint64_t left(uint64_t);
	uint64_t right(uint64_t);
	void heapify(uint64_t);
	void replaceMin(HeapNode);
	void swap(HeapNode*, HeapNode*);
};

struct MergeSortOnFile {
	uint64_t number_of_files;
	uint64_t single_file_length;
	std::vector<FILE*> files;
	FILE* inp;
	std::vector<ThreadPool*> threads;
	MergeSortOnFile(uint64_t, uint64_t);
	FILE* fileOpen(char*, const char*);
	void mergeFiles();
	void divideFiles();
	void sort();
	void insertionSort(std::vector<uint64_t>&, uint64_t, uint64_t);
	void mergeSort(std::vector<uint64_t>&, uint64_t, uint64_t);
	void merge(std::vector<uint64_t>&, uint64_t, uint64_t, uint64_t);
	void mergeSortThread(std::vector<uint64_t>&, uint64_t, uint64_t);
	void divideFilesThread();
	void sortThread();
	void mergeSections(std::vector<uint64_t>&, uint64_t, uint64_t, uint64_t);

};

/* Heap Functions */
void Heap::swap(HeapNode* a, HeapNode* b) {
	HeapNode tmp = *a;
	*a = *b;
	*b = tmp;
}
HeapNode Heap::root() {
	return this->arr[0];
}
uint64_t Heap::left(uint64_t pos) {
	return (2 * pos + 1);
}

uint64_t Heap::right(uint64_t pos) {
	return (2 * pos + 2);
}

Heap::Heap(HeapNode * arr, uint64_t size) {
	this->arr = arr;
	this->size = size;
	for (uint64_t i = (size - 1) / 2; i > 0; i--) {
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
	if (right < size && arr[right].value < arr[smallest].value) {
		smallest = right;
	}
	if (smallest != idx) {
		Heap::swap(&arr[idx], &arr[smallest]);
		heapify(smallest);
	}
}

void Heap::replaceMin(HeapNode val) {
	arr[0] = val;
	heapify(0);
}

/* MergeSort Functions */

void MergeSortOnFile::insertionSort(std::vector<uint64_t>& arr, uint64_t start, uint64_t end) {
	// Merge + Insertion works worse?
	for (int i = start + 1; i < end; i++) {
		uint64_t comparator = arr[i];
		int idxSorted = i-1;
		while (idxSorted >= 0 && arr[idxSorted] > comparator) {
			arr[idxSorted + 1] = arr[idxSorted];
			idxSorted -= 1;
		}
		arr[idxSorted + 1] = comparator;
	}
}
void MergeSortOnFile::merge(std::vector<uint64_t>& arr, uint64_t start, uint64_t mid, uint64_t end) {
	
	//system("pause");
	uint64_t a = mid - start + 1;
	uint64_t b = end - mid;
	//printf("a: %I64u\n", a);
	//printf("b: %I64u\n", b);
	//system("pause");
	std::vector<uint64_t> vec1(a);
	std::vector<uint64_t> vec2(b);

	for (uint64_t i = 0; i < a; i++) {
		vec1[i] = arr[start + i];
		//printf("vec1: %I64u\n", vec1[i]);
		//system("pause");
	}
	for (uint64_t i = 0; i < b; i++) {
		vec2[i] = arr[mid + i + 1];
	}
	uint64_t i = 0;
	uint64_t j = 0;
	uint64_t p = start;
	while (i < a && j < b) {
		if (vec1[i] <= vec2[j]) {
			arr[p] = vec1[i];
			p++;
			i++;
			//printf("Notified: %I64u\n", arr[p-1]);
		}
		else {
			arr[p] = vec2[j];
			p++;
			j++;
			//printf("Notified: %I64u\n", arr[p-1]);
		}
		//printf("%I64u\n", arr[p - 1]);
		//system("pause");
	}
	for (; i < a; i++) {
		arr[p] = vec1[i];
		p++;
		//printf("Notified: %I64u\n", arr[p-1]);
	}
	for (; j < b; j++) {
		arr[p] = vec2[j];
		p++;
		//printf("Notified: %I64u\n", arr[p-1]);
	}
	//m.lock();
	//for (uint64_t o = 0; o < p; o++) {
	//	 printf("Array: %I64u\n", arr[o]);
	//}
	//std::lock_guard<std::mutex> lock(m);
	finishedThreads--;
	//std::cout << "Thread " << std::this_thread::get_id() << "\n";
	//cv.notify_all();
	if (finishedThreads == 0) {
		allThreadsFinished = true;
		cv.notify_one();
		//printf("Notified: %I64u\n", arr[p]);
	}

	//.unlock();
	//printf("Merged S: %I64u, M : %I64u, E: %I64u\n", start, mid, end);
	//system("pause");
}
void MergeSortOnFile::mergeSort(std::vector<uint64_t>& arr, uint64_t start, uint64_t end) {
	/* Recursive */
	if (start < end) {
		uint64_t mid = (end - start) / 2 + start;
		mergeSort(arr, start, mid);
		mergeSort(arr, mid + 1, end);
		merge(arr, start, mid, end);
	}

	/* Iterational */
	//for (uint64_t start_size = start+1; start_size <= end; start_size = 2 * start_size){
	//	for (uint64_t left_start = start; left_start < end; left_start += 2 * start_size){
	//		uint64_t mid = start_size - 1 + left_start;
	//		if (mid > end) {
	//			mid = end;
	//		}

	//		int right_end =left_start + 2 * start_size - 1;
	//		if (right_end > end) {
	//			right_end = end;
	//		}
	//		merge(arr, left_start, mid, right_end);
	//	}
	//}
}


void MergeSortOnFile::divideFiles() {
	for (uint64_t i = 0; i < number_of_files; i++) {
		char file[100];
		snprintf(file, sizeof(file), "%I64u.txt", i);
		files.emplace_back(MergeSortOnFile::fileOpen(file, "wb"));
	}

	char inpfile[] = "file.binary";
	FILE* input = MergeSortOnFile::fileOpen(inpfile, "rb");
	std::vector<uint64_t> arr(single_file_length);
	for (int k = 0; k < number_of_files; k++) {
		uint64_t j = 0;
		
		for (; j < single_file_length; j++) {
			//fscanf_s(input, "%I64u", &arr[j]);
			fread((&arr[j]), sizeof(uint64_t), 1, input);
			
		}

		MergeSortOnFile::mergeSort(arr, 0, j - 1);
		for (uint64_t c = 0; c < j; c++) {
			fwrite(reinterpret_cast<const char*>(&arr[c]), sizeof(uint64_t), 1, files[k]);
			//printf("Written in File:  %I64u\n", arr[c]);
		}
	}

	for (uint64_t i = 0; i < number_of_files; i++) {
		fclose(files[i]);
	}
	fclose(input);
}
void MergeSortOnFile::mergeFiles() {
	for (uint64_t i = 0; i < number_of_files; i++) {
		char file[100];
		snprintf(file, sizeof(file), "%I64u.txt", i);
		files[i] = MergeSortOnFile::fileOpen(file, "rb");
	}
	char file[] = "file.binary";
	FILE* output = MergeSortOnFile::fileOpen(file, "wb");
	HeapNode* arr = new HeapNode[number_of_files];
	//Fill
	for (uint64_t i = 0; i < number_of_files; i++) {
		//fscanf_s(files[i], "%I64u", &arr[i].value);
		fread((&arr[i].value), sizeof(uint64_t), 1, files[i]);
		arr[i].index = i;
		
	}
	Heap pyramid(arr, number_of_files);
	pyramid.heapify(0);
	uint64_t count = number_of_files;
	while (count) {
		HeapNode next = pyramid.root();
		//fprintf_s(output, "%I64u\n", next.value);
		//printf("%I64u\n", next.value);
		fwrite(reinterpret_cast<const char*>(&next.value), sizeof(uint64_t), 1, output); //
		//if (fscanf_s(files[next.index], "%I64u", &next.value) != 1) {
		if(fread(&next.value, sizeof(uint64_t), 1, files[next.index]) != 1){
			count--;
			next.value = UINT64_MAX;
		}
		pyramid.replaceMin(next);
	}
	for (uint64_t i = 0; i < number_of_files; i++) {
		fclose(files[i]);
	}
	fclose(output);
}
void MergeSortOnFile::sort() {
	MergeSortOnFile::divideFiles();
	MergeSortOnFile::mergeFiles();
}

void MergeSortOnFile::mergeSortThread(std::vector<uint64_t>& arr, uint64_t start, uint64_t end) {	
	uint64_t mid = start + (end - start) / 2;
	if (start < end) {
		finishedThreads++;
		MergeSortOnFile::mergeSortThread(arr, start, mid);
		MergeSortOnFile::mergeSortThread(arr, mid + 1, end);
		MergeSortOnFile::merge(arr, start, mid, end); 
		
	}
}

void MergeSortOnFile::divideFilesThread() {
	for (uint64_t i = 0; i < number_of_files; i++) {
		char file[100];
		snprintf(file, sizeof(file), "%I64u.txt", i);
		files.emplace_back(MergeSortOnFile::fileOpen(file, "wb"));
	}
	char inpfile[] = "file.binary";
	FILE* input = MergeSortOnFile::fileOpen(inpfile, "rb");
	std::vector<uint64_t> arr(single_file_length);
	for (int k = 0; k < number_of_files; k++) {
		uint64_t j = 0;

		for (; j < single_file_length; j++) {
			fread((&arr[j]), sizeof(uint64_t), 1, input);
		}

		std::function<void(std::vector<uint64_t>, uint64_t, uint64_t)> f = std::bind(&MergeSortOnFile::mergeSortThread, this, std::ref(arr), std::placeholders::_2, std::placeholders::_3);

		for (uint64_t i = 0; i < threads.size(); i++) {
			
			uint64_t start = i * (j / threads.size());
			uint64_t end = (i + 1) * (j / threads.size()) - 1;
			if (i == threads.size() - 1) {
				end = end + j % threads.size();
			}
			threads[i]->setWork(f, std::ref(arr), start, end);
			//std::cout << "Thread: " << i << " started \n";
		}
		//for (uint8_t i = 0; i < threads.size(); i++) {
		//	if (threads[i]->isReady()) {
		//		i = 0;
		//	}
		//}
		//cv.notify_one();
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [] {return finishedThreads == 0; });
		lk.unlock();
		//printf("Unlocked\n");
		//for (uint64_t i = 0; i < threads.size(); i++) {
		//	uint64_t start = i * (j / threads.size());
		//	uint64_t mid = (i+1) * (j / threads.size()) - 1;
		//	uint64_t end = (i + 2) * (j / threads.size()) - 1;
		//	if (end > j) {
		//		end = j - 1;
		//	}
		//	merge(arr, start, mid, end);
		//}
		//mergeSortThread(arr, 0, j - 1);
		//merge(arr, 0, j / 2, j - 1);
		for (uint64_t c = 0; c < j; c++) {
			fwrite(reinterpret_cast<const char*>(&arr[c]), sizeof(uint64_t), 1, files[k]);
			//printf("Written in File:  %I64u\n", arr[c]);
		}
	}

	for (uint64_t i = 0; i < number_of_files; i++) {
		fclose(files[i]);
	}
	fclose(input);
}

void MergeSortOnFile::sortThread() {

	uint64_t t = std::thread::hardware_concurrency();
	this->threads.reserve(t);
	for (uint64_t i = 0; i < t; i++) {
		threads.emplace_back(new ThreadPool);
	}
	MergeSortOnFile::divideFilesThread();
	MergeSortOnFile::mergeFiles();
}

MergeSortOnFile::MergeSortOnFile(uint64_t numFiles, uint64_t parts) {
	number_of_files = numFiles;
	single_file_length = parts;
}

FILE* MergeSortOnFile::fileOpen(char* filename, const char* io) {
	FILE* file;
	errno_t err;
	err = fopen_s(&file, filename, io);
	return file;
}