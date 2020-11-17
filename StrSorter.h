#pragma once

#include <iostream>
#include <Windows.h>

static class StrSorter {

public:
	void Sort(std::string filePath);

private:
	void PrintVector(std::vector<std::string> vector);
};