// my first program in C++
#include <iostream>
#include <fstream>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

using namespace std;

const string Programroot = "/usr/geobackup/";
const string DaemonPath = Programroot+"bin/daemon";

//bool isInstalled(){
//	if (std::ifstream DaemonPath.c_str())
//	{
//     	return true;
//	}
//	std::ofstream file(DaemonPath.c_str()));
//	if (!file)
//	{
//    	 return false;
//	}
//}

void process(){
	syslog(LOG_INFO, "Running...");
}

int main(){
	setlogmask (LOG_UPTO (LOG_NOTICE));
    openlog ("geobackup", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  	if(true){

    	syslog(LOG_INFO, "Entering Daemon");
    	//pid_t pid, sid;

   		//Fork the Parent Process
    	///pid = fork();

    	///if (pid < 0) { exit(EXIT_FAILURE); }

   		//We got a good pid, Close the Parent Process
    	///if (pid > 0) { exit(EXIT_SUCCESS); }

    	//Change File Mask
    	///umask(0);

   		 //Create a new Signature Id for our child
    	///sid = setsid();
    	///if (sid < 0) { exit(EXIT_FAILURE); }

    	//Change Directory
    	//If we cant find the directory we exit with failure.
    	if ((chdir(Programroot.c_str())) < 0) { exit(EXIT_FAILURE); }
    	//cout << "work2";
   		 //Close Standard File Descriptors
   		//close(STDIN_FILENO);
    	//close(STDOUT_FILENO);
   		//close(STDERR_FILENO);

    	//----------------
    	//Main Process
    	//----------------
    	cout << "work3";
    	int x = 0;
    	while(x<5){
    		x++;
        	process();    //Run our Process
        	sleep(60);    //Sleep for 60 seconds
        	if(x == 5){
        		return 0;
        	}
    	}

    	//Close the log
    	closelog ();
  	}else{
  		syslog(LOG_WARNING, "Daemon executable not found!");
  		return 0;
  	}
}
