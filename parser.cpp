#include "parser.h"

Parser::Parser () {
    tree = new Root;
}

Parser::Parser (const Lexer &lexer) : Parser() {
    parse(lexer);
}

Parser::Parser (ifstream &file) : Parser() {
    parse(file);
}

Parser::~Parser () {
    delete tree;
}



void Parser::parse (const Lexer &lexer) {
    Block * open_block = (Block*) tree;
    Node * last_node = (Node*) tree;
    vector<Lexer::Token> closingTags;
    vector<Lexer::Token> openingTags;
    
    openingTags = lexer.getOpeningTags();
    closingTags = lexer.getClosingTags();
//    cout << "op tags: " << openingTags.size() << endl;
//    cout << "cl tags: " << closingTags.size() << endl;
    try {
        if(openingTags.size() > closingTags.size()){
            for(int i = 0; i < closingTags.size(); i++){
                for(int j = 0; j < openingTags.size(); j++){
                    if(openingTags[j].lexeme == closingTags[i].lexeme){
//                        cout << "for delete: " << openingTags[i].lexeme << endl;
                        openingTags.erase(openingTags.begin() + j);
                        break;
                    }
                }
            }
            throw string("no closing tag for <" + openingTags[0].lexeme + ">");
        }

        for (unsigned long long i(0); i < lexer.Tokens.size(); i++) {
            switch (lexer.Tokens[i].type) {
            case Lexer::TEXT: {
                last_node = open_block->add("TEXT", lexer.Tokens[i].lexeme);
            break; }
            case Lexer::OPENING_BLOCK_TAG_NAME: {
                last_node = open_block = open_block->open(lexer.Tokens[i].lexeme);
                for(unsigned long long j = i; j < lexer.Tokens.size(); j++){
                    if(lexer.Tokens[i].lexeme == lexer.Tokens[j].lexeme){
                        if(lexer.Tokens[j].type == Lexer::CLOSING_BLOCK_TAG_NAME){
                            cout << "lexeme: " << lexer.Tokens[j].lexeme << endl;
                        }
                        
                    }
                }
                cout << endl;
                
            break; }
            case Lexer::CLOSING_BLOCK_TAG_NAME: {
                if (lexer.Tokens[i].lexeme != open_block->get_name())
                    throw string("unexpected closing tag: </" + lexer.Tokens[i].lexeme + ">");
                open_block = open_block->close();
            break; }
            case Lexer::EMPTY_TAG_NAME: {
                last_node = open_block->add(lexer.Tokens[i].lexeme);
            break; }
            case Lexer::COMMENT: {
                last_node = open_block->add("COMMENT", lexer.Tokens[i].lexeme);
            break; }
            case Lexer::MACRO_TAG: {
                last_node = open_block->add("MACRO", lexer.Tokens[i].lexeme);
            break; }
            case Lexer::ATTRIBUTE_NAME: {
                last_node->add_attr(lexer.Tokens[i].lexeme, lexer.Tokens[i].lexeme);
            break; }
            case Lexer::UNQUOTED_ATTRIBUTE_VALUE: {
                last_node->set_last_attr(lexer.Tokens[i].lexeme);
            break; }
            case Lexer::SINGLE_QUOTED_ATTRIBUTE_VALUE: {
                last_node->set_last_attr(lexer.Tokens[i].lexeme);
            break; }
            case Lexer::DOUBLE_QUOTED_ATTRIBUTE_VALUE: {
                last_node->set_last_attr(lexer.Tokens[i].lexeme);
            break; }
            case Lexer::END: {
                open_block->close();
            }
            }
        }
    } catch (const string &error) {
        cout << "parser: " << error << endl;
        exit(1);
    }
//    for(unsigned long long i(0); i < closingTags.size(); i++){
//        cout << closingTags[i].lexeme << endl;
//    }
}

void Parser::parse (ifstream &file) {
    parse(Lexer(file));
}

unsigned long long Parser::count () const {
    return tree->count();
}

void Parser::portray (ostream &out) const {
    unsigned int depth(0);
    tree->portray(out, depth);
}
