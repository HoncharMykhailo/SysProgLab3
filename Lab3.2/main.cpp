#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <regex>
#include <stdlib.h>
#include <chrono>

using namespace std;



enum TokenType {
    NUMBER,                   // 0
    STRING_LITERAL,           // 1
    CHARACTER_LITERAL,        // 2
    PREPROCESSOR_DIRECTIVE,   // 3
    COMMENT,                  // 4
    RESERVED_WORD,            // 5
    OPERATOR,                 // 6
    PUNCTUATION,              // 7
    IDENTIFIER,               // 8
    UNKNOWN                   // 9
};

struct Token {
    string lexeme;
    TokenType type;
};



// Функція для зміни кольору
void setColor(char colorCode) {
    string command = "color ";
    command += colorCode;
    system(command.c_str());
}

void printTokensWithColor(const vector<Token>& tokens) {
    for (int i = 0;i<tokens.size();i++) {

        switch (tokens[i].type) {
            case NUMBER:
                setColor('2');
                break;
            case STRING_LITERAL:
                setColor('6');
                break;
            case CHARACTER_LITERAL:
                setColor('3');
                break;
            case PREPROCESSOR_DIRECTIVE:
                setColor('5');
                break;
            case COMMENT:
                setColor('8');
                break;
            case RESERVED_WORD:
                setColor('9');
                break;
            case OPERATOR:
                setColor('C');
                break;
            case PUNCTUATION:
                setColor('E');
                break;
            case IDENTIFIER:
                setColor('7');
                break;
            case UNKNOWN:
            default:
                setColor('4');
                break;
        }

        // Виводимо токен
        cout << tokens[i].lexeme << " ";
    }
    cout << endl;
}



class Lexer {
private:
    string source;
    unordered_map<string, TokenType> reservedWords = {
            {"int", RESERVED_WORD},
            {"float", RESERVED_WORD},
            {"char", RESERVED_WORD},
            {"if", RESERVED_WORD},
            {"else", RESERVED_WORD},
            {"while", RESERVED_WORD},
            {"return", RESERVED_WORD},
            {"for", RESERVED_WORD},
            {"void", RESERVED_WORD},
            {"using", RESERVED_WORD},
            {"namespace", RESERVED_WORD},
            {"main", RESERVED_WORD},
            {"string", RESERVED_WORD},
            {"std", RESERVED_WORD},
            {"endl", RESERVED_WORD}
    };



    unordered_map<char, TokenType> punctuation = {
            {';', PUNCTUATION},
            {',', PUNCTUATION},
            {'{', PUNCTUATION},
            {'}', PUNCTUATION},
            {'(', PUNCTUATION},
            {')', PUNCTUATION},
            {'[', PUNCTUATION},
            {']', PUNCTUATION},
            {'=', OPERATOR},
            {'+', OPERATOR},
            {'-', OPERATOR},
            {'*', OPERATOR},
            {'/', OPERATOR},
            {'%', OPERATOR},
            {'<', OPERATOR},
            {'>', OPERATOR},
            {'!', OPERATOR},
            {'&', OPERATOR},
            {'|', OPERATOR},
            {':', OPERATOR}
    };


    TokenType currentState = UNKNOWN;
    string currentLexeme;

public:
    Lexer(const string& src) : source(src) {}

    void analyze() {
        string::const_iterator it = source.begin();
        while (it != source.end()) {

           // cout<<"iter ";

            if (isspace(*it) || *it == '\n') {
               // cout<<"Space\n";
                ++it;
                continue;
            }
           // cout<< *it<<"\n";

            currentLexeme = "";

            switch (currentState) {
                case UNKNOWN:
                 //   cout<<"START UNKNOWN "<<*it<<"\n";
                    handleUnknown(it);
                    break;
                case IDENTIFIER:
                    handleIdentifier(it);
                    break;
                case NUMBER:
                    handleNumber(it);
                    break;
                case STRING_LITERAL:
                    handleStringLiteral(it);
                    break;
                case CHARACTER_LITERAL:
                    handleCharacterLiteral(it);
                    break;
                case PUNCTUATION:
                    handlePunctuation(it);
                    break;
                case PREPROCESSOR_DIRECTIVE:
                    handlePreprocessorDirective(it);
                    break;
                case COMMENT:
                    handleComment(it);
                    break;
                default:
                    break;
            }
        }
    }

