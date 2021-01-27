#include <iostream>
#include <sys/time.h>
#include <thread>

#include <fstream>
#include <unistd.h>
#include <set>
#include <algorithm>
#include <sys/types.h> // required for stat.h
#include <sys/stat.h>
#include <map> 
#include <vector>
#include <sstream>
#include <string>
#include <cstring>

#include "gumbo.h"
#include <CkString.h>


using namespace std;

std::string COLLECTION_PATH = "../collection/";
std::string MAP_PATH = COLLECTION_PATH + "urlMap";
std::string VOCAB_PATH = "./out/vocabulary.txt"; // place and name of map

double TOTAL_TIME = 0;
const int MAX_LENGTH = 100;
int PAGES_TO_PARSE = 1;

map<std::string,vector<pair<int,int>>> tokenMap;


class Parser
{
    public:
        // return the time
        static double elapsed ();

        // runs the parsing
        void start(map<int,std::string> urlMap);

        // do the parsing on the html
        static std::string cleanText(GumboNode* node);

        // pre process before token creation
        static std::vector<std::string> preProcessing(std::string content);

        // feed the tokenMap
        void feedMap(std::vector<std::string> doc);

        // save Vocabulary
        bool saveIndex();

        

};
