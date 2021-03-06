/////////////////////////////////////////////////////////////////////
// Checkout.cpp - Implements checkout of a file                    //
// ver 1.0                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Checkout.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Version/Version.h"
#include "../CppNoSqlDb/Utilities/TestUtilities/TestUtilities.h"
#include "../Checkin/Checkin.h"

using namespace NoSqlDb;

//Function for checkout file
void Checkout::checkoutFile(std::string filename, std::string path)
{
	std::vector<std::string> vec = Utilities::split(filename, ':');
	std::string n = vec[0];
	std::string f = vec[2];
	Version v(repository_);
	if (!FileSystem::Directory::exists(path + "\\" + n))
	{
		FileSystem::Directory::create(path + "\\" + n);
	}
	int version = v.versionOfFile(filename);
	FileSystem::File::copy(repository_.path() + "\\" + n + "\\" + f + "." + std::to_string(version), path + "\\" + n + "\\" + f, false);
}

//----< demo first part of requirement #3 >----------------------------
bool testR3a()
{
	Utilities::title("Demonstrating Requirement #3a - demonstrating checkout on a file from server folder to client folder");
	RepositoryCore r;
	r.path("..\\server_folder");
	//Performing Checkin for file -P1::T4.txt 
	std::cout << "\n Checkin file -C1::T4.txt\n";
	Checkin c1(r);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem1;
	demoElem1.name("rachna");
	demoElem1.descrip("TA for CSE874");
	demoElem1.dateTime(DateTime().now());
	NoSqlDb::PayLoad p;
	p.flag(1);
	p.value("The news is ...");
	demoElem1.payLoad(p);
	c1.CheckinFile("C1::T4.txt", "..\\client_folder", demoElem1);
	std::cout << "\nDatabase Contents\n";
	NoSqlDb::showDb(r.db());
	Checkout c(r);
	//checkout file -P1::T4.txt
	std::cout << "\n\ncheckout demonstration of file -P1::T4.txt from server folder to client folder\n";
	c.checkoutFile("C1::T4.txt","..\\client_folder");
	return true;
}

/*----< test stub >--------------------------------------------------*/
#ifdef TEST_CHECKOUT
//main function of checkout
int main()
{
	Utilities::Title("Testing Checkout");
	TestExecutive ex;
	// define test structures with test function and message
	TestExecutive::TestStr ts3a{ testR3a, "\nSupporting checkout of file" };
	ex.registerTest(ts3a);
	// run tests
	bool result = ex.doTests();
	if (result == true)
		std::cout << "\n all tests passed";
	else
		std::cout << "\n  at least one test failed";
    return 0;
}
#endif

