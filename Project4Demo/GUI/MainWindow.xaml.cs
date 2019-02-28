///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 2.1                                                           //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018        //
// Source: Jim Fawcett                                               //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for Project3HelpWPF demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Public Interfaces:
 * -------------------
    private void processMessages() - processes the messages
    private void addClientProc(string key, Action<CsMessage> clientProc) - adds the client to processing of message
    Following are the Dispatcher:
    private void DispatcherLoadGetDirs()
    private void DispatcherLoadGetFiles()
    private void DispatcherLoadSendFile()
    private void DispatcherConnect()
    private void DispatcherSearch()
    private void displayMeta()
    private void DispatcherOR()
    private void DispatcherCheckOut()

    private void loadDispatcher() - loads the dispatcher
    private void Window_Loaded(object sender, RoutedEventArgs e) - loads the window
    private void ConnectButton_Click(object sender, RoutedEventArgs e) - performs connection
    public void SearchButton_Click(object sender, RoutedEventArgs e) - performs and button click
    public void ORButton_Click(object sender, RoutedEventArgs e) - performs or query
    void test1() - automated testing unit

 * 
 * Maintenance History:
 * --------------------
 * ver 2.1 : 01 May 2018
 * - added And,OR query, checkout, connect events
 * ver 2.0 : 22 Apr 2018
 * - added tabbed display
 * - moved remote file view to RemoteNavControl
 * - migrated some methods from MainWindow to RemoteNavControl
 * - added local file view
 * - added NoSqlDb with very small demo as server starts up
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
// - that can be fixed with a load failure event handler
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.IO;
using MsgPassingCommunication;

