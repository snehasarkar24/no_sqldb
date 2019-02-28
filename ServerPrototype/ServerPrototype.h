#pragma once
///////////////////////////////////////////////////////////////////////
// ServerPrototype.h - Console App that processes incoming messages  //
// ver 1.3                                                           //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018        //
// Source: Jim Fawcett                                               //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*  - This is implemented with a message dispatcher (unodered_map<Msg.Id,ServerProc>
*    where ServerProcs are defined for each type of processing required by the server.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp (static library)
*  Process.h, Process.cpp (static library)
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*	Public Interface:
* ----------------------
*   Server(MsgPassingCommunication::EndPoint ep, const std::string& name) - Server constructor sets endpoint
	void start() - starts the server
	void stop() - stops the server
	void addMsgProc(Key key, ServerProc proc) - adds message to process
	bool hasMessageKey(const Key& key) - checks for message key
	void processMessages() - receives messages
	void postMessage(MsgPassingCommunication::Message msg) - post the messages
	MsgPassingCommunication::Message getMessage() - gets the message
	static Dirs getDirs(const SearchPath& path = storageRoot) - returns directories
	static Files getFiles(const SearchPath& path = storageRoot) - returns files
	MsgPassingCommunication::Context* getContext() - returns the context
	std::string setSendFilePath(const std::string& relPath) - sets send file path
	std::string setSaveFilePath(const std::string& relPath) - set save file path
	std::string getSendFilePath() -gets send file path
	std::string getSaveFilePath() - gets save file path
	void checkin(Msg& msg) - performs checkin
	void find(Msg& msg) - performs and query
	void search(Msg& msg) - performs or query
	void checkout(Msg& msg) - performs checkout
	void metadata(Msg& msg) - views the metadata
	void close(Msg& msg) - closes the file
	void initializeDb() - initalizes the database
*
*  Maintenance History:
* ----------------------
*  ver 1.3 : 01 May 2018
*  - updated checkin, checkout, query command functionalities
*  ver 1.2 : 22 Apr 2018
*  - added NoSqlDb to server members
*  - added simple demo of db in Server startup
*  ver 1.1 : 09 Apr 2018
*  - added ServerProcs for
*    - sending files for popup display
*    - executing remote analysis
*  ver 1.0 : 03/27/2018
*  - fourth release
*/
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../CppNoSqlDb/PayLoad/PayLoad.h"
#include <windows.h>
#include <tchar.h>
#include "../CppNoSqlDb/DbCore/DbCore.h"
#include "../Checkin/Checkin.h"
#include "../RepositoryCore/RepositoryCore.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Browse/Browse.h"
#include "../CppNoSqlDb/Query/Query.h"
#include "../CppNoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../CppNoSqlDb/Utilities/TestUtilities/TestUtilities.h"
#include "../Version/Version.h"

namespace Repository
{
	using File = std::string;
	using Files = std::vector<File>;
	using Dir = std::string;
	using Dirs = std::vector<Dir>;
	using SearchPath = std::string;
	using Key = std::string;
	using Msg = MsgPassingCommunication::Message;
	using ServerProc = std::function<Msg(Msg)>;
	using MsgDispatcher = std::unordered_map<Key, ServerProc>;
	const SearchPath storageRoot = "../Storage";  // root for all server file storage
	const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint

																				//Server Class
	class Server
	{
	public:
		Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
		void start();
		void stop();
		void addMsgProc(Key key, ServerProc proc);
		bool hasMessageKey(const Key& key);
		void processMessages();
		void postMessage(MsgPassingCommunication::Message msg);
		MsgPassingCommunication::Message getMessage();
		static Dirs getDirs(const SearchPath& path = storageRoot);
		static Files getFiles(const SearchPath& path = storageRoot);
		MsgPassingCommunication::Context* getContext();
		std::string setSendFilePath(const std::string& relPath);
		std::string setSaveFilePath(const std::string& relPath);
		std::string getSendFilePath();
		std::string getSaveFilePath();
		void checkin(Msg& msg);
		void find(Msg& msg);
		void search(Msg& msg);
		void checkout(Msg& msg);
		void metadata(Msg& msg);
		void close(Msg& msg);
		void initializeDb();
	private:
		MsgPassingCommunication::Comm comm_;
		MsgDispatcher dispatcher_;
		std::thread msgProcThrd_;
		NoSqlDb::DbCore<NoSqlDb::PayLoad> db_;
		RepositoryCore repo_;

	};

	//----< return reference to MsgPassingCommunication context >--------
	inline MsgPassingCommunication::Context* Server::getContext()
	{
		return comm_.getContext();
	}

	//----< initialize server endpoint and give server a name >----------
	inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
		: comm_(ep, name) {
		initializeDb();
	}

