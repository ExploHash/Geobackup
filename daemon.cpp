// my first program in C++
#include <iostream>
#include <fstream>
#include <syslog .h>
using namespace std;

const string Programroot = "/usr/geobackup/";

int main(){

	setlogmask (LOG_UPTO (LOG_NOTICE));
    openlog ("geobackup", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  	if(isInstalled()){
  		
    	syslog(LOG_INFO, "Entering Daemon");

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
    	if ((chdir(Programroot)) < 0) { exit(EXIT_FAILURE); }

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
        	sleep(60);    //Sleep for 60 seconds
        	if(x === 5){
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

bool isInstalled(){
	if (std::ifstream(Programroot+"bin/daemon"))
	{
     	return true;
	}
	std::ofstream file(Programroot+"bin/daemon");
	if (!file)
	{
    	 return false;
	}
}

void process(){
	syslog(LOG_INFO, "Running...");
}
