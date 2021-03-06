/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.4                                                             //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018          //
// Source: Jim Fawcett                                                 //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Process/Process/Process.h"
#include <chrono>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

//----< return name of every file on path >----------------------------
Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

//----< return name of every subdirectory on path >--------------------
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

namespace MsgPassingCommunication
{
  // These paths, global to MsgPassingCommunication, are needed by 
  // several of the ServerProcs, below.
  // - should make them const and make copies for ServerProc usage
  std::string sendFilePath;
  std::string saveFilePath;
  //----< show message contents >--------------------------------------
  template<typename T>
  void show(const T& t, const std::string& msg)
  {
    std::cout << "\n  " << msg.c_str();
    for (auto item : t)
    {
      std::cout << "\n    " << item.c_str();
    }
  }

  //----< test ServerProc simply echos message back to sender >--------
  std::function<Msg(Msg)> echo = [](Msg msg) {
    Msg reply = msg;
    reply.to(msg.from());
    reply.from(msg.to());
    return reply;
  };

  //----< checkin communication >--------
  std::function<Msg(Msg)> check = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("check");
	  reply.from(msg.to());
	  return reply;
  };

  //----< checkout communication >--------
  std::function<Msg(Msg)> checkout = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("checkout");
	  reply.from(msg.to());
	  return reply;
  };

  //----< view metadata of a file >--------
  std::function<Msg(Msg)> view = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("view");
	  reply.from(msg.to());
	  return reply;
  };

  //----< establishing connection with server >--------
  std::function<Msg(Msg)> con = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("connectMe");
	  reply.from(msg.to());
	  return reply;
  };

  //----< and query >--------
  std::function<Msg(Msg)> search = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("search");
	  reply.from(msg.to());
	  return reply;
  };

  //----< OR query >--------
  std::function<Msg(Msg)> find = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("find");
	  reply.from(msg.to());
	  return reply;
  };

  //----< display metadata >--------
  std::function<Msg(Msg)> metadata = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("metadata");
	  reply.from(msg.to());
	  return reply;
  };

  //----< close file >--------
  std::function<Msg(Msg)> close = [](Msg msg) {
	  Msg reply = msg;
	  reply.to(msg.from());
	  reply.command("close");
	  reply.from(msg.to());
	  return reply;
  };

  //----< getFiles ServerProc returns list of files on path >----------
  std::function<Msg(Msg)> getFiles = [](Msg msg) {
    Msg reply;
    reply.to(msg.from());
    reply.from(msg.to());
    reply.command("getFiles");
    std::string path = msg.value("path");
    if (path != "")
    {
      std::string searchPath = storageRoot;
      if (path != ".")
        searchPath = searchPath + "\\" + path;
      Files files = Server::getFiles(searchPath);
      size_t count = 0;
      for (auto item : files)
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("file" + countStr, item);
      }
    }
    else
    {
      std::cout << "\n  getFiles message did not define a path attribute";
    }
    return reply;
  };

  //----< getDirs ServerProc returns list of directories on path >-----
  std::function<Msg(Msg)> getDirs = [](Msg msg) {
    Msg reply;
    reply.to(msg.from());
    reply.from(msg.to());
    reply.command("getDirs");
    std::string path = msg.value("path");
    if (path != "")
    {
      std::string searchPath = storageRoot;
      if (path != ".")
        searchPath = searchPath + "\\" + path;
      Files dirs = Server::getDirs(searchPath);
      size_t count = 0;
      for (auto item : dirs)
      {
        if (item != ".." && item != ".")
        {
          std::string countStr = Utilities::Converter<size_t>::toString(++count);
          reply.attribute("dir" + countStr, item);
        }
      }
    }
    else
    {
      std::cout << "\n  getDirs message did not define a path attribute";
    }
    return reply;
  };

  //----< sendFile ServerProc sends file to requester >----------------
  /*
  *  - Comm sends bodies of messages with sendingFile attribute >------
  */
  std::function<Msg(Msg)> sendFile = [](Msg msg) {
    Msg reply;
    reply.to(msg.from());
    reply.from(msg.to());
    reply.command("sendFile");
    reply.attribute("sendingFile", msg.value("fileName"));
    reply.attribute("fileName", msg.value("fileName"));
    reply.attribute("verbose", "blah blah");
    std::string path = msg.value("path");
    if (path != "")
    {
      std::string searchPath = storageRoot;
      if (path != "." && path != searchPath)
        searchPath = searchPath + "\\" + path;
      if (!FileSystem::Directory::exists(searchPath))
      {
        std::cout << "\n  file source path does not exist";
        return reply;
      }
      std::string filePath = searchPath + "/" + msg.value("fileName");
      std::string fullSrcPath = FileSystem::Path::getFullFileSpec(filePath);
      std::string fullDstPath = sendFilePath;
      if (!FileSystem::Directory::exists(fullDstPath))
      {
        std::cout << "\n  file destination path does not exist";
        return reply;
      }
      fullDstPath += "/" + msg.value("fileName");
      FileSystem::File::copy(fullSrcPath, fullDstPath);
    }
    else
    {
      std::cout << "\n  getDirs message did not define a path attribute";
    }
    return reply;
  };

  //----< analyze code on current server path >--------------------------
  /*
  *  - Creates process to run CodeAnalyzer on specified path
  *  - Won't return until analysis is done and logfile.txt
  *    is copied to sendFiles directory
  */
  std::function<Msg(Msg)> codeAnalyze = [](Msg msg) {
    Msg reply;
    reply.to(msg.from());
    reply.from(msg.to());
    reply.command("sendFile");
    reply.attribute("sendingFile", "logfile.txt");
    reply.attribute("fileName", "logfile.txt");
    reply.attribute("verbose", "blah blah");
    std::string path = msg.value("path");
    if (path != "")
    {
      std::string searchPath = storageRoot;
      if (path != "." && path != searchPath)
        searchPath = searchPath + "\\" + path;
      if (!FileSystem::Directory::exists(searchPath))
      {
        std::cout << "\n  file source path does not exist";
        return reply;
      }
      // run Analyzer using Process class
      Process p;
      p.title("test application");
      std::string appPath = "CodeAnalyzer.exe";
      p.application(appPath);
      std::string cmdLine = "CodeAnalyzer.exe ";
      cmdLine += searchPath + " ";
      cmdLine += "*.h *.cpp /m /r /f";
      p.commandLine(cmdLine);
      std::cout << "\n  starting process: \"" << appPath << "\"";
      std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
      CBP callback = []() { std::cout << "\n  --- child process exited ---"; };
      p.setCallBackProcessing(callback);
      if (!p.create())
      {
        std::cout << "\n  can't start process";
      }
      p.registerCallback();
      std::string filePath = searchPath + "\\" +  "logfile.txt";
      std::string fullSrcPath = FileSystem::Path::getFullFileSpec(filePath);
      std::string fullDstPath = sendFilePath;
      if (!FileSystem::Directory::exists(fullDstPath))
      {
        std::cout << "\n  file destination path does not exist";
        return reply;
      }
      fullDstPath += std::string("\\") +  "logfile.txt";
      FileSystem::File::copy(fullSrcPath, fullDstPath);
    }
    else
    {
      std::cout << "\n  getDirs message did not define a path attribute";
    }
    return reply;
  };
}