    void printTokens() {
        for (const auto& token : tokens) {
            if(token.lexeme == " " || token.lexeme == "\n") continue;
            cout << "< " << token.lexeme << " , " << token.type << ">\n";
        }
    }

    vector<Token> tokens;
private:

    vector<string> declaredIdentifiers;  // Динамічний масив для збереження ідентифікаторів (змінних/функцій)

    vector<string> varTypes = {
            "int", "float", "void", "string"
    };

    bool isType(const string& identifier) {
        for (const auto& item : varTypes) {
            if (item == identifier) {
                return true;
            }
        }
        return false;
    }

    bool isDeclared(const string& identifier) {
        return find(declaredIdentifiers.begin(), declaredIdentifiers.end(), identifier) != declaredIdentifiers.end();
    }




    void handleUnknown(string::const_iterator& it) {

        if (isspace(*it) || *it == '\n' || *it == ' ') {
            ++it;
        }
        char firstChar = *it;

        if ((firstChar>=65 && firstChar<=90) || (firstChar>=97 && firstChar<=122) ||firstChar==95 ) {
            currentState = IDENTIFIER;
        } else if ((firstChar>=48 && firstChar<=57)) {
            currentState = NUMBER;
        } else if (firstChar == 34) {
            currentState = STRING_LITERAL;
        } else if (*it == '/' && (it + 1) != source.end() && (*(it + 1) == '/' || *(it + 1) == '*')) {
            currentState = COMMENT;
        } else if (firstChar == 39) {
            currentState = CHARACTER_LITERAL;
        } else if (firstChar == 35) {
            currentState = PREPROCESSOR_DIRECTIVE;
        } else if ((firstChar>=40 && firstChar<=47) || (firstChar>=58 && firstChar<=64)||(firstChar>=91 && firstChar<=95)|| (firstChar>=123 && firstChar<=126)) {
            currentState = PUNCTUATION;
        } else {
            currentState = UNKNOWN;
            string word;
            while (it != source.end() && (isalnum(*it) || *it == '_')) {
                word += *it++;
            }

            tokens.push_back({word, UNKNOWN});
        }
    }




    void handleIdentifier(string::const_iterator& it) {
        string identifier;
        while (it != source.end() && (isalnum(*it) || *it == '_')) {
            identifier += *it++;
        }
        if(isDeclared(identifier))
        {
           // cout<<identifier<<" var\n";
            tokens.push_back({identifier, IDENTIFIER});
            if (isalpha(*it) || *it == '_') {
                currentState = IDENTIFIER;
            } else if (isdigit(*it)) {
                currentState = NUMBER;
            } else if (*it == '"') {
                currentState = STRING_LITERAL;
            } else if (*it == '\'') {
                currentState = CHARACTER_LITERAL;
            } else if (ispunct(*it)) {
                currentState = PUNCTUATION;
            } else {
                currentState = UNKNOWN;
            }
            return;
        }


        // reserved
        if (reservedWords.find(identifier) != reservedWords.end()) {
            tokens.push_back({identifier, RESERVED_WORD});

            if (isspace(*it) || *it == '\n'|| *it == '\r') {
                ++it;
            }

            if (isalpha(*it) || *it == '_') {
              //  cout<<*it<<"isalpha\n";
                currentState = IDENTIFIER;
            } else if (isdigit(*it)) {
                currentState = NUMBER;
            } else if (*it == '"') {
                currentState = STRING_LITERAL;
            } else if (*it == '\'') {
                currentState = CHARACTER_LITERAL;
            } else if (ispunct(*it)) {
                currentState = PUNCTUATION;
            } else {
                currentState = UNKNOWN;
          //      cout<<"UNKNOWN"<<*it<<"\n";
            }

            if (isType(identifier)) {
           //     cout<<identifier<<" type\n";
                while (it != source.end() && isspace(*it)) ++it;

                if (isalpha(*it) || *it == '_') {
                    string varName;
                    while (it != source.end() && (isalnum(*it) || *it == '_')) {
                        varName += *it++;
                    }
                    if(reservedWords.find(varName) != reservedWords.end())
                    {
                        if (isspace(*it) || *it == '\n'|| *it == '\r') {
                            ++it;
                        }
                       // cout<<varName<<" reserved var\n";
                        tokens.push_back({varName, RESERVED_WORD});
                        if (isalpha(*it) || *it == '_') {
                            currentState = IDENTIFIER;
                        } else if (isdigit(*it)) {
                            currentState = NUMBER;
                        } else if (*it == '"') {
                            currentState = STRING_LITERAL;
                        } else if (*it == '\'') {
                            currentState = CHARACTER_LITERAL;
                        } else if (ispunct(*it)) {
                            currentState = PUNCTUATION;
                        } else {
                            currentState = UNKNOWN;
                      //      tokens.push_back({varName, UNKNOWN});
                          //  ++it;
                        }
                        return;
                    }


                    declaredIdentifiers.push_back(varName);
                    tokens.push_back({varName, IDENTIFIER});

                    if (isspace(*it) || *it == '\n'|| *it == '\r') {
                        ++it;
                    }

                    if (isalpha(*it) || *it == '_') {
                        currentState = IDENTIFIER;
                    } else if (isdigit(*it)) {
                        currentState = NUMBER;
                    } else if (*it == '"') {
                        currentState = STRING_LITERAL;
                    } else if (*it == '\'') {
                        currentState = CHARACTER_LITERAL;
                    } else if (ispunct(*it)) {
                        currentState = PUNCTUATION;
                    } else {
                        currentState = UNKNOWN;
                    //    tokens.push_back({varName, UNKNOWN});
                    //    ++it;
                    }
                }
                return;
            }
        }
        else
        {
            tokens.push_back({identifier, UNKNOWN});
        //    cout<<identifier<<" unknown\n";
            currentState = UNKNOWN;
        }
    }

