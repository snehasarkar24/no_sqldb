#pragma once

/////////////////////////////////////////////////////////////////////
// Browse.h - Implements browsing of files                         //            
// ver 1.1                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides one class:
* Browse -it implements querying on the database
* The package provides function for opening a file based on query:

* Required Files:
* ---------------
* RepositoryCore.h
* DateTime.h
* Query.h
* FileSystem.h,FileSystem.cpp
* Version.h,Version.cpp
* PayLoad.h
* Checkin.h,Checkin.cpp

* Maintenance History:
* --------------------
* ver 1.1 : 01 May 2018
* - second release

Public Interface :
* ------------------
* void Browse::showFile(std::ostream& out = std::cout)- opens a file based on query
*/

#include<iostream>
#include "../CppNoSqlDb/Query/Query.h"
#include "../CppNoSqlDb/DateTime/DateTime.h"
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../RepositoryCore/RepositoryCore.h"

using namespace NoSqlDb;

//Browse Class
class Browse : public NoSqlDb::Query<NoSqlDb::PayLoad>
{
public:
	Browse(RepositoryCore& r) : NoSqlDb::Query<NoSqlDb::PayLoad>(r.db()), r_{ r }
	{
		r_ = r;
	}
	RepositoryCore repo() const { return r_; }
	RepositoryCore& repo() { return r_; }
	void repository(const RepositoryCore& r) { r_ = r; }
	void showFile(std::ostream& out = std::cout);

private:
	RepositoryCore& r_;
};