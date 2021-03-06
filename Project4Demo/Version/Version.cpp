/////////////////////////////////////////////////////////////////////
// Version.cpp - Implements version updation on files on checkin   //
// ver 1.0                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////

#include "Version.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Checkin/Checkin.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../CppNoSqlDb/Utilities/TestUtilities/TestUtilities.h"

using namespace NoSqlDb;

//method that returns version of the file
int Version::versionOfFile(std::string filename)
{
	int i = 0;
	while (r_.db().contains(filename + "." + std::to_string(i + 1)))
	{
		i++;
	}
	return i;
}

//----< demo second part of requirement #3-version fetch >----------------------------
bool testR3b()
{
Utilities::title("Demonstrating Requirement #3b - demonstrating version fetch for a file");
RepositoryCore r;
r.path("..\\server_folder");
Version v(r);
std::cout << "\nDatabase Contents\n";
NoSqlDb::showDb(r.db());
std::cout << "\nVersion of file - V5::T5.txt::" << v.versionOfFile("V5::T5.txt");
//Performing Checkin for file -P5::T5.txt 
std::cout << "\n Checkin file -V5::T5.txt\n";
Checkin c1(r);
NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem1;
demoElem1.name("rachna");
demoElem1.descrip("TA for CSE874");
demoElem1.dateTime(DateTime().now());
NoSqlDb::PayLoad p;
p.flag(1);
p.value("The news is ...");
demoElem1.payLoad(p);
c1.CheckinFile("V5::T5.txt", "..\\client_folder", demoElem1);
std::cout << "\nDatabase Contents\n";
NoSqlDb::showDb(r.db());
std::cout << "\nVersion of file - V5::T5.txt::" << v.versionOfFile("V5::T5.txt");
return true;
}

/*----< test stub >--------------------------------------------------*/
#ifdef TEST_V
//main function of checkout
int main()
{
	Utilities::Title("Testing Version");
	TestExecutive ex;
	// define test structures with test function and message
	TestExecutive::TestStr ts3b{ testR3b, "Supporting checkout of file" };
	ex.registerTest(ts3b);
	// run tests
	bool result = ex.doTests();
	if (result == true)
		std::cout << "\n all tests passed";
	else
		std::cout << "\n  at least one test failed";
	return 0;
}
#endif