using namespace MsgPassingCommunication;

//main function
int main()
{
  SetConsoleTitleA("Project4S Server Console");
  sendFilePath = "../Storage";
  saveFilePath = "../Storage";
  Server server(serverEndPoint, "ServerPrototype");
  // may decide to remove Context
  MsgPassingCommunication::Context* pCtx = server.getContext();
  pCtx->saveFilePath = saveFilePath;
  pCtx->sendFilePath = sendFilePath;
  server.start();
  Files files = server.getFiles();
  Dirs dirs = server.getDirs();
  std::cout << "\n  Demonstrating Requirement #1";
  std::cout << "\n  " << typeid(std::function<bool()>).name()
	  << ", declared in this function, "
	  << "\n  is only valid for C++11 and later versions.\n"
	  << "-----------------------------------------------------------------------\n";
  server.addMsgProc("echo", echo);
  server.addMsgProc("check", check);
  server.addMsgProc("checkout", checkout);
  server.addMsgProc("view", view);
  server.addMsgProc("con", con);
  server.addMsgProc("search", search);
  server.addMsgProc("find", find);
  server.addMsgProc("metadata", metadata);
  server.addMsgProc("close", close);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("sendFile", sendFile);
  server.addMsgProc("codeAnalyze", codeAnalyze);
  server.addMsgProc("serverQuit", echo);
  server.processMessages();
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  msg.name("msgToSelf");
  std::cout << "\n  press enter to exit\n";
  std::cin.get();
  std::cout << "\n";
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