    void handleNumber(string::const_iterator& it) {
        string number;
        while (it != source.end() && isdigit(*it)) {
            number += *it++;
        }
        tokens.push_back({number, NUMBER});
        if (isalpha(*it) || *it == '_') {
            currentState = IDENTIFIER;
        } else if (ispunct(*it)) {
            currentState = PUNCTUATION;
        } else {
            //   cout<<"UNKNOWN 2\n";
            currentState = UNKNOWN;
            ++it;
        }
    }

    void handleStringLiteral(string::const_iterator& it) {
        string strLiteral;
        ++it;  // Skip the starting "
        while (it != source.end() && *it != '"') {
            strLiteral += *it++;
        }
        if (it != source.end()) ++it;  // Skip the ending "
        tokens.push_back({strLiteral, STRING_LITERAL});
        currentState = UNKNOWN;
    }

    void handleCharacterLiteral(string::const_iterator& it) {
        string charLiteral;
        ++it;  // Skip the starting '
        while (it != source.end() && *it != '\'') {
            charLiteral += *it++;
        }
        if (it != source.end()) ++it;  // Skip the ending '
        tokens.push_back({charLiteral, CHARACTER_LITERAL});
        if (isalpha(*it) || *it == '_') {
            currentState = IDENTIFIER;
        } else if (isdigit(*it)) {
            currentState = NUMBER;
        } else if (ispunct(*it)) {
            currentState = PUNCTUATION;
        } else {
            //   cout<<"UNKNOWN 2\n";
            currentState = UNKNOWN;
            ++it;
        }
    }

