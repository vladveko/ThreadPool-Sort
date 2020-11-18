#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "StrSorter.h"
#include "ThreadPool.h"

using namespace std;

void SortGroup(LPVOID params);

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
	int groupSize = 2;
	while (groupSize < length) {

		vector<vector<string>> groups;
		int i = 0;
		while (i < length / 2) {

			vector<string> buffer(groupSize);
			for (int j = 0; j < groupSize / 2; j++) {
				buffer[j] = fLines[i];
				buffer[groupSize / 2 + j] = fLines[length / 2 + i];
				i++;
			}
			groups.push_back(buffer);
		}

		for (int i = 0; i < groups.size(); i++) {
			tPool.EnqueueTask(SortGroup, &groups[i]);
		}
		tPool.WaitAll();

		for (int i = 0; i < groups.size(); i++) {
			copy((groups[i]).begin(), (groups[i]).end(), fLines.begin() + i * groupSize);
		}
		groups.clear();
		groupSize = groupSize * 2;
	}

	StrSorter::PrintVector(fLines);
}

void StrSorter::PrintVector(std::vector<std::string> vector) {
	cout << "\nPrinting vector:" << endl;
	for (int i = 0; i < vector.size(); i++) {
		cout << vector[i] << endl;
	}
}

void SortGroup(LPVOID params) {
	vector<string>* pGroup = (vector<string>*)params;
	int size = pGroup->size();
	int mid = size / 2;
	int i = 0, j = mid;
	vector<string> buf;
	while (i < mid && j < size)
	{
		if (pGroup->at(i) < pGroup->at(j)) {
			buf.push_back(pGroup->at(i++));
		} 
		else {
			buf.push_back(pGroup->at(j++));
		}
	}
	
	if (i >= mid) {
		while (j < size) {
			buf.push_back(pGroup->at(j++));
		}
	}
	else {
		while (i < mid) {
			buf.push_back(pGroup->at(i++));
		}
	}
	*pGroup = buf;
}