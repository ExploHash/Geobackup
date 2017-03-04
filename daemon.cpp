// my first program in C++
#include <iostream>
#include <fstream>
//#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctime>
#include <sstream>


using namespace std;

//const string Programroot = "/usr/geobackup/";
const string Programroot = "/root/build/geobackup/";
const string DaemonPath = Programroot+"bin/daemon";
const string TimesPath = Programroot+"times.txt";
const string LogPath = "/root/build/debug.log";

bool isInstalled(){//check if daemon executable is fund
  struct stat buffer;   
  return (stat (DaemonPath.c_str(), &buffer) == 0);
}

string to_s(int a){//int to string
stringstream ss;
ss << a;
string str = ss.str();
 return str;
}

int logN(string message){//log message to log file
  ofstream myfile;
  myfile.open(LogPath.c_str(), ios::app);
  myfile << message+"\n";
  myfile.close();
  return 0;
}
string getTime(){
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t ); //set object tm to now
  int hour = now->tm_hour;
  int minute = now->tm_min;
  return to_s(hour) + ":" + to_s(minute);
}

int gettimesfile_amount(){//get amount of lines of the times.txt
    int number_of_lines = 0;
    string line;
    ifstream myfile(TimesPath.c_str());

    while (std::getline(myfile, line)){
        ++number_of_lines;
      } 
    return number_of_lines;
}

void process(){//the process which get runned every minute
  logN("Running process...");
  //check if /usr/geobackup/times.txt exists
  int line_amount = gettimesfile_amount();
  //logN(to_s(line_amount));
}

int main(){
  	if(isInstalled()){

    	logN("Entering Daemon");
    	pid_t pid, sid;

   		//Fork the Parent Process
    	pid = fork();

    	if (pid < 0) { exit(EXIT_FAILURE); }

   		//We got a good pid, Close the Parent Process
    	if (pid > 0) { exit(EXIT_SUCCESS); }

    	//Change File Mask
    	umask(0);

   		 //Create a new Signature Id for our child
    	sid = setsid();
    	if (sid < 0) { exit(EXIT_FAILURE); }

    	//Change Directory
    	//If we cant find the directory we exit with failure.
    	//if ((chdir(Programroot.c_str())) < 0) { exit(EXIT_FAILURE); }
   		 //Close Standard File Descriptors
   		close(STDIN_FILENO);
    	close(STDOUT_FILENO);
   		close(STDERR_FILENO);

    	//----------------
    	//Main Process
    	//----------------
    	int x = 0;
    	while(x<5){
    		x++;
        	process();    //Run our Process
        	sleep(1);    //Sleep for 1 seconds later 60 seconds
        	if(x == 5){  //end
        		return 0;
        	}
    	}
  	}else{
  		logN("Daemon executable not found!");
  		return 0;
  	}
}
