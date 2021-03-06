/////////////////////////////////////////////////////////////////////
// Checkin.cpp - Implements checkin on a file                      //
// ver 1.1                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////

#include "Checkin.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Version/Version.h"
#include "../CppNoSqlDb/PayLoad/PayLoad.h"
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include <Queue>
#include "../CppNoSqlDb/Utilities/TestUtilities/TestUtilities.h"
#include <stdlib.h>

using namespace NoSqlDb;

//----< reduce the number of characters to type >----------------------
auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};

//Performing open and closed checkin
std::string Checkin::CheckinFile(std::string filename, std::string path, NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem)
{
	Version v(repository_);
	int version = v.versionOfFile(filename);
	//if there is no file in server folder
	if (version == 0)
	{
		
		std::string ver = filename + "." + std::to_string(version + 1);
		repository_.db().addRecord(ver, demoElem);
		std::cout << "\nDB after checkin file" + filename + "\n";
		NoSqlDb::showDb(repository_.db());
		return ver;
	}
	else
	{
		int flag = repository_.db().operator[](filename + "." + std::to_string(version)).payLoad().flag();
		//open checkin
		if (flag == 1)
		{
			
			std::cout << "\nDB after checkin file" + filename + "\n";
			NoSqlDb::showDb(repository_.db());
			std::string ver1 = filename + "." + std::to_string(version);
			return ver1;
		}
		//closed checkin
		else
		{
			
			std::string ver2 = filename + "." + std::to_string(version + 1);
			repository_.db().addRecord(ver2, demoElem);	
			std::cout << "\nDB after checkin file" + filename + "\n";
			NoSqlDb::showDb(repository_.db());
			return ver2;
		}
	}
}

//Closing a file and it's child files
void Checkin::close(std::string filename)
{
	Version v(repository_);
	int version = v.versionOfFile(filename);
	std::vector<std::string> q = repository_.db().operator[](filename).children();
	std::cout << "\n\nPerforming close on a file------------"<<filename<< " and it's child files" << std::endl;
	
	int x = 0;
	for (unsigned int j = 0; j < q.size(); j++)
	{
		std::cout << "\nDependent file of "<<filename<<": "<<q[j];
		std::string file = q[j];
		int v1 = v.versionOfFile(file);
		int status = repository_.db().operator[](file + "." + std::to_string(v1)).payLoad().flag();
		if(status == 1)
			std::cout << "\nStatus of file -" << q[j]<<": OPEN";
		else if(status == 2)
			std::cout << "\nStatus of file -" << q[j]<<": CLOSED";
		else
			std::cout << "\nStatus of file -" << q[j] <<": CLOSED PENDING";

		if (status == 2)
		{
			x++;
		}
		
	}
	if (x == q.size())
	{	
		repository_.db().operator[](filename).payLoad().flag(3);
		std::cout << "\nCheckin status of file - "<<filename <<": CLOSED"<< std::endl;
	}
	else
	{
		repository_.db().operator[](filename).payLoad().flag(2);
		std::cout << "\nCheckin status of file - " << filename << ": CLOSED PENDING" << std::endl;
	}
}