	//setSendFilePath
	inline std::string Server::setSendFilePath(const std::string& relPath)
	{
		comm_.setSendFilePath(relPath);
	}

	//setSaveFilePath
	inline std::string Server::setSaveFilePath(const std::string& relPath)
	{
		comm_.setSaveFilePath(relPath);
	}

	//getSendFilePath
	inline std::string Server::getSendFilePath()
	{
		comm_.getSendFilePath();
	}

	//getSaveFilePath
	inline std::string Server::getSaveFilePath()
	{
		comm_.getSaveFilePath();
	}

	//----< start server's instance of Comm >----------------------------
	inline void Server::start()
	{
		comm_.start();
	}


	//----< stop Comm instance >-----------------------------------------
	inline void Server::stop()
	{
		if (msgProcThrd_.joinable())
			msgProcThrd_.join();
		comm_.stop();
	}

	//----< pass message to Comm for sending >---------------------------
	inline void Server::postMessage(MsgPassingCommunication::Message msg)
	{
		comm_.postMessage(msg);
	}

	//----< get message from Comm >--------------------------------------
	inline MsgPassingCommunication::Message Server::getMessage()
	{
		Msg msg = comm_.getMessage();
		return msg;
	}

	//----< add ServerProc callable object to server's dispatcher >------
	inline void Server::addMsgProc(Key key, ServerProc proc)
	{
		dispatcher_[key] = proc;
	}

	//----< does server have specified key? >----------------------------
	inline bool Server::hasMessageKey(const Key& key)
	{
		for (auto item : dispatcher_)
		{
			if (item.first == key)
				return true;
		}
		return false;
	}

	//checkin fucntionality
	void Server::checkin(Msg& msg)
	{
		std::cout << "\n\n  Demonstrating Requirement #2a, 3, 4, 5, 6";
		std::cout << "\n  " << "Checkin from LocalStorage to Storage, upload file, message communication system\n"
			<< "-----------------------------------------------------------------------\n";
		NoSqlDb::DbElement<NoSqlDb::PayLoad> demoElem;
		demoElem.name(msg.value("name"));
		demoElem.descrip(msg.value("description"));
		demoElem.dateTime(DateTime().now());
		NoSqlDb::PayLoad payload;
		payload.value(msg.value("path"));
		demoElem.payLoad(payload);
		demoElem.payLoad().categories() = Utilities::split(msg.value("categories"), ';');
		demoElem.children() = Utilities::split(msg.value("children"), ';');
		Checkin checkin(repo_);
		std::string version = checkin.CheckinFile(msg.value("fileName"), msg.value("path"), demoElem);
		std::string f = msg.value("fileName");
		std::string a = "../Storage/" + f;
		std::string b = "../Storage/" + version;
		std::string a1 = FileSystem::Path::getFullFileSpec(a);
		std::string b1 = FileSystem::Path::getFullFileSpec(b);
		FileSystem::File::copy(a1, b1, false);
		FileSystem::File::remove(a1);
	}

	//OR query
	void Server::find(Msg& msg)
	{
		std::cout << "\n\n  Demonstrating Requirement #2c";
		std::cout << "\n  " << "Browse demonstrating OR query between name, description, categories, children.\n"
			<< "-----------------------------------------------------------------------\n";
		NoSqlDb::Query<NoSqlDb::PayLoad> q1(repo_.db());
		NoSqlDb::Query<NoSqlDb::PayLoad> q2(repo_.db());
		NoSqlDb::Query<NoSqlDb::PayLoad> q3(repo_.db());
		NoSqlDb::Conditions<NoSqlDb::PayLoad> conds0;
		NoSqlDb::Conditions<NoSqlDb::PayLoad> conds1;
		NoSqlDb::Conditions<NoSqlDb::PayLoad> conds2;
		conds0.name(msg.value("name"));
		conds1.description(msg.value("description"));
		std::vector<std::string> vi = Utilities::split(msg.value("categories"), ';');
		std::vector<std::string> c = Utilities::split(msg.value("children"), ';');
		conds2.children(c);
		for (std::string j : vi)
		{
			std::cout << "\nCategory:: " << j;
			auto hasCategory = [&j](DbElement<PayLoad>& elem) {
				return (elem.payLoad()).hasCategory(j);
			};
			q3.select(hasCategory);
		}
		q1.select(conds0);
		q2.select(conds1);
		q2.query_or(q1);
		q3.query_or(q2);
		q3.show();
		std::string v = "";
		for (std::string var : q3.keys())
		{
			v += var + ";";
			std::cout << "\n V::" << v;

		}
		msg.attribute("fileList1", v);
	}

