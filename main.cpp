#include <iostream>
#include "StrSorter.h"

int main()
{
	std::cout << "Sorting starts\n" << std::endl;
	StrSorter::Sort("D:\\Semester 5\\OSaSP\\Lab4\\ThreadPool\\FileToSort.txt");
	std::cout << "Sorting ended\n" << std::endl;
}