//----< demo first part of requirement #4-open checkin >----------------------------
bool testR4a()
{
	Utilities::title("Demonstrating Requirement #4a - demonstrating open checkin on a file from client folder to server folder\n");
	RepositoryCore r;
	r.path("..\\server_folder");
	std::string filename = "P1::T9.txt";
	std::string filename1 = "P2::T8.txt";
	std::string filename2 = "P3::T7.txt";
	std::string filename3 = "P4::T6.txt";
	//Performing first time checkin
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem;
	demoElem.name("sneha");
	demoElem.descrip("TA for CSE687");
	demoElem.dateTime(DateTime().now());
	NoSqlDb::PayLoad p;
	p.flag(1);
	p.value("The news is ...");
	demoElem.payLoad(p);
	Checkin c(r);
	c.CheckinFile(filename, "..\\client_folder", demoElem);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem1;
	demoElem1.name("neha");
	demoElem1.descrip("TA for CSE767");
	demoElem1.dateTime(DateTime().now());
	p.flag(2);
	p.value("The weather is ...");
	demoElem1.payLoad(p);
	c.CheckinFile(filename1, "..\\client_folder", demoElem1);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem2;
	demoElem2.name("rachna");
	demoElem2.descrip("TA for CSE7");
	demoElem2.dateTime(DateTime().now());
	p.flag(3);
	p.value("The sun is ...");
	demoElem2.payLoad(p);
	c.CheckinFile(filename2, "..\\client_folder", demoElem2);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem3;
	demoElem3.name("priu");
	demoElem3.descrip("TA for CSE009");
	demoElem3.dateTime(DateTime().now());
	p.flag(2);
	p.value("The group is ...");
	demoElem2.payLoad(p);
	demoElem3.addChildKey(filename1);
	demoElem3.addChildKey(filename2);
	c.CheckinFile(filename3, "..\\client_folder", demoElem3);
	c.CheckinFile(filename, "..\\client_folder", demoElem);
	std::cout << "\n\n\n Db content\n";
	NoSqlDb::showDb(r.db());
	return true;
}
//----< demo second part of requirement #4-closed checkin >----------------------------
bool testR4b()
{
	Utilities::title("Demonstrating Requirement #4b - demonstrating closed checkin on a file from client folder to server folder\n");
	RepositoryCore r;
	r.path("..\\server_folder");
	std::string filename = "P5::T5.txt";
	std::string filename1 = "P6::T4.txt";
	std::string filename2 = "P7::T3.txt";
	std::string filename3 = "P8::T2.txt";
	Checkin c(r);
	//Performing first time checkin
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem;
	demoElem.name("sneha");
	demoElem.descrip("TA for CSE687");
	demoElem.dateTime(DateTime().now());
	NoSqlDb::PayLoad p;
	p.flag(1);
	p.value("The news is ...");
	demoElem.payLoad(p);
	c.CheckinFile(filename, "..\\client_folder", demoElem);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem1;
	demoElem1.name("neha");
	demoElem1.descrip("TA for CSE767");
	demoElem1.dateTime(DateTime().now());
	p.value("The weather is ...");
	p.flag(2);
	demoElem1.payLoad(p);
	c.CheckinFile(filename1, "..\\client_folder", demoElem1);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem2;
	demoElem2.name("rachna");
	demoElem2.descrip("TA for CSE7");
	demoElem2.dateTime(DateTime().now());
	p.value("The function is ...");
	p.flag(3);
	demoElem2.payLoad(p);
	c.CheckinFile(filename2, "..\\client_folder", demoElem2);
	NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem3;
	demoElem3.name("priu");
	demoElem3.descrip("TA for CSE009");
	demoElem3.dateTime(DateTime().now());
	p.value("The icecream is ...");
	p.flag(2);
	demoElem3.payLoad(p);
	demoElem3.addChildKey(filename1);
	demoElem3.addChildKey(filename2);
	c.CheckinFile(filename3, "..\\client_folder", demoElem3);
	c.close(filename3);
	c.CheckinFile(filename3, "..\\client_folder", demoElem3);
	std::cout << "\n\n\n Db content\n";
	NoSqlDb::showDb(r.db());
	return true;
}

/*----< test stub >--------------------------------------------------*/
#ifdef TEST_CHECKIN
int main()
{
	Utilities::Title("Testing Checkout");
	putLine();
	TestExecutive ex;
	// define test structures with test function and message
	TestExecutive::TestStr ts4a{ testR4a, "Supporting checkin on file" };
	TestExecutive::TestStr ts4b{ testR4b, "Supporting checkin on file" };
	ex.registerTest(ts4a);
	ex.registerTest(ts4b);
	// run tests
	bool result = ex.doTests();
	if (result == true)
		std::cout << "\n all tests passed";
	else
		std::cout << "\n  at least one test failed";
	putLine(2);
    return 0;
}
#endif

