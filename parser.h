#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <thread>

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

// vars to change
string COLLECTION_PATH = "../collection/";
string MAP_PATH = COLLECTION_PATH + "urlMap";
string OUT_FOLDER = "./out";
string VOCAB_PATH = OUT_FOLDER+"/vocabulary.txt"; // where the vocabulary will be placed
string INDEX_PATH = OUT_FOLDER+"/index.txt"; 
int PAGES_TO_PARSE = 10000;


// the key of first map is the word.
// the second map represents a document and all occurrences positions in the document 
map<std::string,map<int,vector<int>>> tokenMap;


double TOTAL_TIME = 0;
const int MAX_LENGTH = 100;
int TOTAL_WORDS_COUNT = 0;

class Parser
{
    public:
        // return the time
        static double elapsed ();

        /* loads the url map from collection.
        each line of the file contains a url followed by a number. 
        The number is the name of the .html file for that url on the collection, for example "2.html"*/
        map<int,std::string> loadUrlMap();

        // runs the parsing
        void start();

        // do the parsing on the html
        static std::string cleanText(GumboNode* node);

        // pre process before token creation
        static std::vector<std::string> preProcessing(std::string content);

        // add words in the vocabulary
        void feedMap(int docNumber, std::vector<std::string> doc);

        // save the vocabulary and index, true if sucess
        static bool saveVocabularyAndIndex();

        // print the output results and return the vocabulary sorted by occurrence
        static vector<pair<string,int>> printOutput();


        

};
