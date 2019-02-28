#pragma once
/////////////////////////////////////////////////////////////////////
// TestExecutive.h - Implements test cases for database            //            
// ver 1.0                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package includes all the header files needed by Test.cpp for checkin,checkout,browsing operations

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
* ver 1.0 : 11 Mar 2018
* - first release

Public Interface :
* ------------------
* The package doesnt include any class functions
*/

#include<iostream>
#include "../RepositoryCore/RepositoryCore.h"
#include "../DateTime/DateTime.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Checkin/Checkin.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Version/Version.h"
#include "../PayLoad/PayLoad.h"
#include "../DbCore/DbCore.h"
#include <Queue>
#include "../Utilities/TestUtilities/TestUtilities.h"
#include "../Checkout/Checkout.h"
#include "../Browse/Browse.h"

using namespace NoSqlDb;