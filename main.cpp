//CPSC 323
//programmed by Justin Drouin
//October 30th, 2019
//Assignment 2

#include "lexermap2.h"

int main(int argc,  char *argv[]){
//initialize lexer
token lexer;
char str;
//open input file & file for tokens and lexemes
ifstream infile (argv[1]);

ofstream outfile("tokenOutput.txt");

//check if file is open
if (infile.is_open()){
//pass class object and files to lexer

   lexer.lex(str, outfile, infile);

//close files
  infile.close();
  outfile.close();
}
else { cout << "Unable to open file\n"; }

return 0;
}
