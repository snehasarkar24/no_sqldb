///////////////////////////////////////////////////////////////////////
// RemoteNavControl.xaml.cs - Control GUI for remote Navigation      //
// ver 1.1                                                           //
// Sneha Sarkar, CSE687 - Object Oriented Design, Spring 2018        //
// Source: Jim Fawcett                                               //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based control GUI for Project4Demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of the remote server.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   
 * Required Files:
 * ---------------
 * RemoteNavControl.xaml, RemoteNavControl.xaml.cs
 * 
 * Maintenance History:
 * --------------------
 * ver 1.1 : 01 May 2018
 * - Second release
 * - Added view metadata of file on mouse single click and checkout events on button click
 *  Public Interface :
 * ------------------
 * public RemoteNavControl()
 * private void RoutedLoaded(object sender, EventArgs e)
 * public void Refresh_Click(object sender, RoutedEventArgs e)
 * internal void refreshDisplay()
 * internal void clearDirs()
 * internal void addDir(string dir)
 * internal void insertParent()
 * internal void clearFiles()
 * internal void addFile(string file)
 * private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
 * public void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
 * public void CheckoutButton_Click(object sender, RoutedEventArgs e) - performs checkout
 * public void Close_Click(object sender, RoutedEventArgs e) - close file

 */

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
{   //----< class LocalNavControl>----------------
    public partial class RemoteNavControl : UserControl
    {
        internal CsEndPoint navEndPoint_;
        internal Stack<string> pathStack_ = new Stack<string>();
        private Dictionary<string, Action<CsMessage>> dispatcher_
      = new Dictionary<string, Action<CsMessage>>();
        //----< initialiseComp >----------------
        public RemoteNavControl()
        {
            InitializeComponent();
        }

        //RoutedLoaded
        private void RoutedLoaded(object sender, EventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            win.setRea(this);
            
        }

        //----< respond to mouse click on refresh >----------------
        private void Refresh_Click(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(navEndPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            win.translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            win.translater.postMessage(msg);
        }

        //----< resfreshDisplay >----------------
        internal void refreshDisplay()
        {
            Refresh_Click(this, null);
        }

        //----< clearDirs >----------------
        internal void clearDirs()
        {
            DirList.Items.Clear();
        }

        //----< function dispatched by child thread to main thread >-------
        internal void addDir(string dir)
        {
            DirList.Items.Add(dir);
        }

        //----< function dispatched by child thread to main thread >-------
        internal void insertParent()
        {
            DirList.Items.Insert(0, "..");
        }

        //----< function dispatched by child thread to main thread >-------
        internal void clearFiles()
        {
            FileList.Items.Clear();
        }

        //----< function dispatched by child thread to main thread >-------
        internal void addFile(string file)
        {
            FileList.Items.Add(file);
        }

        //----< show file metadata on SingleDoubleClick >-----------------------
        public void FileList_MouseSingleClick(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);

            string fileName;
            if (FileList.SelectedIndex == -1)
            {
                fileName = "Comm.h.1";
            }
            else
            {
                fileName = (string)FileList.SelectedItem;
            }
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(navEndPoint_));
            msg.add("command", "metadata");
            msg.add("path", pathStack_.Peek());
            msg.add("fileName", fileName);
            win.translater.postMessage(msg);
        }

        //----< show file text on MouseDoubleClick >-----------------------
        public void FileList_MouseDoubleClick(object sender, RoutedEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);

            string fileName;
            if (FileList.SelectedIndex == -1)
            {
                fileName = "Comm.h.1";
            }
            else
            {
                fileName = (string)FileList.SelectedItem;
            }
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(navEndPoint_));
            msg.add("command", "sendFile");
            msg.add("path", pathStack_.Peek());
            msg.add("fileName", fileName);
            win.translater.postMessage(msg);
        }

        //----< respond to mouse double-click on dir name >----------------
        public void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            MainWindow win = (MainWindow)Window.GetWindow(this);
            // build path for selected dir
            string selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock.Text = win.removeFirstDir(pathStack_.Peek());
            // build message to get dirs and post it
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(navEndPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            win.translater.postMessage(msg);
            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            win.translater.postMessage(msg);
        }

        //----< respond to mouse click on checkout >----------------
        public void CheckoutButton_Click(object sender, RoutedEventArgs e)
        {

            string fileName;
            if (FileList.SelectedIndex == -1)
            {
                fileName = "Comm.h.1";
            }
            else
            {
                fileName = (string)FileList.SelectedItem;
            }
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(win.endPoint_));
            msg.add("command", "checkout");
            msg.add("fileName", fileName);
            win.translater.postMessage(msg);

        }

        //----< respond to mouse click on close >----------------
        public void Close_Click(object sender, RoutedEventArgs e)
        {

            string fileName;
            if (FileList.SelectedIndex == -1)
            {
                fileName = "Comm.h.1";
            }
            else
            {
                fileName = (string)FileList.SelectedItem;
            }
            MainWindow win = (MainWindow)Window.GetWindow(this);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(win.endPoint_));
            msg.add("command", "close");
            msg.add("fileName", fileName);
            win.translater.postMessage(msg);

        }


    }
}