    void handlePunctuation(string::const_iterator& it) {
      //  cout<<"punc "<< *it<<"\n";
        if ((it + 1) != source.end()) {
            string twoCharOp(2, *it);

          //  cout<<"2cop "<< twoCharOp<<"\n";

            // Перевірка на оператор "::"
            if (twoCharOp == "::" ||
                twoCharOp == "<<" ||
                twoCharOp == ">>" ||
                twoCharOp == ">=" ||
                twoCharOp == "<=" ||
                twoCharOp == "==" ||
                twoCharOp == "++" ||
                twoCharOp == "--" )
            {
                tokens.push_back({twoCharOp, OPERATOR});
            //    int temp = punctuation[*it];


                it += 2;
                if (isalpha(*it) || *it == '_') {
                    currentState = IDENTIFIER;
                } else if (isdigit(*it)) {
                    currentState = NUMBER;
                }else if (*it == '/' && (it + 1) != source.end() && (*(it + 1) == '/' || *(it + 1) == '*')) {
                    currentState = COMMENT;
                } else if (*it == '"') {
                    currentState = STRING_LITERAL;
                } else if (*it == '\'') {
                    currentState = CHARACTER_LITERAL;
                } else if (ispunct(*it)) {
                    currentState = PUNCTUATION;
                } else {
                 //   cout<<"UNKNOWN 2\n";
                    currentState = UNKNOWN;
               //     tokens.push_back({std::string(1, *it), UNKNOWN});
                    ++it;
                }
                return;
            }
            else
            {

                // cout<<"==============ONE============\n";
              //  cout<<*it<<" 1 "<<punctuation[*it]<<"\n";
                tokens.push_back({string(1, *it), punctuation[*it]});

                ++it;
                if (isalpha(*it) || *it == '_') {
                    currentState = IDENTIFIER;
                } else if (*it == '/' && (it + 1) != source.end() && (*(it + 1) == '/' || *(it + 1) == '*')) {
                    currentState = COMMENT;
                } else if (ispunct(*it)) {
                    currentState = PUNCTUATION;
                } else {
                  //  cout<<"UNKNOWN 1\n";
                    currentState = UNKNOWN;
                    ++it;
                }


                return;
            }
        }


    }

    void handlePreprocessorDirective(string::const_iterator& it) {
        string directive;
        while (it != source.end() && *it != '\n') {
            directive += *it++;
        }
  //     cout<<"PPDir "<<directive<<"\n";
        tokens.push_back({directive, PREPROCESSOR_DIRECTIVE});

        if (isalpha(*it) || *it == '_') {
            currentState = IDENTIFIER;
        } else if (*it == '\'') {
            currentState = CHARACTER_LITERAL;
        } else if (*it == '/' && (it + 1) != source.end() && (*(it + 1) == '/' || *(it + 1) == '*')) {
            currentState = COMMENT;
        }else if (*it == '#') {
            currentState = PREPROCESSOR_DIRECTIVE;
        } else {
            currentState = UNKNOWN;
            ++it;

        }

    }

    void handleComment(string::const_iterator& it) {
    //    cout<<"comment"<<"\n";
        string comment;
        if (*(it + 1) == '/') {
            while (it != source.end() && *it != '\n') {
                comment += *it++;
            }
        } else {
            comment += *it++;
            comment += *it++;
            while (it != source.end() && !(*it == '*' && (it + 1) != source.end() && *(it + 1) == '/')) {
                comment += *it++;
            }
            if (it != source.end()) {
                comment += *it++;
                comment += *it++;
            }
        }
        tokens.push_back({comment, COMMENT});
        if (isalpha(*it) || *it == '_') {
            currentState = IDENTIFIER;
        }else if (*it == '#') {
            currentState = PREPROCESSOR_DIRECTIVE;
        } else if (ispunct(*it)) {
            currentState = PUNCTUATION;
        } else {
            currentState = UNKNOWN;
            tokens.push_back({string(1, *it), UNKNOWN});
            ++it;
        }
    }


};

int main() {
    string code = R"(


#include <iostream>
#include <string>
using namespace std;
/*
   LoooooNG COMMENT
    very long comment
*/
int main() {
    // short comment
    int abc = 10;
    float b = 3.14;
    char symbol = 'A';
    string text = "Hello";

    abc = abc + 5 * b;

    std::cout << text << " World!" << std::endl;

    if (a == b) {
        std::cout << "a more then b" << std::endl;
    } else {
        std::cout << "a less or equal to b" << std::endl;
    }
    rtrhrtfdgfg
    return 0;
    return frgfggm;
}


    )";



    Lexer lexer(code);
    auto start = chrono::high_resolution_clock::now();
    lexer.analyze();
    auto end = chrono::high_resolution_clock::now();
    lexer.printTokens();


    chrono::duration<double, milli> duration = end - start;

    cout << "TIME: " << duration.count() << " ms" << endl;

    // printTokensWithColor(lexer.tokens);
    return 0;
}
