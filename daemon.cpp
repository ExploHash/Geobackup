#include <iostream>
#include <fstream>
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
#include <new>
#include <cstddef>

using namespace std;

//const string Programroot = "/usr/geobackup/";
const string Programroot = "/root/build/geobackup/";
const string DaemonPath = Programroot+"bin/daemon";
const string TimesPath = Programroot+"times.txt";
const string LogPath = "/root/build/debug.log";
const string KnownChars = ":0123456789/,*!-";

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

string * gettimesfile(int a){
	ifstream file(TimesPath.c_str());
		string* myArray;
        myArray = new string[a];

        for(int i = 0; i < 5; ++i)
        {
            file >> myArray[i];
        }
		return myArray;  //pointer so I can delete memory later
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
	size_t location;
	location = line.find_first_of(" "); //get location of the first space
	if(location != string::npos){
		size_t location2;
		location2 = line.find_first_of(":", location); //get location of the first ':' after the space 
		if(location2 == string::npos){//checking if space is found in time area
			//check if time area is correct formatted
			int strlen = line.length();
			int totalis = 0;
			for(int a = 0; a < location; a = a + 1 ){//loop through chars
				string character = line.substr(a,1); //not a char because sometimes have to use it as string
				if(character == ":"){//new ':' found
					totalis++;
				}

				if(KnownChars.find_first_of(character)){//check for known chars
					logN("Incorrect formatting, illegal token '"+character+"' found on "+TimesPath+"["+to_s(i)+"]. Skipping line.");
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
			int startPos = 0;
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

char * generateCommand(string type){
	char * command = new char[12];
	return command;
} 
bool process(){//the process which get runned every minute
  logN("Running process...");
  struct stat buffer;   
  if(stat(TimesPath.c_str(), &buffer) == 0){//check if tab file exists
  int line_amount = gettimesfile_amount();
  string* lines = gettimesfile(line_amount);
  for(int i = 0; i < line_amount; ++i){
	  bool MustRunNow = checkTimes(lines[i], i);
	  if(MustRunNow){
		  exec(generateCommand("backup"));
	  }
  }
  delete[] lines; //delete lines memory
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
