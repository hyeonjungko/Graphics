#ifndef _SCENEGRAPHTTEXTRENDER_H_
#define _SCENEGRAPHTTEXTRENDER_H_

using namespace std;
#include <istream>
#include <map>
#include <string>
#include <iostream>

namespace sgraph {
  class SceneGraphTextRender {

    public:
      SceneGraphTextRender() {};
      void render(istream& input, int l, string n) {
        initial_level = l;
        level = initial_level;
        root = n;
        string command;
        string inputWithOutCommentsString = stripComments(input);
        istringstream inputWithOutComments(inputWithOutCommentsString);

        if (n != "") {
          cout << getSpaces(initial_level) + " - " + n << endl; 
        }

        while (inputWithOutComments >> command) {  
          if (command == "assign-root") {
            parseRoot(inputWithOutComments);
          } 
          else if (command == "group") {
            parseGroup(inputWithOutComments);
          } 
          else if (command == "add-child") {
            parseAddChild(inputWithOutComments);
          } 
          else if (command == "import") {
            parseImport(inputWithOutComments);
          }
        }
      }

    protected: 
      void parseRoot(istream& input) {
        string rootname;
        input >> rootname;

        if (import_node == "") {
          if (root.empty()) {
            level = 1;
            root = rootname;
            root_level.insert({root, 1});
            cout << getSpaces(level) + root << endl;
          }
        }
      }

      void parseGroup(istream& input) {
        string varname, name;
        input >> varname;

        if (import_node == "") {
          if (root.empty()) {
            level = 1;
            root = varname;
            root_level.insert({root, 1});
            cout << getSpaces(level) + root << endl;
          }
        } else {
          level += 1;
          root_level.insert({varname, level});
          cout << getSpaces(level) + " - " + varname << endl;
        }
      } 

      void parseAddChild(istream& input) {
        string childname, parentname;
        input >> childname >> parentname;

        if (import_node != childname) {
          if (root_level.count(parentname)) {
            int parent_level = root_level.at(parentname);
            level = parent_level;
          } else {
            level += 1;
            root_level.insert({parentname, level});
            cout << getSpaces(level) + " - " + parentname << endl;
          }
        
          level += 1;
          if (root_level.count(childname)) {
            level -= 1;
          } else {
            root_level.insert({childname, level});
            cout << getSpaces(level) + " - " + childname << endl;
          }
        } else {
          import_node = "";
        }
      }

      void parseImport(istream& input) {
        string nodename,filepath;

        input >> nodename >> filepath;
        ifstream external_file(filepath);
        level += 1;
        import_node = nodename;
        render(external_file, level, nodename);
      }

      string getSpaces(int level) {
        string spaces = "";
        for (int i = 0; i < level; i++) {
          spaces += " ";
        }
        return spaces;
      }

      string stripComments(istream& input) {
        string line;
        stringstream clean;
        while (getline(input,line)) {
          int i=0;
          while ((i<line.length()) && (line[i]!='#')) {
            clean << line[i];
            i++;
          }
          clean << endl;
        }
        return clean.str();
      }

    private:
      int level;
      int initial_level;
      string root = "";
      string import_node = "";
      map<string, int> root_level;
  };
}

#endif