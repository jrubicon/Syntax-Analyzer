//CPSC 323
//programmed by Justin Drouin
//October 30th, 2019
//Assignment 2
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <ctype.h>
#include <map>
#include <array>
#include <stack>
using namespace std;

enum Symbols {
//TERMINALS
IDENTIFIER, // id
INT,      // int 'i'
FLOAT,    // float 'f'
BOOL,     // bool 'b'
EQUALS,   // '='
ADD,      // '+'
SUB,      // '-'
MULT,     // '*'
DIV,      // '/'
LPAR,     // '('
RPAR,     // ')'
SEMI,     // ';'
LESS,     // '<'
GREAT,    // '>'
WHILE,    // 'w'
DO,       // 'd'
WHILEEND, // 'e'
S_EOS,     // $, in this case corresponds to '\0'

// NON TERMINALS
STATEMENT,
ASSIGNMENT,
DECLARATIVE,
MOREDEC,
TYPE,
EXPRESSION,
EXPRESSION_PRIME,
TERM,
TERM_PRIME,
FACTOR,

S_INVALID, //invalid

};


//token class
class token {
public:

  char Keywords[18][10] = {"int", "float", "bool", "if", "else", "then", "endif", "while", "whileend", "do",
   "doend", "for", "forend", "input", "output", "and", "or", "function"};
  string Oper = "*+-=/><%";
  string Separators = "'(){}[],.:;!";
  vector<string> Identifiers;
  int state;
  token();

// SYNTAX TABLE
  map< Symbols, map<Symbols, int> > TDPP;

/*         'id'   '='  '+'    '-'   '*'    '/'    '('   ')'   ';'    'int'     'float'      'bool'    '$'       'while'          'do' 'whileend' '<'   '>'
    S  |    A    | - |   -  |  -  |  -  |   -  |   -  |  -  |  -  |    D    |     D     |    D     |  -  |          D            | - |    -  |   -  |  -   |
    D  |    -    | - |   -  |  -  |  -  |   -  |   -  |  -  |  -  |Y id = E;| Y id = E; | Y id = E;|  -  |          M            | - |    -  |   -  |  -   |
    M  |    -    | - |   -  |  -  |  -  |   -  |   -  |  -  |  -  |    -    |     -     |    -     |  -  | while E do E whileend | - |    -  |   -  |  -   |
    Y  |    -    | - |   -  |  -  |  -  |   -  |   -  |  -  |  -  |   int   |   float   |   bool   |  -  |          -            | - |    -  |   -  |  -   |
    A  |  id = E | - |   -  |  -  |  -  |   -  |   -  |  -  |  -  |    -    |     -     |    -     |  -  |          -            | - |    -  |   -  |  -   |
    E  |   TE'   | - |   -  |  -  |  -  |   -  |  TE' |  -  |  -  |    -    |     -     |    -     |  -  |          -            | - |    -  |   -  |  -   |
    E' |    -    | - | +TE' | -TE'|  -  |   -  |   -  |  ε  |  ε  |    -    |     -     |    -     |  -  |          -            | ε |    ε  | <TE' | >TE' |
    T  |   FT'   | - |   -  |  -  |  -  |   -  |  FT' |  -  |  -  |    -    |     -     |    -     |  -  |          -            | - |    -  |   -  |  -   |
    T' |    -    | - |  ε   |  ε  | *FR |  /FR |   -  |  ε  |  ε  |    -    |     -     |    -     |  -  |          -            | ε |    ε  |   ε  |   ε  |
    F  |   id    | - |   -  |  -  |  -  |   -  |  (E) |  -  |  -  |    -    |     -     |    -     |  -  |          -            | - |    -  |   -  |   -  |
*/

