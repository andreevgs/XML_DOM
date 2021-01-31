#include "lexer.h"

Lexer::Lexer () {
    buffer = new char[MAX_BUFFER_SIZE];
}

Lexer::Lexer (ifstream &file) : Lexer() {
    disassemble(file);
}

vector<Lexer::Token> Lexer::getClosingTags() const{
    vector<Lexer::Token> tags;
    for(int i = 0; i < Tokens.size(); i++){
        if(Tokens[i].type == CLOSING_BLOCK_TAG_NAME){
            tags.push_back(Tokens[i]);
        }
    }
    return tags;
}

vector<Lexer::Token> Lexer::getOpeningTags() const{
    vector<Lexer::Token> tags;
    for(int i = 0; i < Tokens.size(); i++){
        if(Tokens[i].type == OPENING_BLOCK_TAG_NAME){
            tags.push_back(Tokens[i]);
        }
    }
    return tags;
}

void Lexer::process (const char &c) {
    switch (curr_token_type) {
    case END: {
        throw string("unexpected ending!");
    break; }
    case TEXT: {
        if(c == ' ' || c == '\t' || c == '\n') {
            if(buffer.empty()){
                break;
            }
        }
        if (c == '>')
            throw string("unexpected symbol: \">\"!");

        else if (c == '<') {
            if (!buffer.empty()) {
                add(buffer, TEXT);
                buffer.clear();
            }
            curr_token_type = OPENING_BLOCK_TAG_NAME | CLOSING_BLOCK_TAG_NAME | EMPTY_TAG_NAME | COMMENT | MACRO_TAG;//000 001 111 100

        } else {
            
            buffer.push_back(c);

        }
    break; }
    case OPENING_BLOCK_TAG_NAME: {
        throw string("error!");
    break; }
    case CLOSING_BLOCK_TAG_NAME: {
        if (c == '<')
            throw string("unexpected symbol: \"<\"!");
        else if (c == '/')
            throw string("unexpected symbol: \"<\"!");
        else if (c == '!')
            throw string("unexpected symbol: \"!\"!");
        else if (c == '?')
            throw string("unexpected symbol: \"?\"!");
        else if (c == ' ')
            throw string("unexpected symbol: \" \"!");
        else if (c == '\t')
            throw string("unexpected symbol: \"\\t\"!");
        else if (c == '\n')
            throw string("unexpected symbol: \"\\n\"!");
        else if (c == '>') {
            
                    add(buffer, CLOSING_BLOCK_TAG_NAME);
                    buffer.clear();
                    curr_token_type = TEXT;
            
        } else
            buffer.push_back(c);
    break; }
    case EMPTY_TAG_NAME: {
        throw string("error!");
    break; }
    case COMMENT: {
        if (c == '>' && buffer.size() > 1 && buffer[buffer.size()-1] == '-' && buffer[buffer.size()-2] == '-') {
            add(buffer.substr(0, buffer.size()-2), COMMENT);
            buffer.clear();
            curr_token_type = TEXT;

        } else
            buffer.push_back(c);
    break; }
    case MACRO_TAG: {
        if (c == '>' && buffer[buffer.size()-1] == '?') {
            add(buffer.substr(0, buffer.size()-1), MACRO_TAG);
            buffer.clear();
            curr_token_type = TEXT;

        } else
            buffer.push_back(c);
    break; }
    case OPENING_BLOCK_TAG_NAME | CLOSING_BLOCK_TAG_NAME | EMPTY_TAG_NAME | COMMENT | MACRO_TAG: {
        if (c == '!') {
            if (buffer.empty()) {
                buffer.push_back('!');
                curr_token_type = EMPTY_TAG_NAME | COMMENT;//110 000
            } else
                throw string("unexpected symbol: \"!\"!");

        } else if (c == '?') {
            if (buffer.empty())
                curr_token_type = MACRO_TAG;
            else
                throw string("unexpected symbol: \"?\"!");
        } else if (c == '>') {
            if (buffer.empty())
                throw string("empty tag!");
            else {
                check_tag();
                //cout << "END OF TAG" << endl;
                curr_token_type = TEXT;
                //cout << "BUFF FOR CHECK: " << buffer_for_check << endl;
                buffer_for_check.clear();
            }
        } else if (c == '/') {
            if (buffer.empty()){
                curr_token_type = CLOSING_BLOCK_TAG_NAME;
            }
            else
            {
                check_tag();
            }

        } else if (c == ' ' || c == '\t' || c == '\n') {
            if (!buffer.empty()){
                add(buffer, OPENING_BLOCK_TAG_NAME);
                curr_token_type = ATTRIBUTE_NAME;
                buffer.clear();
            }//проверка после того как пошел пробел сразу после имени тега!!(переделать)
        } else {
            buffer.push_back(c);
            buffer_for_check.push_back(c);
        }
    break; }
    case EMPTY_TAG_NAME | COMMENT: {
        buffer.push_back(c);
        if (buffer == "!--") {
            curr_token_type = COMMENT;
            buffer.clear();
            return;
        } else if (c == ' ' || c == '\t' || c == '\n') {
            string buf = buffer.substr(0, buffer.size()-1);
            add(buf, EMPTY_TAG_NAME);
            buffer.clear();
            curr_token_type = ATTRIBUTE_NAME;

        }
    break; }
    case ATTRIBUTE_NAME: {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (buffer.empty()) {
                curr_token_type = ATTRIBUTE_NAME | UNQUOTED_ATTRIBUTE_VALUE | SINGLE_QUOTED_ATTRIBUTE_VALUE | DOUBLE_QUOTED_ATTRIBUTE_VALUE;
            }

        } else if (c == '=') {
            add(buffer, ATTRIBUTE_NAME);
            buffer.clear();
            curr_token_type = UNQUOTED_ATTRIBUTE_VALUE | SINGLE_QUOTED_ATTRIBUTE_VALUE | DOUBLE_QUOTED_ATTRIBUTE_VALUE;

        } else if (c == '<')
            throw string("unexpected symbol: \"<\"!");

        else if (c == '>') {
            if (!buffer.empty()) {
                add(buffer, ATTRIBUTE_NAME);
                buffer.clear();
            }
            //cout << "END OF TAG" << endl;
            curr_token_type = TEXT;

        } else if (c == '/') {
            if (!buffer.empty()) {
                add(buffer, ATTRIBUTE_NAME);
                buffer.clear();
                //cout << "EMPT" << endl;
            }
            else {
//                if(!buffer_for_check.empty()){
//                    cout << "THIS TAG: " << buffer_for_check << endl;
//                }
                buffer_for_check.clear();
            }

        } else if (c == '!') {
            throw string("unexpected symbol: \"!\"!");

        } else if (c == '?') {
            throw string("unexpected symbol: \"?\"!");

        } else
            buffer.push_back(c);
    break; }
    case ATTRIBUTE_NAME | UNQUOTED_ATTRIBUTE_VALUE | SINGLE_QUOTED_ATTRIBUTE_VALUE | DOUBLE_QUOTED_ATTRIBUTE_VALUE: {
        if (c != ' ' && c != '\t' && c != '\n') {
            if (c == '=')
                curr_token_type = UNQUOTED_ATTRIBUTE_VALUE | SINGLE_QUOTED_ATTRIBUTE_VALUE | DOUBLE_QUOTED_ATTRIBUTE_VALUE;

            else if (c == '<')
                throw string("unexpected symbol: \"<\"!");

            else if (c == '>') {
                if (!buffer.empty()) {
                    add(buffer, ATTRIBUTE_NAME);
                    buffer.clear();
                }
                //cout << "END OF TAG" << endl;
                curr_token_type = TEXT;

            } else if (c == '/') {
                if (!buffer.empty()) {
                    add(buffer, ATTRIBUTE_NAME);
                    buffer.clear();
                }
                if(!buffer_for_check.empty()){
                    //cout << "THIS TAG: " << buffer_for_check << endl;
                    for(int i = Tokens.size()-1; i >= 0; i--){
                        cout << Tokens[i].lexeme << endl;
                        if(Tokens[i].lexeme == buffer_for_check){
                            Tokens[i].type = EMPTY_TAG_NAME;
                            break;
                        }
                    }
                }
                buffer_for_check.clear();

            } else if (c == '!') {
                throw string("unexpected symbol: \"!\"!");

            } else if (c == '?') {
                throw string("unexpected symbol: \"?\"!");

            } else {
                curr_token_type = ATTRIBUTE_NAME;
                buffer.push_back(c);
            }
        }
    break; }
    case UNQUOTED_ATTRIBUTE_VALUE | SINGLE_QUOTED_ATTRIBUTE_VALUE | DOUBLE_QUOTED_ATTRIBUTE_VALUE: {
        if (c != ' ' && c != '\t' && c != '\n') {
            if (c == '\'') {
                if (buffer.empty())
                    curr_token_type = SINGLE_QUOTED_ATTRIBUTE_VALUE;
                else
                    throw string("unexpected symbol: \"'\"!");

            } else if (c == '"') {
                if (buffer.empty())
                    curr_token_type = DOUBLE_QUOTED_ATTRIBUTE_VALUE;
                else
                    throw string("unexpected symbol: \"\"\"!");

            } else if (c == '<') {
                throw string("unexpected symbol: \"<\"!");

            } else if (c == '>') {
                throw string("unexpected symbol: \">\"!");

            } else if (c == '/') {
                throw string("unexpected symbol: \"/\"!");

            } else if (c == '!') {
                throw string("unexpected symbol: \"!\"!");

            } else if (c == '?') {
                throw string("unexpected symbol: \"?\"!");

            } else if (buffer.empty()) {
                curr_token_type = UNQUOTED_ATTRIBUTE_VALUE;
                buffer.push_back(c);

            } else
                buffer.push_back(c);
        }
    break; }
    case UNQUOTED_ATTRIBUTE_VALUE: {
            if (c == ' ' || c == '\t' || c == '\n') {
                if (!buffer.empty()) {
                    add(buffer, UNQUOTED_ATTRIBUTE_VALUE);
                    buffer.clear();
                    curr_token_type = ATTRIBUTE_NAME;

                } else if (c == '<') {
                    throw string("unexpected symbol: \"<\"!");

                } else if (c == '>') {
                    add(buffer, UNQUOTED_ATTRIBUTE_VALUE);
                    buffer.clear();
                    curr_token_type = TEXT;
                    //cout << "END OF TAG" << endl;

                } else if (c == '/') {
                    add(buffer, UNQUOTED_ATTRIBUTE_VALUE);
                    buffer.clear();

                } else if (c == '!') {
                    throw string("unexpected symbol: \"!\"!");

                } else if (c == '?') {
                    throw string("unexpected symbol: \"?\"!");
                    
                }
                
            } else {
                buffer.push_back(c);
        }
    break; }
    case SINGLE_QUOTED_ATTRIBUTE_VALUE: {
        if (c == '\'') {
            add(buffer, SINGLE_QUOTED_ATTRIBUTE_VALUE);
            buffer.clear();
            curr_token_type = ATTRIBUTE_NAME;

        } else
            buffer.push_back(c);
    break; }
    case DOUBLE_QUOTED_ATTRIBUTE_VALUE: {
        if (c == '"') {
            add(buffer, DOUBLE_QUOTED_ATTRIBUTE_VALUE);
            buffer.clear();
            curr_token_type = ATTRIBUTE_NAME;

        } else
            buffer.push_back(c);
    break; }
    }
}

void Lexer::add (const string &lexeme, const Token_type &type) {
//    cout << "LEXEME: " << lexeme << endl;
//    cout << "TYPE: " << type << endl;
    Token TokenToPush;
    TokenToPush.lexeme = lexeme;
    TokenToPush.type = type;
    Tokens.push_back(TokenToPush);
    
}

void Lexer::check_tag () {
    add(buffer, OPENING_BLOCK_TAG_NAME);
    buffer.clear();
    curr_token_type = ATTRIBUTE_NAME;
}

void Lexer::disassemble (ifstream &file) {
    curr_token_type = 0;

    unsigned long line(1), pos(1);
    try {
        char c;
        curr_token_type = TEXT;

        while ((c = file.get()) != EOF) {
            if (c == '\n') {
                pos = 1;
                line++;
            } else
                pos++;
            process(c);
        }

        if (buffer.size() != 0) {
            if (!(curr_token_type | TEXT))
                throw string("text was expected!");
            add(buffer, TEXT);
            buffer.clear();
        }

        add("", END);
    } catch (const string &error) {
        throw string("lexer: " + to_string(line) + "," + to_string(pos) + ": " + error);
    }
}
