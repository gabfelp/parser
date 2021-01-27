#include "parser.h"


double Parser::elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
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

void Parser::feedMap(std::vector<std::string> doc){
  //add words in map
  for(string re : doc){
    cout<<re<<endl;
  }

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

void Parser::start(map<int,std::string> urlMap){
    double t0 = elapsed();

    for(int i = 0; i < PAGES_TO_PARSE; i ++){
      std::string contents;
      ifstream file(COLLECTION_PATH+to_string(i)+".html");
      file.seekg(0,std::ios::end);
      contents.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(&contents[0], contents.size());
      file.close();

      GumboOutput* out = gumbo_parse(contents.c_str());
      std::string cleanWebsite = cleanText(out->root);
      gumbo_destroy_output(&kGumboDefaultOptions, out);

      std::vector<std::string> preProcessedDoc = preProcessing(cleanWebsite);
      
      feedMap(preProcessedDoc);
    }

    
    


    double t1 = elapsed();

    TOTAL_TIME = t1-t0;

    std::cout << "Average Parsing time per page : "<< TOTAL_TIME/PAGES_TO_PARSE << "s\n";
    std::cout << "Total time : "<< TOTAL_TIME << "s\n";
}


int main(int argc, char **argv){
    
    map<int,std::string> urlMap;
    Parser p;


    ifstream map(MAP_PATH);
    if(map.fail()){
      cout << "Url Map doesn't exist, continuing anyway" << endl;
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
    /*
    for(auto const& elem : urlmap){
        cout << elem.first << ":" << elem.second << endl;
    }
    */
   cout<<urlMap.size();

    //p.start(urlMap);

    return 0;
}
