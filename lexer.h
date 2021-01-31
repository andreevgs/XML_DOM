#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class Lexer {
public:
    static const unsigned long long MAX_TOKENS_COUNT = 0xFFFFFF;//макс кол-во токенов
    static const unsigned long long MAX_BUFFER_SIZE = 0xFFFFFF;
    static const string block_tags[];
    static const string empty_tags[];
    static const unsigned int BLOCK_TAGS_COUNT = 97;
    static const unsigned int EMPTY_TAGS_COUNT = 16;

    enum Token_type {
        END = 1, //000 000 000 001
        TEXT = 2,//000 000 000 010
        OPENING_BLOCK_TAG_NAME = 4,//000 000 000 100
        CLOSING_BLOCK_TAG_NAME = 8,//000 000 001 000
        EMPTY_TAG_NAME = 16,//000 000 010 000
        COMMENT = 32,//000 000 100 000
        MACRO_TAG = 64,//000 001 000 000
        ATTRIBUTE_NAME = 128,//000 010 000 000
        UNQUOTED_ATTRIBUTE_VALUE = 256,//000 100 000 000
        SINGLE_QUOTED_ATTRIBUTE_VALUE = 512,//001 000 000 000
        DOUBLE_QUOTED_ATTRIBUTE_VALUE = 1024//010 000 000 000
    };//перечисление типов токенов
    
    struct Token {
        string lexeme;
        Token_type type;
        string type_str () {
            switch (type) {
            case END: return "END";
            case TEXT: return "TEXT";
            case OPENING_BLOCK_TAG_NAME: return "OPENING_BLOCK_TAG_NAME";
            case CLOSING_BLOCK_TAG_NAME: return "CLOSING_BLOCK_TAG_NAME";
            case EMPTY_TAG_NAME: return "EMPTY_TAG_NAME";
            case COMMENT: return "COMMENT";
            case MACRO_TAG: return "MACRO_TAG";
            case ATTRIBUTE_NAME: return "ATTRIBUTE_NAME";
            case UNQUOTED_ATTRIBUTE_VALUE: return "UNQUOTED_ATTRIBUTE_VALUE";
            case SINGLE_QUOTED_ATTRIBUTE_VALUE: return "SINGLE_QUOTED_ATTRIBUTE_VALUE";
            case DOUBLE_QUOTED_ATTRIBUTE_VALUE: return "DOUBLE_QUOTED_ATTRIBUTE_VALUE";
            }
            return "NONE";
        }
    };
    vector<Token> Tokens;

private:
    string buffer, buffer_for_check;
    unsigned long long curr_token_type;

    void process(const char &c);
    void add(const string &lexeme, const Token_type &type);
    void check_tag();

public:
    Lexer();
    Lexer(ifstream &file);
    
    vector<Token> getClosingTags() const;
    vector<Token> getOpeningTags() const;

    void disassemble(ifstream &file);
};

#endif // LEXER_H
