#pragma once
/////////////////////////////////////////////////////////////////////
// Checkin.h - Implements checkin of a file                      //            
// ver 1.1                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides one class:
* Checkin -it implements checkin on a file
* The package provides function for performing checkout on a file:
* Checkout facilitates open and closed checkin functionalities

* Required Files:
* ---------------
* RepositoryCore.h
* DateTime.h
* FileSystem.h,FileSystem.cpp
* Version.h,Version.cpp
* PayLoad.h
* DbCore.h

* Maintenance History:
* --------------------
* ver 1.1 : 01 May 2018
* - second release

Public Interface :
* ------------------
* std::string Checkin::CheckinFile(std::string filename,std::string path, NoSqlDb::DbElement<NoSqlDb::PayLoad> dbElem) - open and closed checkin functionality from client to server folder
* void Checkin::close(std::string filename) - performs closing of a file to facilitate closed checkin
*/

#include<iostream>
#include "../RepositoryCore/RepositoryCore.h"
#include "../CppNoSqlDb/DateTime/DateTime.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../CppNoSqlDb/PayLoad/PayLoad.h"


//Checkin Class
class Checkin
{
public:
	Checkin(RepositoryCore& repository) : repository_{ repository} { repository_ = repository; }
	std::string CheckinFile(std::string filename,std::string path, NoSqlDb::DbElement<NoSqlDb::PayLoad> dbElem);
	void close(std::string filename);
private:
	RepositoryCore& repository_;
};


