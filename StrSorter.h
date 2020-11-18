#pragma once

#include <iostream>
#include <vector>

class StrSorter {

public:
	static void Sort(std::string filePath);

private:
	static void PrintVector(std::vector<std::string> vector);
};