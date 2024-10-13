#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <chrono>

using namespace std;

enum TokenType {
    NUMBER,//--------------------0
    STRING_LITERAL, //-----------1
    CHARACTER_LITERAL,//---------2
    PREPROCESSOR_DIRECTIVE,//----3
    COMMENT,//-------------------4
    RESERVED_WORD,//-------------5
    OPERATOR,//------------------6
    PUNCTUATION,//---------------7
    IDENTIFIER,//----------------8
    UNKNOWN//--------------------9
};

struct Token {
    string lexeme;
    TokenType type;
};

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
        {"main", RESERVED_WORD}
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

vector<string> declaredIdentifiers;

vector<string> varTypes = {
        "int", "float", "void"
};

bool isType(const string& identifier) {
    return find(declaredIdentifiers.begin(), declaredIdentifiers.end(), identifier) != varTypes.end();
}

bool isDeclared(const string& identifier) {
    return find(declaredIdentifiers.begin(), declaredIdentifiers.end(), identifier) != declaredIdentifiers.end();
}

vector<Token> tokenize(const string& source) {
    vector<Token> tokens;
    string::const_iterator it = source.begin();

    while (it != source.end()) {
        if (isspace(*it)) {
            ++it;
            continue;
        }

        // comment
        if (*it == '/' && (it + 1) != source.end() && (*(it + 1) == '/' || *(it + 1) == '*')) {
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
            continue;
        }

        // PPDir
        if (*it == '#') {
            string directive;
            while (it != source.end() && *it != '\n') {
                directive += *it++;
            }
            tokens.push_back({directive, PREPROCESSOR_DIRECTIVE});
            continue;
        }

        // 1243
        if (isdigit(*it) || (*it == '-' && isdigit(*(it + 1)))) {
            string number;
            while (it != source.end() && (isdigit(*it) || *it == '.' || *it == 'e' || *it == 'E' || *it == '-' || *it == '+')) {
                number += *it++;
            }
            tokens.push_back({number, NUMBER});
            continue;
        }

        // ""''
        if (*it == '\"' || *it == '\'') {
            char quote = *it;
            string literal;
            literal += *it++;
            while (it != source.end() && *it != quote) {
                literal += *it++;
            }
            if (it != source.end()) {
                literal += *it++;
            }
            tokens.push_back({literal, (quote == '\"') ? STRING_LITERAL : CHARACTER_LITERAL});
            continue;
        }

        // id + reserved
        if (isalpha(*it) || *it == '_') {
            string identifier;
            while (it != source.end() && (isalnum(*it) || *it == '_')) {
                identifier += *it++;
            }

            //reserved
            if (reservedWords.find(identifier) != reservedWords.end()) {
                tokens.push_back({identifier, RESERVED_WORD});
            } else {
                // id
                if (isDeclared(identifier)) {
                    tokens.push_back({identifier, IDENTIFIER});
                } else {

                    tokens.push_back({identifier, UNKNOWN});
                }
            }
            continue;
        }

        // op
        if (punctuation.find(*it) != punctuation.end()) {
            // ::
            if ((it + 1) != source.end()) {
                string twoCharOp(1, *it);
                twoCharOp += *(it + 1);
                if (twoCharOp == "::" || twoCharOp == "<<" || twoCharOp == ">>" || twoCharOp == ">=" ||
                    twoCharOp == "<=" || twoCharOp == "==") {
                    tokens.push_back({twoCharOp, OPERATOR});
                    it += 2; // Пропускаємо обидва символи
                    continue;
                }
            }
            // ;
            tokens.push_back({string(1, *it), punctuation[*it]});
            ++it;
            continue;
        }

        tokens.push_back({string(1, *it), UNKNOWN});
        ++it;
    }

    return tokens;
}


void printTokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        if(token.lexeme == " " || token.lexeme == "\n") continue;
        cout << "< " << token.lexeme << " , " << token.type << " >" << endl;
    }
}

int main() {
    string sourceCode = R"(


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
    float b = 3.14f;
    char symbol = 'A';
    std::string text = "Hello";

    abc = abc + 5 * b;

    std::cout << text << " World!" << std::endl;

    if (a == b) {
        std::cout << "a more then b" << std::endl;
    } else {
        std::cout << "a less or equal to b" << std::endl;
    }
    rtrhrtfdgfg
 return hgchfghbf;
    return 0;
}


)";
    auto start = chrono::high_resolution_clock::now();
    vector<Token> tokens = tokenize(sourceCode);
    auto end = chrono::high_resolution_clock::now();
    printTokens(tokens);

    chrono::duration<double, milli> duration = end - start;

    cout << "TIME: " << duration.count() << " ms" << endl;
    return 0;
}
