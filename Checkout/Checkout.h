#pragma once

/////////////////////////////////////////////////////////////////////
// Checkout.h - Implements checkout of a file                      //            
// ver 1.0                                                         //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018      //
// Source: Jim Fawcett                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides one class:
* Ckeckout -it implements checkout on a file
* The package provides function for performing checkout on a file:

* Required Files:
* ---------------
* RepositoryCore.h
* FileSystem.h,FileSystem.cpp
* Checkin.h,Checkin.cpp
* Version.h,Version.cpp

* Maintenance History:
* --------------------
* ver 1.0 : 11 Mar 2018
* - first release

Public Interface :
* ------------------
* void Checkout::checkoutFile(std::string filename, std::string path) - checkout a file from server folder to client folder

*/

#include <stdio.h>
#include <tchar.h>
#include <string>
#include "../RepositoryCore/RepositoryCore.h"


//Checkout Class
class Checkout
{
public:
	Checkout(RepositoryCore& repository) : repository_{ repository } { repository_ = repository; }
	void checkoutFile(std::string filename, std::string path);
private:
	RepositoryCore& repository_;
};