  //SYNTAX VARIABLES AND FLAGS
  int type;
  bool error;
  int lineNUM;
  string sInput;
  //SYNTAX ANALYZER FUNCTIONS
  void tbletostack(int temp, ofstream& outfile);
  void mainloader(ofstream& outfile);
  Symbols translateCOL(char x);
  stack<Symbols> synstack;

//LEXER FUNCTIONS
  bool isKeyword(char temp[]);
  bool isOperator(char temp);
  bool isSeparator(char temp);
  int isValidIdentifier(char temp[], int state, int i);
  void lex(char str, ofstream& outfile,ifstream& infile);
};

token::token(){
  lineNUM = 1;
  state = 0;
//syntax table set
  TDPP[STATEMENT][IDENTIFIER] = 1;
  TDPP[STATEMENT][INT] = 11;
  TDPP[STATEMENT][FLOAT] = 11;
  TDPP[STATEMENT][BOOL] = 11;
  TDPP[STATEMENT][WHILE] = 11;

  TDPP[ASSIGNMENT][IDENTIFIER] = 2;

  TDPP[DECLARATIVE][INT] = 15;
  TDPP[DECLARATIVE][FLOAT] = 15;
  TDPP[DECLARATIVE][BOOL] = 15;
  TDPP[DECLARATIVE][WHILE] = 22;

  TDPP[MOREDEC][WHILE] = 19;

  TDPP[TYPE][INT] = 16;
  TDPP[TYPE][FLOAT] = 17;
  TDPP[TYPE][BOOL] = 18;

  TDPP[EXPRESSION][IDENTIFIER] = 3;
  TDPP[EXPRESSION][LPAR] = 3;

  TDPP[EXPRESSION_PRIME][ADD] = 4;
  TDPP[EXPRESSION_PRIME][SUB] = 5;
  TDPP[EXPRESSION_PRIME][RPAR] = 13;
  TDPP[EXPRESSION_PRIME][SEMI] = 13;
  TDPP[EXPRESSION_PRIME][WHILEEND] = 13;
  TDPP[EXPRESSION_PRIME][LESS] = 20;
  TDPP[EXPRESSION_PRIME][GREAT] = 21;

  TDPP[TERM][IDENTIFIER] = 6;
  TDPP[TERM][LPAR] = 6;

  TDPP[TERM_PRIME][ADD] = 13;
  TDPP[TERM_PRIME][SUB] = 13;
  TDPP[TERM_PRIME][MULT] = 7;
  TDPP[TERM_PRIME][DIV] = 8;
  TDPP[TERM_PRIME][RPAR] = 13;
  TDPP[TERM_PRIME][SEMI] = 13;
  TDPP[TERM_PRIME][WHILEEND] = 13;
  TDPP[TERM_PRIME][LESS] = 13;
  TDPP[TERM_PRIME][GREAT] = 13;

  TDPP[FACTOR][IDENTIFIER] = 10;
  TDPP[FACTOR][LPAR] = 9;

  TDPP[MOREDEC][WHILE] = 19;
}

//bool, checks for operator
bool token::isOperator(char temp){
  for(int i = 0; i < 8; i++){
      if(temp == Oper[i]){
        return true;
      }
  }
  return false;
}

//bool, checks for separators
bool token::isSeparator(char temp){
  for(int i = 0; i < 12; i++){
    if(temp == Separators.at(i)){
      return true;
    }
  }
  return false;
}

//bool, checks for keywords
bool token::isKeyword(char temp[]){
  	for(int i = 0; i < 18; i++){
  		if(strcmp(Keywords[i], temp) == 0){
        type = i;
  			return true;
  		}
  	}
  	return false;
}

int token::isValidIdentifier(char temp[], int state, int i){
//FSM table for checking identifiers for validity
int validIdentifier[3][4] = {{1,2,2,2},{1,1,1,2},{2,2,2,2}};

      for( int j = 0; j < 4; j++){
          switch(j){
            case 0:
                if(isalpha(temp[i])){
                  state = validIdentifier[state][0];
                  return state;
                  }
                  break;
            case 1:
                if(isdigit(temp[i])){
                  state = validIdentifier[state][1];
                  return state;
                  }
                  break;
            case 2:
                if(temp[i]=='$'){
                  state = validIdentifier[state][2];
                  return state;
                  }
                  break;
            case 3:

                  state = validIdentifier[state][3];
                  return state;
                  break;
            default: return state;
            }
   }
return state;
}

