#include "parser.h"


double Parser::elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

map<int,std::string> Parser::loadUrlMap(){
  map<int,std::string> urlMap;
  ifstream map(MAP_PATH);
  if(map.fail()){
    cout << "Url Map doesn't exist on " << MAP_PATH << endl;
  }else{
    // gets the website and numPages from argv
    std::string line;
    std::string delimiter = " ";
    std::string word;
    while(map){
        std::getline(map, line);
        int pos = 0;
        if(line == ""){
            continue;
        }
        
        while((pos = line.find(delimiter)) != std::string::npos){
            word = line.substr(0,pos);
            line.erase(0,pos+delimiter.length());
        }
        urlMap.insert(pair<int,std::string>(stoi(line),word));
        
    }
  }

  return urlMap;
}

std::string Parser::cleanText(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (node->type == GUMBO_NODE_ELEMENT &&
              node->v.element.tag != GUMBO_TAG_SCRIPT &&
              node->v.element.tag != GUMBO_TAG_STYLE) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = cleanText((GumboNode*) children->data[i]);
      if (i != 0 && !text.empty()) {
        contents.append(" ");
      }
      contents.append(text);
    }
    return contents;
  } else {
    return "";
  }
}

std::vector<std::string> Parser::preProcessing(std::string content){
  CkString obj;
  
  obj.appendUtf8(content.c_str());
  // eliminate line break
  obj.eliminateChar('\n',0);
  // put everything to lower case
  obj.toLowerCase();
  std::string ret = obj.getStringUtf8();
  
  // remove punctuation
  //std::replace_if(ret.begin() , ret.end() , [] (const char& c) { return std::ispunct(c) ;},' ');

  // chars to remove
  char chars_to_remove[] = "\“\”|?#.,/!:";
  for(int j = 0; j < strlen(chars_to_remove);j++){
    ret.erase(std::remove(ret.begin(), ret.end(), chars_to_remove[j]), ret.end());
  }
  /*
  //for replacing m0 with m²
  int idx = 0;
  while(true){
    idx = ret.find("mÒ",idx);
    if (idx == std::string::npos) break;
    ret.replace(idx,2,"m²");
    idx+=2;
  }
  */

  std::vector<std::string> result; 
  std::istringstream iss(ret); 
  for(std::string ret; iss >> ret; ) 
    result.push_back(ret); 

  


  return result;

}

void Parser::feedMap(int docNumber,std::vector<std::string> doc){
  int wordCount = 0;
  for(string re : doc){
    if ( tokenMap.find(re) == tokenMap.end() ) {
      // word not found, add word + (doc+occurrence)
      map<int,vector<int>> sub;
      sub.insert(pair<int,vector<int>>(docNumber,{wordCount}));

      tokenMap.insert(pair<std::string,map<int,vector<int>>>(re,sub));

    } else {
      // word found, check doc
      
      //check if doc already exists
      if ( tokenMap[re].find(docNumber) == tokenMap[re].end() ) {
        // doc not found, add doc+occurrence
        tokenMap[re].insert(pair<int,vector<int>>(docNumber,{wordCount}));
      } else {
        // doc found, add occurrence
        tokenMap[re][docNumber].push_back(wordCount);

      }


    }
    //cout<<re<<endl;
    wordCount++;
  }
  TOTAL_WORDS_COUNT+= wordCount;
}

void Parser::printVocabulary(){
  for(auto iter = tokenMap.begin(); iter != tokenMap.end(); iter++){
    cout << iter->first << " ";
    for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++){
      cout << iter2->first << " { ";
      for(auto elem : iter2->second){
        cout << elem << " ";
      }
      cout << "}" << endl;
    }
  }
}

void Parser::printOutput(){
  int size = 0;
  size += tokenMap.size() * sizeof(string); // number of words
  
  
  for(auto iter = tokenMap.begin(); iter != tokenMap.end(); iter++){
    size +=iter->second.size() * sizeof(int);
    for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++){
      size +=iter2->second.size() * sizeof(int);
    }
  }

  //for Kb
  float size_kb = size / 1000.0; 

  cout << "Inverted Index Size (in Kbytes): " << size_kb << endl;
  
}

bool Parser::saveIndex(){
    try{
        std::ofstream outVocab;

        outVocab.open(VOCAB_PATH,std::ios_base::app);
        
        outVocab.close();
    }catch (int e){
        std::cout << "Error on saving" << endl;
    }
    return true;
}

void Parser::start(){
    double t0 = elapsed();

    for(int i = 0; i < PAGES_TO_PARSE; i ++){
      std::string contents;
      ifstream file(COLLECTION_PATH + to_string(i)+".html");

      if(file.fail()){
        cout << "File" << COLLECTION_PATH+to_string(i)<<".html not found, stopping."<<endl;
        PAGES_TO_PARSE = i;
        break;
      }

      file.seekg(0,std::ios::end);
      contents.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(&contents[0], contents.size());
      file.close();

      GumboOutput* out = gumbo_parse(contents.c_str());
      std::string cleanWebsite = cleanText(out->root);
      gumbo_destroy_output(&kGumboDefaultOptions, out);

      std::vector<std::string> preProcessedDoc = preProcessing(cleanWebsite);
      
      feedMap(i, preProcessedDoc);
    }


    double t1 = elapsed();

    TOTAL_TIME = t1-t0;


    std::cout << "Total Words Count : "  << TOTAL_WORDS_COUNT << endl;
    std::cout << "Number of Terms in Vocabulary : " << tokenMap.size() << endl;
    printOutput();

    std::cout << "Average Parsing time per page : "<< TOTAL_TIME/PAGES_TO_PARSE << "s" << endl;
    std::cout << "Total time : "<< TOTAL_TIME << "s" << endl;
}


int main(int argc, char **argv){
    
    
    Parser p;

    
    /* 
    map<int,std::string> urlMap = p.loadUrlMap();

    for(auto const& elem : urlMap){
        cout << elem.first << " : " << elem.second << endl;
    }
    cout<<urlMap.size();
    */

    p.start();

    return 0;
}
