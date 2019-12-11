//#pragma once
//namespace Generate {
//	void numbers() {
//		int n = 100000;
//		std::ofstream out;
//		out.open("GeneratedNumbers.txt");
//		std::random_device rd;
//		std::mt19937_64 e2(rd());
//		std::uniform_int_distribution<uint64_t> res(0, 0xFFFFFFFFFFFFFFFF);
//		//int n = static_cast<int>(number);
//		for (int i = 0; i < n; i++) {
//
//			//uint64_t res = (num30 << 34) + (num30 << 4) + t4;
//			out << res(e2) << "\n";
//		}
//		out.close();
//
//	}
//}
//
//namespace Check {
//	bool sorted() {
//		std::ifstream fin;
//		fin.open("Sorted.txt");
//		uint64_t first;
//		uint64_t next;
//		fin >> first;
//		while (!fin.eof()) {
//			fin >> next;
//			if (next <= first) {
//				fin.close();
//				return false;
//			}
//			first = next;
//		}
//		fin.close();
//		return true;
//	}
//}