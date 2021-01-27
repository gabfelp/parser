#include "parser.h"


double Parser::elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

int Parser::checkBars(std::string url){
    int n = count(url.begin(), url.end(), '/');
    n = n - 2; // double bar from "http://"

    if(n < 0){
        return 0;
    }
    return n;
}


void Parser::start(){
    double t0 = elapsed();

    double t1 = elapsed();

    TOTAL_TIME = t1-t0;

    
    std::cout << "Total time : "<< TOTAL_TIME << "s\n";
}

static std::string cleantext(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (node->type == GUMBO_NODE_ELEMENT &&
             node->v.element.tag != GUMBO_TAG_SCRIPT &&
             node->v.element.tag != GUMBO_TAG_STYLE) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = cleantext((GumboNode*) children->data[i]);
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

int main(int argc, char **argv){
    
    map<int,std::string> urlmap;
    Parser p;

    ifstream map("../collection/urlMap");
    //ifstream fin("seeds.txt");
    // gets the website and numPages from argv
    std::string line;
    std::string delimiter = " ";
    std::string token;
    while(map){
        std::getline(map, line);
        int pos = 0;
        if(line == ""){
            continue;
        }
        
        while((pos = line.find(delimiter)) != std::string::npos){
            token = line.substr(0,pos);
            line.erase(0,pos+delimiter.length());
            //cout<<token<<"\n";
        }
        //cout<<line<<"\n";
        urlmap.insert(pair<int,std::string>(stoi(line),token));
        
    }
    /*
    for(auto const& elem : urlmap){
        cout << elem.first << ":" << elem.second << endl;
    }
    */
    std::string contents;
    ifstream file("../collection/0.html");
    file.seekg(0,std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();

    GumboOutput* out = gumbo_parse(contents.c_str());
    std::cout << cleantext(out->root) << std::endl;
    gumbo_destroy_output(&kGumboDefaultOptions, out);

    //p.start();

    return 0;
}
