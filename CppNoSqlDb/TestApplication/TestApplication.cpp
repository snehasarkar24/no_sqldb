///////////////////////////////////////////////////////////////////////
// TestApplication.cpp - designed to test deployment strategy        //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
* - This is an example of how an application will use the NoSqlDb functionality
* - It needs to:
*   - Copy NoSqlDb folder into solution directory
*     - It doesn't need to add NoSqlDb packages to Application's code.
*       That's taken care of by the library it references, below.
*   - Define it's own PayLoad package
*   - Include the composite NoSqlDb.h header
*   - Include its own PayLoad.h
*   - Make a reference to the Executive.lib
*/
#include "../Executive/NoSqlDb.h"
#include "../PayLoad/PayLoad.h"

using namespace NoSqlDb;

/*
* - This demo code was simply lifted from Executive.cpp
*/
int main()
{
   
}

