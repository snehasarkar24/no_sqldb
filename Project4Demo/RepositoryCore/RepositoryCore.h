#pragma once

////////////////////////////////////////////////////////////////////////////
// RepositoryCore.h - Implements NoSql database prototype                 //            
// ver 1.0                                                                //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018             //
// Source: Jim Fawcett                                                    //
////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides one class:
* RepositoryCore -it is building a database prototype

* Required Files:
* ---------------
* DbCore.h
* PayLoad.h


* Maintenance History:
* --------------------
* ver 1.0 : 11 Mar 2018
* - first release

Public Interface :
* ------------------
* the class only contains getters and setters and no additional methods

*/
#include <iostream>
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../CppNoSqlDb/PayLoad/PayLoad.h"

using namespace NoSqlDb;

//RepositoryCare Class
class RepositoryCore
{
public:
	NoSqlDb::DbCore<NoSqlDb::PayLoad> db() const { return db_; }
	NoSqlDb::DbCore<NoSqlDb::PayLoad>& db() { return db_; }
	void db(const NoSqlDb::DbCore<NoSqlDb::PayLoad>& db) { db_ = db; }
	std::string& path() { return path_; }
	std::string name() const { return path_; }
	void path(const std::string& path) { path_ = path; }
	
private:
	NoSqlDb::DbCore<NoSqlDb::PayLoad> db_;
	std::string path_;
};