//main lexer function
void token::lex(char str, ofstream& outfile, ifstream& infile){
  int i = 0, j = 0, z = 0;
  char buff[20];
  char inputb[50];
  bool dbop = 0;
  char doperator[3];

  //flags for comments
  int flag = 0;

  while (!infile.eof()){
    type = 3;
      str = infile.get(); //get char from file
      if(str == '\n'){
        lineNUM++; //LINE NUMBER FOR ERRORS
        continue;
      }

      //if section is commented out; flags
      if ( str == '!'){
            flag+=1;
            flag = flag % 2;
            continue;
      }

      //check flag and ignores char until second flag is found by prev check.
      if(flag == 0){

            //section for keywords and identifiers
            if((str == ' ' || str == '\n' || str == '('|| str == ')' || str ==',' ||
                str == '{'|| str == '}' || str =='.' || str == 39 || str == ';' ||
                str == ':' || str == '!' || str == '[' || str == ']' || str =='+' ||
                str == '-' || str == '=' || str == '*' || str == '<' || str == '>') && (j != 0)){
                      buff[j] = '\0';
                      j = 0;

                      if(isKeyword(buff) == 1){
                          outfile << "Token: Keyword        Lexeme: " << buff<< "\n";
                        if(type == 0)
                          inputb[z] = 'i';
                        if(type == 1)
                          inputb[z] = 'f';
                        if(type == 2)
                          inputb[z] = 'b';
                        if(type == 7)
                          inputb[z] = 'w';
                        if(type == 8)
                          inputb[z] = 'e';
                        if(type == 9)
                          inputb[z] = 'd';
                        z++;
                        }
                      else{
                        state = 0;
                          //call isValidIdentifier function to run FSM and check if valid
                         for(int i = 0; i < strlen(buff); i++){
                            state = isValidIdentifier(buff, state, i);
                          }
                          //check final state
                          switch(state){
                            case 1: { outfile << "Token: Identifier     Lexeme: " << buff<<"\n";
                            inputb[z] = 'I';
                            z++;
                            string tempbuff(buff);
                              if(Identifiers.empty()){
                                    Identifiers.push_back(tempbuff);
                              }
                              else{
                                int itsadupe = 1;
                                  for(int q = 0; q <Identifiers.size(); q++){
                                    if(Identifiers.at(q) == tempbuff){
                                      itsadupe = 0;
                                      break;
                                    }
                                  }
                                  if(itsadupe == 1){
                                    Identifiers.push_back(tempbuff);
                                  }
                              }
                            break;
                            }
                            case 2: { outfile << "INVALID ID     =     " << buff<<"\n";
                            perror("Invalid ID");
                            break;
                            }
                            default: break;
                          }

                      }
              }

              if(isOperator(str) == 1){
                if(isOperator(infile.peek()) == 1){
                  doperator[0] = str;
                  doperator[1] = infile.get();
                  doperator[2] = '\0';
                  dbop = 1;
                  outfile << "Token: Operator       Lexeme: " << doperator <<"\n";

                }
                else
                  outfile << "Token: Operator       Lexeme: " << str <<"\n";
              }

              //section for seperators
              if(isSeparator(str) == 1){
                    outfile << "Token: Separator      Lexeme: " << str <<"\n";
              }
              //if char is alphanumeric or $, increments buffer index and adds char
              //used for identifiers
              if(isalnum(str) || str == '$'){
                    buff[j++] = str;
                    continue;
              }

            //FOR DOUBLE OPERATORS (++)
            if(dbop == 1){
              inputb[z] = doperator[0];
              inputb[z++] = doperator[1];
              dbop = 0;
              z++;
              continue;
            }

            if(str == ';' ){
              inputb[z] = str;
              inputb[z+1] = '$';
              inputb[z+2] = '\0';

              sInput.clear();
                  //CLEAN FEED FOR SYNTAX ANALYZER
                  for (int m = 0; m < 50; m++){
                    if(inputb[m] == '\0'){
                      break;
                    }
                    if(inputb[m] == 'I' || inputb[m] == '('|| inputb[m] == 'i' || inputb[m] == 'f' ||
                    inputb[m] == ')' || inputb[m] ==',' || inputb[m] == '{'|| inputb[m] == '}' ||
                    inputb[m] =='.' || inputb[m] == '$' || inputb[m] == ':' || inputb[m] == ';' ||
                    inputb[m] == '/' || inputb[m] == '[' || inputb[m] == ']' || inputb[m] =='+' ||
                    inputb[m] == '-' || inputb[m] == '=' || inputb[m] == '*' || inputb[m] == 'b' ||
                    inputb[m] == 'w' || inputb[m] == 'd' || inputb[m] == 'e' || inputb[m] == '<'||
                    inputb[m] == '>'){
                      sInput += inputb[m];
                    }
                  }

                  z = 0;

               //for testing
               //sInput = "w(I<I)dI+Ie;$";
              outfile << sInput << " : line " << lineNUM <<'\n';
              //outfile << sInput << " : " << sInput.length() <<'\n';

              error = 0;
              //SYNTAX ANALYZER
              mainloader(outfile);
              sInput.clear();

              continue;
            }

            inputb[z] = str;
            z++;
        }

}
return;
}




