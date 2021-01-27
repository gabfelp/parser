#include <iostream>
#include <sys/time.h>
#include <thread>
#include <string>
#include <fstream>
#include <unistd.h>
#include <set>
#include <algorithm>
#include <sys/types.h> // required for stat.h
#include <sys/stat.h>
#include <map> 

#include "gumbo.h"



using namespace std;

std::string MAP_PATH = "../collection/urlMap";

double TOTAL_TIME = 0;
int TOTAL_BARS = 0;
int fullPagesCollected = 0;
const int MAX_LENGTH = 100;

//set<string> alreadyCrawled;
//set<string> allCollected;


class Parser
{
    public:
        // return the time
        static double elapsed ();

        // return the page from scheduler
        void start();

        //do the crawling
        static string processing(std::string  website);

        //check the number of bars in url
        static int checkBars(std::string website);

        //save the page on txt
        static bool savePage(string url, string html, int number);

        

};
