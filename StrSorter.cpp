#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "StrSorter.h"
#include "ThreadPool.h"

using namespace std;


void StrSorter::Sort(string filePath) {
	ifstream file(filePath);
	
	vector<string> fLines;
	string str;

	if (file.is_open()) {
		while (getline(file, str))
			fLines.push_back(str);
	}
	file.close();

	StrSorter::PrintVector(fLines);

	int length = fLines.size();
	ThreadPool tPool = ThreadPool();
	for (int groupSize = 2; groupSize < length; groupSize *= 2) {

		vector<vector<string>> groups;
		int i = 0;
		while (i < length / 2) {

			vector<string> buffer(groupSize);
			for (int j = 0; j < groupSize / 2; j++) {
				buffer[j] = fLines[i];
				buffer[j + 1] = fLines[length / 2 + i];
				i++;
			}
			groups.push_back(buffer);
		}

		for (int i = 0; i < groups.size();i++) {
			tPool.EnqueueTask(SortGroup, &groups[i]);
		}
		tPool.WaitAll();

		for (int i = 0;i < groups.size();i++) {
			copy(groups[i].begin, groups[i].end, fLines.begin() + i * groupSize);
		}
		groups.clear();
	}
}

void StrSorter::PrintVector(std::vector<std::string> vector) {
	for (int i = 0; i < vector.size(); i++) {
		cout << vector[i] << endl;
	}
}

void SortGroup(LPVOID params) {

}