//SYNTAX FUNCTIONS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//DRIVER FOR SYNTAX TABLE
void token::mainloader(ofstream& outfile){
  int count = 0;
  //INITIALIZE STACK
  synstack.push(S_EOS);
  synstack.push(STATEMENT);
  while (synstack.size() > 0){
      //IF TOKEN == TOP OF STACK
      if( translateCOL(sInput.at(count)) == synstack.top()){
          count++;
          synstack.pop();
      }
      else {
        //TRAVERSE TABLE & GET RULE
      int tablestate = TDPP[synstack.top()][translateCOL(sInput.at(count))];
        tbletostack(tablestate, outfile);
    }
    if (error == 1){
      //CLEAR STACK
      while(!synstack.empty()){
        synstack.pop();
      }
      // ERROR HANDLING
      cout << "SYNTAX ERROR: Line "<< lineNUM << "\n";
      outfile << "SYNTAX ERROR: Line "<< lineNUM << "\n";
      return;
      }
  }
  outfile << "SYNTAX: ACCEPTED" << '\n';
}

//TRANSLATE FOR COLUMN
Symbols token::translateCOL(char x){
    switch(x){
      case '=':
      return EQUALS;
      break;
      case 'b':
      return BOOL;
      break;
      case 'f':
      return FLOAT;
      break;
      case 'i':
      return INT;
      break;
      case '<':
      return LESS;
      break;
      case '>':
      return GREAT;
      break;
      case 'w':
      return WHILE;
      break;
      case 'd':
      return DO;
      break;
      case 'e':
      return WHILEEND;
      break;
      case 'I':
      return IDENTIFIER;
      break;
      case '+':
      return ADD;
      break;
      case '-':
      return SUB;
      break;
      case '*':
      return MULT;
      break;
      case '/':
      return DIV;
      break;
      case '(':
      return LPAR;
      break;
      case ')':
      return RPAR;
      break;
      case ';':
      return SEMI;
      break;
      case '$':
      return S_EOS;
      break;
      default:
        error = 1;
      }
      return S_INVALID;
}

