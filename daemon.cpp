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
#include <vector>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <new>
#include <cstddef>

using namespace std;

//const string Programroot = "/usr/src/geobackup/";
//const string Logroot = "/var/log/geobackup/";
//const string Configroot = "/etc/geobackup/";
const string Configroot = "/root/build/geobackup/";
const string DaemonPath = Configroot+"bin/daemon";
const string TabPath = Configroot+"tab.txt";
const string LogPath = "/root/build/debug.log";
const string KnownChars = ":0123456789/,*!-$";
const string Tabformat[5] = {"year","month","day","hour","minute"};

bool fileExists(string h){//check if daemon executable is found
  struct stat buffer;   
  return (stat (h.c_str(), &buffer) == 0);
}

template <typename TS>
string to_s(TS a){//something to string
stringstream ss; //create string stream
ss << a; //print something to stream
string str = ss.str(); //convert stream to string
 return str;
}

int logN(string message){//log message to log file
  time_t seconds;
  seconds = time(NULL); //gettimestamp
  ofstream myfile;
  myfile.open(LogPath.c_str(), ios::app); //openfile to write to end
  myfile << "["+to_s(seconds)+"]"+message+"\n";
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

int gettabfile_amount(){//get amount of lines of the tab.txt
    int number_of_lines = 0;
    string line;
    ifstream myfile(TabPath.c_str());

    while (std::getline(myfile, line)){
        ++number_of_lines;
      } 
	return number_of_lines;
}

string * gettabfile(int a){//get lines from tab file in array
	ifstream file(TabPath.c_str());
		string* myArray;
        myArray = new string[a-1];
        short loop=0;
        string line;
        if (file.is_open()){
          while (loop < a-1){
            getline (file,line); //get one line from the file
            myArray[loop] = line;
            loop++;
        }
        file.close(); //closing the file
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
bool readTab(string line, int i){
	//check if line is comment or empty
	if(line.substr(0,1) == "#" || line.length() == 1){
    logN("Comment or empty line found on "+TabPath+"["+to_s(i+1)+"]. Skipping line.");
		return false;
	}
	//find first space
	size_t location;
	location = line.find_first_of(" "); //get location of the first space
	if(location != string::npos){//check for spaces
    //check if time area is correct formatted
		size_t location2;
		location2 = line.find_first_of(":", location); //get location of the first ':' after the space 
		
		if(location2 == string::npos){//checking if space is found in time area
			int strlength = line.length();
			int totalis = 0;
      bool error = false; 
			string character;
			for(int a = 0; a < location; a = a + 1 ){//loop through chars from time section
				character = line.substr(a,1); //not a char because sometimes have to use it as string
				if(character == ":"){//new ':' found
					totalis++;
				}
      //save in arrag so ca  be used later
				if(KnownChars.find_first_of(character) == string::npos){//check if character is illegal
					logN("Incorrect formatting, illegal token '"+character+"' found on "+TabPath+"["+to_s(i+1)+"].");
          error = true;
				}
        if(character == ":" && line.substr(a+1,1) == ":"){//check if part is empty
          logN("Incorrect formatting, "+Tabformat[totalis]+" is empty on "+TabPath+"["+to_s(i+1)+"].");
          error = true;
        }
			}
			if(totalis < 4){//to little ':'
				logN("Incorrect formatting, to little ':' on "+TabPath+"["+to_s(i+1)+"].");
				error = true;
			}
      if(totalis > 4){//to little ':'
        logN("Incorrect formatting, to many ':' on "+TabPath+"["+to_s(i+1)+"].");
        error = true;
      }
      if(error){//if there were errors
        logN("Line "+to_s(i+1)+" skipped.");
        return false;
      }

      //check if backup paths are formatted right
      string backuppaths = line.substr(location+1, strlength-(location+1));
      unsigned short int bplength = backuppaths.length();
			string character2;
			int EarlierSpace = 0;
			vector<string> backupP;
			vector<string> excludeP;
      for(int a = 0; a < bplength; a++){//loop through chars from path section
				character2 = backuppaths.substr(a,1);
				if(character2 == " " && backuppaths.substr(a+1,1) != " "){//if the >1nd space is found and the next char is no space
						if(backuppaths.substr(EarlierSpace+1,1) != "!"  && EarlierSpace > 0){//if it has to be excluded
							string Bpath = backuppaths.substr(EarlierSpace+2, a-1);
							if(fileExists(Bpath)){
							backupP.resize(backupP.size()+1);
							backupP[backupP.size()-1] = Bpath; //add path to array
							}else{
								logN("The path '"+Bpath+"' on "+TabPath+"["+to_s(i+1)+"] doesn't exist.");
							}
						}else if(EarlierSpace > 0){//if it hast to be backupped
							string Bpath = backuppaths.substr(EarlierSpace+1, a-1);
							if(fileExists(Bpath)){
							excludeP.resize(excludeP.size()+1);
							excludeP[excludeP.size()-1] = Bpath; //add path to array
							}else{
								logN("The path '"+Bpath+"' on "+TabPath+"["+to_s(i+1)+"] doesn't exists.");
							} 
						}
						EarlierSpace = a;
				}

      }
			string currentTime = getTime(); //get current time in year:month:day:hour:minute format
			if(line.substr(0,1) == "$"){//check if always backup ('$') is set
				return true;
			}
			string Timestime = line.substr(startPos, location-1); //get time section from line
        logN("BACKUP["+to_s(i+1)+"]");
		}else{
			logN("Space(s) found on "+TabPath+"["+to_s(i+1)+"] in the area where the time is defined. Skipping line.");
			return false;
		}
	}else{
		logN("No spaces found on "+TabPath+"["+to_s(i+1)+"]. Skipping line.");
		return false;
	}
}

bool generateCommand(string type){
  logN("BACKUP");
	return false;
} 
bool process(){//the process which get runned every minute
  logN("Running process...");
  struct stat buffer;   
  if(stat(TabPath.c_str(), &buffer) == 0){//check if tab file exists
  int line_amount = gettabfile_amount();
  string* lines = gettabfile(line_amount);
  for(int i = 0; i < line_amount-1; ++i){
	  bool MustRunNow = readTab(lines[i], i);
	  if(MustRunNow){
		  generateCommand("backup");
	  }
  }
  delete[] lines; //delete lines memory
}else{
	logN("Tab file (still) not accessable!");
	return false;
}}

int main(){
		if(fileExists(DaemonPath)){

    	logN("Entering Daemon GB");
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
    	//if ((chdir(Configroot.c_str())) < 0) { exit(EXIT_FAILURE); }
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