	//checkout
	void Server::checkout(Msg& msg)
	{
		std::cout << "\n\n  Demonstrating Requirement #2b, 3, 4, 5, 6";
		std::cout << "\n  " << "Checkout from Storage to local_folder, download file, message communication system\n"
			<< "-----------------------------------------------------------------------\n";
		NoSqlDb::DbElement<NoSqlDb::PayLoad> elem = repo_.db().operator[](msg.value("fileName"));
		std::vector<std::string> v = elem.children();
		Version ver(repo_);

		for (std::string c : v)
		{
			Version ver(repo_);
			int version = ver.versionOfFile(c);
			Msg rMsg = msg;
			rMsg.attribute("name", c);
			rMsg.attribute("sendingFile", c + "." + std::to_string(version));
			rMsg.attribute("fileName", c + "." + std::to_string(version));
			Msg rReply = dispatcher_[msg.command()](rMsg);
			postMessage(rReply);
			rMsg.show();
			rReply.show();
		}
		msg.attribute("sendingFile", msg.value("fileName"));
		msg.attribute("name", elem.name());
	}

	//And query
	void Server::search(Msg& msg)
	{
		std::cout << "\n\n  Demonstrating Requirement #3c";
		std::cout << "\n  " << "Browse demonstrating AND between name, description, categories, children.\n"
			<< "-----------------------------------------------------------------------\n";
		NoSqlDb::Query<NoSqlDb::PayLoad> q2(repo_.db());
		NoSqlDb::Conditions<NoSqlDb::PayLoad> conds0;
		conds0.name(msg.value("name"));
		conds0.description(msg.value("description"));
		std::vector<std::string> vi = Utilities::split(msg.value("categories"), ';');
		std::vector<std::string> c = Utilities::split(msg.value("children"), ';');
		conds0.children(c);
		for (std::string j : vi)
		{
			std::cout << "\nCategory:: " << j;
			auto hasCategory = [&j](DbElement<PayLoad>& elem) {
				return (elem.payLoad()).hasCategory(j);
			};
			q2.select(hasCategory);

		}
		q2.select(conds0);
		std::string v = "";
		for (std::string var : q2.keys())
		{
			std::cout << "\nKey:: " << var;
			v += var + ";";
		}
		std::cout << "\nFileList:: " << v;
		msg.attribute("fileList", v);
	}

	//metadata
	void Server::metadata(Msg& msg)
	{
		std::cout << "\n\n  " << "displaying metadata of a file.\n"
			<< "-----------------------------------------------------------------------\n";
		std::string name = repo_.db().operator[](msg.value("fileName")).name();
		std::string description = repo_.db().operator[](msg.value("fileName")).descrip();
		std::vector<std::string> categories = repo_.db().operator[](msg.value("fileName")).payLoad().categories();
		std::vector<std::string> c = repo_.db().operator[](msg.value("fileName")).children();
		std::string category = "";
		for (std::string s : categories)
		{
			category += s + ";";
		}
		std::string children = "";
		for (std::string s : c)
		{
			children += s + ";";
		}
		msg.attribute("name", name);
		msg.attribute("description", description);
		msg.attribute("categories", category);
		msg.attribute("children", children);
	}

	//close
	void Server::close(Msg& msg)
	{
		std::cout << "\n\n  " << "Closing a file .\n"
			<< "-----------------------------------------------------------------------\n";
		Checkin checkin(repo_);
		checkin.close(msg.value("fileName"));
	}
	//----< start processing messages on child thread >------------------
	inline void Server::processMessages(){
		auto proc = [&]()
		{if (dispatcher_.size() == 0){
			std::cout << "\n  no server procs to call";
			return;
		}while (true){
			Msg msg = getMessage();
			std::cout << "\n  received message: " << msg.command() << " from " << msg.from().toString();
			 if (msg.command() == "checkin"){
				checkin(msg);
			}if (msg.command() == "checkout"){
				checkout(msg);
			}if (msg.command() == "search"){
				search(msg);
			}if (msg.command() == "metadata"){
				metadata(msg);
			}if (msg.command() == "find"){
				find(msg);
			}if (msg.command() == "close"){
				close(msg);
			}Msg reply;
			reply.to(msg.from());
			reply.from(msg.to());
			if (hasMessageKey(msg.command())) {
				reply = dispatcher_[msg.command()](msg);
				postMessage(reply);
				msg.show();
				reply.show();
			}
		}
		
		}; std::thread t(proc);
		std::cout << "\n  starting server thread to process messages";
		msgProcThrd_ = std::move(t);
	}
	inline void Server::initializeDb()
	{
		NoSqlDb::DbElement<NoSqlDb::PayLoad> elem;
		elem.name("SuperNode_Proj4");
		elem.descrip("Intended to hold references to Project #4 files.  Left as an exercise.");
		NoSqlDb::PayLoad pl;
		pl.value("demonstration");
		elem.payLoad(pl);
		db_[elem.name()] = elem;
		NoSqlDb::showDb(db_);
		std::cout << "\n";
	}
}