//GET TABLE CMMD
void token::tbletostack(int temp, ofstream& outfile){
  switch(temp){
    case 1:
      outfile << " S -> A | D\n";
      synstack.pop();
      synstack.push(ASSIGNMENT);
    break;
    case 2:
      outfile << " A -> id = E ;\n";
      synstack.pop();
      synstack.push(SEMI);
      synstack.push(EXPRESSION);
      synstack.push(EQUALS);
      synstack.push(IDENTIFIER);
    break;
    case 3:
      outfile << " E -> TE' \n";
      synstack.pop();
      synstack.push(EXPRESSION_PRIME);
      synstack.push(TERM);
    break;
    case 4:
      outfile << " E' -> +TE' | -TE' | <TE' | >TE' | ε \n";
      synstack.pop();
      synstack.push(EXPRESSION_PRIME);
      synstack.push(TERM);
      synstack.push(ADD);
    break;
    case 5:
      outfile << " E' -> +TE' | -TE' | <TE' | >TE' | ε \n";
      synstack.pop();
      synstack.push(EXPRESSION_PRIME);
      synstack.push(TERM);
      synstack.push(SUB);
    break;
    case 6:
      outfile << " T -> FT' \n";
      synstack.pop();
      synstack.push(TERM_PRIME);
      synstack.push(FACTOR);
    break;
    case 7:
      outfile << " T' -> *FT' | /FT' | ε \n";
      synstack.pop();
      synstack.push(TERM_PRIME);
      synstack.push(FACTOR);
      synstack.push(MULT);
    break;
    case 8:
      outfile << " T' -> *FT' | /FT' | ε \n";
      synstack.pop();
      synstack.push(TERM_PRIME);
      synstack.push(FACTOR);
      synstack.push(DIV);
    break;
    case 9:
      outfile << " F -> (E) | id \n";
      synstack.pop();
      synstack.push(RPAR);
      synstack.push(EXPRESSION);
      synstack.push(LPAR);
    break;
    case 10:
      outfile << " F -> (E) | id \n";
      synstack.pop();
      synstack.push(IDENTIFIER);
    break;
    case 11:
      outfile << " S -> A | D \n";
      synstack.pop();
      synstack.push(DECLARATIVE);
    break;
    case 15:
      outfile << " D -> Y id = E ; | M\n";
      synstack.pop();
      synstack.push(SEMI);
      synstack.push(EXPRESSION);
      synstack.push(EQUALS);
      synstack.push(IDENTIFIER);
      synstack.push(TYPE);
    break;
    case 16:
      outfile << " Y -> int | float | bool\n";
      synstack.pop();
      synstack.push(INT);
    break;
    case 17:
      outfile << " Y -> int | float | bool\n";
      synstack.pop();
      synstack.push(FLOAT);
    break;
    case 18:
      outfile << " Y -> int | float | bool\n";
      synstack.pop();
      synstack.push(BOOL);
    break;
    case 19:
      outfile << " M -> while (E) do E whileend ;\n";
      synstack.pop();
      synstack.push(SEMI);
      synstack.push(WHILEEND);
      synstack.push(EXPRESSION);
      synstack.push(DO);
      synstack.push(RPAR);
      synstack.push(EXPRESSION);
      synstack.push(LPAR);
      synstack.push(WHILE);
      break;
    case 20:
      outfile << " E' ->  +TE' | -TE' | <TE' | >TE' | ε \n";
      synstack.pop();
      synstack.push(EXPRESSION_PRIME);
      synstack.push(TERM);
      synstack.push(LESS);
      break;
    case 21:
      outfile << " E' -> +TE' | -TE' | <TE' | >TE' | ε \n";
      synstack.pop();
      synstack.push(EXPRESSION_PRIME);
      synstack.push(TERM);
      synstack.push(GREAT);
    case 22:
      outfile << " D -> Y id = E ; | M\n";
      synstack.pop();
      synstack.push(MOREDEC);
    break;
    case 13:
      outfile << " ε \n";
      synstack.pop();
    break;
    default:
          error = 1;
          break;
  }
}
