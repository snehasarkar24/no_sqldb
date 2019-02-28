#pragma once

/////////////////////////////////////////////////////////////////////
// Version.h - Implements version updation on files on checkin     //            
// ver 1.0                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides one class:
* Version -it is core for implementing versioning operations on checkin of a file
* The package provides function for returning the :
* - current version of a file in database

* Required Files:
* ---------------
* RepositoryCore.h
* FileSystem.h,FileSystem.cpp

* Maintenance History:
* --------------------
* ver 1.0 : 11 Mar 2018
* - first release

Public Interface :
* ------------------
* int Version::versionOfFile(std::string filename) - returns version of a current  file in database

*/

#include <iostream>
#include "../RepositoryCore/RepositoryCore.h"

using namespace NoSqlDb;

//Class for version updation of file
class Version
{
public:
	Version(RepositoryCore& r) : r_{ r } { r_ = r; }
	int versionOfFile(std::string filename);

private:
	RepositoryCore& r_;
};