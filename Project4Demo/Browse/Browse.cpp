///////////////////////////////////////////////////////////////////////
// Browse.cpp - Implements browsing on files                         //
// ver 1.1                                                           //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018        //
// Source: Jim Fawcett                                               //
///////////////////////////////////////////////////////////////////////

#include <chrono>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../CppNoSqlDb/PayLoad/PayLoad.h"
#include "../Version/Version.h"
#include "Browse.h"
#include "../Checkin/Checkin.h"

using namespace NoSqlDb;

//Function to open a file
void Browse::showFile(std::ostream& out)
{
	for (auto key : keys())
	{
		std::vector<std::string> s = Utilities::split(key, ':');
		std::string n = s[0];
		std::string f = s[2];
		std::string c = "notepad.exe " + r_.path() + "\\" + n + "\\" + f;
		std::cout << "\nFile Path:" << r_.path() + "\\" + n + "\\" + f << std::endl;
		system(c.c_str());
	}
}

//----< demo first part of requirement #5 >----------------------------
bool testR5a(RepositoryCore& r)
{
	Utilities::title("Demonstrating Requirement #5a - demonstrating database creation\n");
	r.path("..\\server_folder");
	std::string filename = "B5::T5.txt";
	std::string filename1 = "B6::T4.txt";
	std::string filename2 = "B7::T3.txt";
	std::string filename3 = "B8::T2.txt";
	//Performing first time checkin
	Checkin c(r);
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
	c.close(filename2);
	c.CheckinFile(filename, "..\\client_folder", demoElem3);
	return true;
}


//----< demo second part of requirement #5 >----------------------------
bool testR5b(RepositoryCore& r)
{
	Utilities::title("Demonstrating Requirement #5b - Metadata Queries");
	std::string filename = "B5::T5.txt";
	std::string filename1 = "B6::T4.txt";
	std::string filename2 = "B7::T3.txt";
	std::string filename3 = "B8::T2.txt";
	std::cout << "\n  creating default query, q1, and displaying:\n";
	Browse q1(r);
	q1.show();  // complete db
	Utilities::putline();
	Keys saveKeys = q1.keys();
	std::cout << "\n  select on child keys " << filename1 <<"\n";
	Keys keys{ filename1 };
	Conditions<PayLoad> conds0;
	conds0.children(keys);
	q1.select(conds0).show();
	Utilities::putline();
	std::cout << "\n  select on name containing \"Sn\" and description containing \"In\" or \"TA\"\n";
	q1.from(saveKeys);
	Conditions<PayLoad> conds1;
	conds1.name("sn");
	conds1.description("(.*In)|(.*TA)");
	q1.select(conds1).show();
	Utilities::putline();
	DateTime dtlb, dtub;       // default time is now
	std::chrono::hours day(24);
	dtlb -= (365 * day);  // one year ago
	DbElement<PayLoad>& elem = r.db().operator[](filename);
	DateTime newDt;
	newDt -= (10 * day);
	elem.dateTime(newDt);
	std::cout << "\n  select on time lowerbound = " << dtlb.time()
		<< " and upperbound = " << dtub.time()
		<< "\n  after changing time to "<<filename << newDt.time() << "\n";
	conds1.lowerBound(dtlb);
	conds1.upperBound(dtub);
	q1.select(conds1).show();
	Utilities::putline();
	return true;
}

//----< demo third part of requirement #5 >----------------------------
bool testR5c(RepositoryCore& r)
{
	Utilities::title("Demonstrating Requirement #5c - Opening file content on demand of resulting compound Queries");
	Utilities::putline();
	Utilities::title("q1 selecting on name = \"Sn\"");
	Conditions<PayLoad> conds1;
	conds1.name("sneha");
	Browse q1(r);
	q1.select(conds1).show();
	Utilities::putline();
	Utilities::title("q2 - selecting on description containing \"TA\"");
	Conditions<PayLoad> conds2;
	conds2.description("TA");
	Query<PayLoad> q2(r.db());
	q2.select(conds2).show();
	Utilities::putline();
	Utilities::title("q2 = q1 or q2");
	q2.query_or(q1).show();
	Utilities::putline();
	Utilities::title("q2 = q2 and q1");
	q2.select(conds1).show();
	std::cout << "\n Opening file content on demand of resulting query" << std::endl;
	q1.showFile();
	Utilities::putline();
	return true;
}

//----< demo fourth part of requirement #5 >----------------------------
bool testR5d(RepositoryCore& r)
{
	Utilities::title("Demonstrating Requirement #5d - PayLoad Queries");
	std::string filename = "B5::T5.txt";
	std::string filename1 = "B6::T4.txt";
	std::string filename2 = "B7::T3.txt";
	std::string filename3 = "B8::T2.txt";
	NoSqlDb::PayLoad pl;
	pl.categories().push_back("firstCategory");
	pl.categories().push_back("secondCategory");
	pl.value() = "Test Payload #1";
	r.db().operator[](filename).payLoad() = pl;
	pl.categories().clear();
	pl.categories().push_back("firstCategory");
	pl.categories().push_back("thirdCategory");
	pl.value() = "Test Payload #2";
	r.db().operator[](filename1).payLoad() = pl;
	pl.categories().push_back("firstCategory");
	pl.value() = "Test Payload #3";
	r.db().operator[](filename2).payLoad() = pl;
	pl.categories().push_back("thirdCategory");
	pl.value() = "Test Payload #3";
	r.db().operator[](filename3).payLoad() = pl;
	std::cout << "\n  db revised for payload tests:\n";
	PayLoad::showDb(r.db());
	Utilities::putline();
	Query<PayLoad> q1(r.db());
	std::string category = "thirdCategory";
	std::cout << "\n  select on payload categories for \"" << category << "\"\n";
	auto hasCategory = [&category](NoSqlDb::DbElement<NoSqlDb::PayLoad>& elem) {
		return (elem.payLoad()).hasCategory(category);
		
	};
	q1.select(hasCategory).show();
	std::string value = "Test Payload #1";
	auto hasValue = [&value](NoSqlDb::DbElement<NoSqlDb::PayLoad>& elem) {
		return (elem.payLoad()).value() == value;
	};
	Utilities::putline();
	std::cout << "\n  select on payload value \"" << value << "\"\n";
	q1.from(r.db().keys());
	q1.select(hasValue).show();
	return true;
}

/*----< test stub >--------------------------------------------------*/
#ifdef TEST_Browse
//main function
int main()
{
	Utilities::Title("Demonstrating Browsing Package");
	RepositoryCore repository;
	repository.path("..\\server_folder");
	testR5a(repository);
	testR5b(repository);
	testR5d(repository);
	testR5c(repository);
	std::cout << "\n\n";
	return 0;
}
#endif 
