#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
using namespace std;

int main () {
    try {
        ifstream file("file1.xml");
        if (!file.is_open())
            throw string("file is not open!");
        if (file.eof())
            throw string("file is empty!");

        Lexer lexer;
        lexer.disassemble(file);
        for (int i = 0; i < lexer.Tokens.size(); i++){
            cout << "[ \"" << lexer.Tokens[i].lexeme << "\" : " << lexer.Tokens[i].type_str() << " ]" << endl;
        }
        
        cout << endl << endl;

        Parser parser;
        parser.parse(lexer);
        parser.portray(cout);

    } catch (const string &error) {
        cerr << "nyxml: " << error << endl;
        return 1;
    }
    return 0;
}
