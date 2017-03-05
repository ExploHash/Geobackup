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
#include <stdexcept>
#include <string>

using namespace std;

//const string Programroot = "/usr/geobackup/";
std::const string Programroot = "/root/build/geobackup/";
std::const string DaemonPath = Programroot+"bin/daemon";
std::const string TimesPath = Programroot+"times.txt";
std::const string LogPath = "/root/build/debug.log";

bool isInstalled(){//check if daemon executable is found
  struct stat buffer;   
  return (stat (DaemonPath.c_str(), &buffer) == 0);
}

string to_s(int a){//int to string
stringstream ss; //create string stream
ss << a; //print int to stream
string str = ss.str(); //convert stream to string
 return str;
}

int logN(string message){//log message to log file
  ofstream myfile;
  myfile.open(LogPath.c_str(), ios::app); //openfile to write to end
  myfile << message+"\n";
  myfile.close();
  return 0;
}
string getTime(){
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t ); //set object tm to now
  int year = now->tm_year + 1900;
  int month = now->tm_mon + 1;
  int day = now->tm_mday;
  int hour = now->tm_hour;
  int minute = now->tm_min;
  return to_s(year) + ":" + to_s(month) + ":" + to_s(day) + ":" + to_s(hour) + ":" + to_s(minute);
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

string gettimesfile(int a){
	ifstream file(TimesPath.c_str());

        string myArray[a];

        for(int i = 0; i < 5; ++i)
        {
            file >> myArray[i];
        }
		return myArray;
}

string exec(const char* cmd) {//I know, I know, grabbed from StackOverflow :/
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
bool checkTimes(string line, int i){
	//check if line is comment or empty
	if("#" == line.substr(0,1) && line.empty()){
		return false;
	}
	//find first space
	location = strchr(line, ' '); //get location of the first space
	if(location != NULL){
		location2 = line.find_first_of(":", location); //get location of the first ':' after the space 
		if(location2 == string::npos){//checking if space is found in time area
			//check if time area is correct formatted
			int strlen = line.length();
			char lineChars[strlen];
			strncpy(lineChars, line.c_str(), sizeof(lineChars)); //convert string to chars
			lineChars[sizeof(lineChars) - 1] = 0;
			int totalis = 0;
			for(a = 0; a < location; a = a + 1 ){//loop through chars
				if(lineChars[a] == ':'){//new ':' found
					totalis++;
				}
				if(strchr(":0123456789/,*!", lineChars[a]) == NULL){//check for know chars
					logN("Incorrect formatting, illegal token '"+lineChars[a]+"' found on "+TimesPath+"["+to_s(i)+"]. Skipping line.");
					return false;
				}
				//stoped working here
				//
				//
				//
			}
			if(totalis < 4){//to little ':'
				logN("Incorrect formatting, to little ':' on "+TimesPath+"["+to_s(i)+"]. Skipping line.");
				return false;
			}

			string currentTime = getTime(); //get current time in year:month:day:hour:minute format
			int startPos = 0
			if(line.substr(0,1) == "$"){//check if always backup ('$') is set
				startPos++;
			}
			string Timestime = line.substr(startPos, location-1); //get time section from line

		}else{
			logN("Space(s) found on "+TimesPath+"["+to_s(i)+"] in the area where the time is defined. Skipping line.");
			return false;
		}
	}else{
		logN("No spaces found on "+TimesPath+"["+to_s(i)+"]. Skipping line.");
		return false;
	}
}

void process(){//the process which get runned every minute
  logN("Running process...");
  struct stat buffer;   
  if(stat(TimesPath.c_str(), &buffer) == 0){
  int line_amount = gettimesfile_amount();
  string lines[line_amount] = gettimesfile(line_amount);
  for(int i = 0; i < line_amount; ++i){
	  bool MustRunNow = checkTimes(lines[i], i);
	  if(MustRunNow){
		  exec(generateCommand("backup"));
	  }
  }
}else{
	logN("Times file (still) not accessable!");
	return false;
}}

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