namespace WpfApp1
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Console.Title = "Project4Demo GUI Console";
        }
        private Stack<string> pathStack_ = new Stack<string>();
        internal Translater translater;
        internal CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();
        internal string saveFilesPath;
        internal string sendFilesPath;
        internal RemoteNavControl rea;
        internal void setRea(RemoteNavControl r)
        {
            rea = r;
        }
        internal LocalNavControl lea;
        internal void setLea(LocalNavControl l)
        {
            lea = l;
        }

        //----< process incoming messages on child thread >----------------
        private void processMessages()
        {
            ThreadStart thrdProc = () =>
            {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    try
                    {
                        string msgId = msg.value("command");
                        Console.Write("\n  client getting message \"{0}\"", msgId);
                        if (dispatcher_.ContainsKey(msgId))
                            dispatcher_[msgId].Invoke(msg);
                    }
                    catch (Exception ex)
                    {
                        Console.Write("\n  {0}", ex.Message);
                        msg.show();
                    }
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }

        //----< add client processing for message with key >---------------
        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        ////----< load getDirs processing into dispatcher dictionary >-------
        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
          {
              //NavLocal.clearDirs();
              NavRemote.clearDirs();
          };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                  {
                      NavRemote.addDir(dir);
                  };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
          {
              NavRemote.insertParent();
          };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }

        //----< load getFiles processing into dispatcher dictionary >------
        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
          {
              NavRemote.clearFiles();
          };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                  {
                      NavRemote.addFile(file);
                  };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }

        //----< load getFiles processing into dispatcher dictionary >------
        private void DispatcherLoadSendFile()
        {
            Action<CsMessage> sendFile = (CsMessage rcvMsg) =>
            {
                Console.Write("\n  processing incoming file");
                string fileName = "";
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("sendingFile"))
                    {
                        fileName = enumer.Current.Value;
                        break;
                    }
                }
                if (fileName.Length > 0)
                {
                    Action<string> act = (string fileNm) => { showFile(fileNm); };
                    Dispatcher.Invoke(act, new object[] { fileName });
                }
            };
            addClientProc("sendFile", sendFile);
        }

        //----< load connection between client and server processing into dispatcher dictionary >------
        private void DispatcherConnect()
        {
            Action<CsMessage> connectMe = (CsMessage rcvMsg) =>
            {

                Action<string> doConnect = (string path) =>
           {
               lstBoxFilesForCheckin6.Text += "\nconnection establised between " + txtMsg3.Text + " and " + path;

           };
                Dispatcher.Invoke(doConnect, new Object[] { rcvMsg.value("path") });

            };
            addClientProc("connectMe", connectMe);
        }

        //----< And search query>------
        private void DispatcherSearch()
        {
            Action<CsMessage> search = (CsMessage rcvMsg) =>
            {

                Action<string> doSearch = (string fileList) =>
                {
                    FileContent.Text += "\nSearch results for AND Query: " + fileList;

                };
                Dispatcher.Invoke(doSearch, new Object[] { rcvMsg.value("fileList") });

            };
            addClientProc("search", search);
        }

        //----< display metadata>------
        private void displayMeta()
        {
            Action<CsMessage> metadata = (CsMessage rcvMsg) =>
            {

                Action dometadata = () =>
                {
                    rea.Name.Content = rcvMsg.value("name");
                    rea.Description.Content = rcvMsg.value("description");
                    rea.Categories.Content = rcvMsg.value("categories");
                    rea.Children.Content = rcvMsg.value("children");

                };
                Dispatcher.Invoke(dometadata, new Object[] { });

            };
            addClientProc("metadata", metadata);
        }

        //----< OR search query>------
        private void DispatcherOR()
        {
            Action<CsMessage> find = (CsMessage rcvMsg) =>
            {

                Action<string> dofind = (string fileList1) =>
                {
                    FileContent.Text += "\nSearch results for OR Query: " + fileList1;

                };
                Dispatcher.Invoke(dofind, new Object[] { rcvMsg.value("fileList1") });

            };
            addClientProc("find", find);
        }

        //----< Checkout>------
        private void DispatcherCheckOut()
        {
            Action<CsMessage> checkout = (CsMessage rcvMsg) =>
            {

                Action docheckout = () =>
                {
                    if (!File.Exists("../../../../local_folder/" + rcvMsg.value("name")))
                    {
                        File.Copy("../../../../local_folder/" + rcvMsg.value("fileName"), "../../../../local_folder/" + rcvMsg.value("name"));
                    }
                    File.Delete("../../../../local_folder/" + rcvMsg.value("fileName"));
                };
                Dispatcher.Invoke(docheckout, new Object[] { });

            };
            addClientProc("checkout", checkout);
        }

        //----< load all dispatcher processing >---------------------------
        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
            DispatcherLoadSendFile();
            DispatcherConnect();
            DispatcherSearch();
            DispatcherOR();
            displayMeta();
            DispatcherCheckOut();
        }

        //----< start Comm, fill window display with dirs and files >------
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // start Comm
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = "localhost";
            endPoint_.port = 8082;
            NavRemote.navEndPoint_ = endPoint_;
            translater = new Translater();
            translater.listen(endPoint_);
            // start processing messages
            processMessages();
            // load dispatcher
            loadDispatcher();
            CsEndPoint serverEndPoint = new CsEndPoint();

            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            pathStack_.Push("../Storage");
            NavRemote.PathTextBlock.Text = "Storage";
            NavRemote.pathStack_.Push("../Storage");
            NavLocal.PathTextBlock.Text = "LocalStorage";
            NavLocal.pathStack_.Push("");
            NavLocal.localStorageRoot_ = "../../../../LocalStorage";
            saveFilesPath = translater.setSaveFilePath("../../../../local_folder");
            sendFilesPath = translater.setSendFilePath("../../../../LocalStorage");
            NavLocal.refreshDisplay();
            NavRemote.refreshDisplay();
            test1();

        }

        //----< strip off name of first part of path >---------------------
        public string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }

        //----< show file text >-------------------------------------------
        private void showFile(string fileName)
        {
            Paragraph paragraph = new Paragraph();
            string fileSpec = saveFilesPath + "\\" + fileName;
            string fileText = File.ReadAllText(fileSpec);
            paragraph.Inlines.Add(new Run(fileText));
            CodePopupWindow popUp = new CodePopupWindow();
            popUp.codeView.Blocks.Clear();
            popUp.codeView.Blocks.Add(paragraph);
            popUp.Show();
        }

        //----< respond to mouse double-click on connect >----------------
        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "con");
            msg.add("path", txtMsg4.Text);
            win.translater.postMessage(msg);
        }

        //----< respond to mouse double-click on And query button >----------------
        public void SearchButton_Click(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(win.endPoint_));
            msg.add("command", "search");
            msg.add("name", FileName.Text);
            msg.add("description", FileDescription.Text);
            msg.add("categories", PayloadCategory.Text);
            msg.add("children", PayloadValue.Text);
            msg.add("parents", Parents.Text);
            msg.add("versions", Versions.Text);
            win.translater.postMessage(msg);
        }

        //----< respond to mouse double-click on OR query button >----------------
        public void ORButton_Click(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(win.endPoint_));
            msg.add("command", "find");
            msg.add("name", FileName.Text);
            msg.add("description", FileDescription.Text);
            msg.add("categories", PayloadCategory.Text);
            msg.add("children", PayloadValue.Text);
            msg.add("parents", Parents.Text);
            msg.add("versions", Versions.Text);
            win.translater.postMessage(msg);
        }


        private void NavLocal_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void NavRemote_Loaded(object sender, RoutedEventArgs e)
        {

        }

        //----< test >---------------------------------
        void test1()
        {
            Console.WriteLine("\n  Demonstrating Requirement #7");
            Console.WriteLine("\n  This is the automated testing\n");
            Console.WriteLine("-----------------------------------------------------------------------\n");
            Console.WriteLine("\n  Connection established\n");
            ConnectButton_Click(new object(), new RoutedEventArgs());
            NavLocal.DirList_MouseDoubleClick(new object(), new MouseButtonEventArgs(Mouse.PrimaryDevice, 0, MouseButton.Left));
            Console.WriteLine("\n  CheckIng File Comm.h\n");
            NavLocal.FileList.SelectedIndex = 0;
            NavLocal.name.Text = "Comm.h";
            NavLocal.description.Text = "file of CIS687";
            NavLocal.categories.Text = "first;second";
            NavLocal.CheckinButton_Click(new object(), new RoutedEventArgs());
            Console.WriteLine("\n  CheckIng File Comm.cpp\n");
            NavLocal.FileList.SelectedIndex = 1;
            NavLocal.name.Text = "Comm.cpp";
            NavLocal.description.Text = "Message passing file";
            NavLocal.categories.Text = "Third;Fourth";
            NavLocal.children.Text = "Comm.h";
            NavLocal.CheckinButton_Click(new object(), new RoutedEventArgs());
            Console.WriteLine("\n CheckIng File IComm.h\n");
            NavLocal.FileList.SelectedIndex = 2;
            NavLocal.name.Text = "IComm.h";
            NavLocal.description.Text = "Its a new file";
            NavLocal.categories.Text = "first;second";
            NavLocal.CheckinButton_Click(new object(), new RoutedEventArgs());
            NavRemote.refreshDisplay();
            NavRemote.DirList_MouseDoubleClick(new object(), new MouseButtonEventArgs(Mouse.PrimaryDevice, 0, MouseButton.Left));
            NavRemote.FileList.SelectedIndex = 3;
            Console.WriteLine("\n Closing file IComm.h\n");
            NavRemote.Close_Click(new object(), new RoutedEventArgs());
            System.Threading.Thread.Sleep(1000);
            Console.WriteLine("\n CheckIng file IComm.h again, checkin happens with versioning\n");
            NavLocal.refreshDisplay();
            NavLocal.FileList.SelectedIndex = -1;
            NavLocal.FileList.SelectedIndex = 1;
            NavLocal.name.Text = "IComm.h";
            NavLocal.description.Text = "Its a new file";
            NavLocal.categories.Text = "first;second";
            NavLocal.CheckinButton_Click(new object(), new RoutedEventArgs());
            NavRemote.refreshDisplay();
            Console.WriteLine(" CheckOut file Comm.cpp, The dependent children file will also be checked out i.e Comm.h\n");
            NavRemote.CheckoutButton_Click(new object(), new RoutedEventArgs());
            NavRemote.FileList_MouseDoubleClick(new object(), new MouseButtonEventArgs(Mouse.PrimaryDevice, 0, MouseButton.Left));
            NavRemote.FileList_MouseSingleClick(new object(), new MouseButtonEventArgs(Mouse.PrimaryDevice, 0, MouseButton.Left));
            Console.WriteLine("\n And query on name, description, categories and children of files in db \n");
            SearchButton_Click(new object(), new RoutedEventArgs());
            Console.WriteLine("\n OR query on name, description, categories and children of files in db \n");
            ORButton_Click(new object(), new RoutedEventArgs());
        }
    }
}
