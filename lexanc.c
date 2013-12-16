/* lexanc.c */
/* modified by Robert E Reed

/* Copyright (c) 2001 Gordon S. Novak Jr. and
   The University of Texas at Austin. */

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "lexan.h"

// assigns values to the given TOKEN
TOKEN createOperator(int opVal, TOKEN tok)
{
    tok->tokentype = OPERATOR;
    tok->whichval = opVal;
    return (tok);
}

// assigns values to the given TOKEN
TOKEN createDelimiter(int delimVal, TOKEN tok)
{
    tok->tokentype = DELIMITER;
    tok->whichval = delimVal - DELIMITER_BIAS;
    return (tok);
}

// assigns values to the given TOKEN
TOKEN createReserved(int resVal, TOKEN tok)
{
    tok->tokentype = RESERVED - RESERVED_BIAS;
    tok->whichval = resVal;
    return (tok);
}

// assigns values to the given TOKEN
TOKEN createIdentifier(char* identifier, TOKEN tok)
{
    tok->tokentype = IDENTIFIERTOK;
    strncpy(tok->stringval, identifier, 16); // should this be 15 or 16?
    return (tok);
}

// assigns values to the given TOKEN
TOKEN createString(char* word, TOKEN tok)
{
    tok->tokentype = STRINGTOK;
    strncpy(tok->stringval, word, 16); // should this be 15 or 16?
    return (tok);
}

// assigns values to the given TOKEN
/* Skip blanks, whitespace and comments. */
void skipblanks ()
{
    int c;
    int cc;
    while ((c = peekchar()) != EOF
             && (c == ' ' || c == '\n' || c == '\t' || c == '{' 
             || (c == '(' && (cc = peek2char()) == '*')))
    {
			  // remove bracket style comment
        if(c == '{'){
            do{
                getchar();
            }while ((c = peekchar()) != EOF && c != '}');
        }
        // remove (* style comment
        else if(c == '('){
				    getchar();
            do{
                getchar();
                c = cc;
                cc = peek2char();
            }while (cc != EOF 
                       && (c != '*' || cc != ')'));
            getchar();
            c = cc;
        }

        // move pointer to end of current comment or whitespace char 
        if(c != EOF)
            getchar();
    }
}

/*static char* reservedWords[] = { " ", "array", "begin", "case", "const", "do",
               "downto", "else", "end", "file", "for",
		           "function", "goto", "if", "label", "nil",
               "of", "packed", "procedure", "program", "record",
               "repeat", "set", "then", "to", "type",
		           "until", "var", "while", "with", "and", "or",
	             "not", "div", "mod", "in" };

							 */

/* Get identifiers and reserved words */
TOKEN identifier (TOKEN tok)
{
	  int c;
    int numericFlag = 0; // set to 1 if a number has been read, signaling that this is not a reserved word
		int counter = 0; // the number of symbols in this identifier
		char word[16]; // buffer for the current word
		int id = 1; // set to a non zero number if 
		
    do{
        word[counter++] = getchar(); 
				
				// peek at next char
				c = peekchar(); 
				if(CHARCLASS[c] == NUMERIC)
				    numericFlag = 1;
		}while(c != EOF && counter < 15 && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC));
		
		// eat remaining characters in identifier
		while(c != EOF && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) {
		    getchar();
				c = peekchar();}
				
		// end buffer string
		word[counter] = '\0';
		
/*		
		int valueFound = 0;
		for(; id < 25; id++){
		    if(strcmp(word, reservedWords[id]) == 0){
				    valueFound = 1;
						break;
				}
		}
		if(valueFound == 1){
		    if(id >= 20)
				    createOperator(id - 6, tok);
				else
					  createReserved(id, tok);
		}
		else
			  createIdentifier(word, tok);
		
*/

		
		// identifier is not a reserved word
		if(numericFlag == 1 || counter < 2 || counter > 9)
		    createIdentifier(word, tok);
		
		//check if reserved word
		else {
		    switch(counter){
					 case 2: 
					     if(strcmp(word, "do") == 0)
					         tok = createReserved(DO, tok);
						   else if(strcmp(word, "if") == 0)
								   tok = createReserved(IF, tok);
							 else if(strcmp(word, "of") == 0)
								   tok = createReserved(OF, tok);
							 else if(strcmp(word, "to") == 0)
								   tok = createReserved(TO, tok);
							 else if(strcmp(word, "or") == 0)
								   tok = createOperator(OROP, tok);
							 else if(strcmp(word, "in") == 0)
								   tok = createOperator(INOP, tok);
							 else 
								   tok = createIdentifier(word, tok);
							 break;
					 case 3: 
							 if(strcmp(word, "end") == 0)
								   tok = createReserved(END, tok);
							 else if(strcmp(word, "for") == 0)
								   tok = createReserved(FOR, tok);
							 else if(strcmp(word, "nil") == 0)
								   tok = createReserved(NIL, tok);
							 else if(strcmp(word, "set") == 0)
								   tok = createReserved(SET, tok);
							 else if(strcmp(word, "var") == 0)
								   tok = createReserved(VAR, tok);
							 else if(strcmp(word, "and") == 0)
								   tok = createOperator(ANDOP, tok);
							 else if(strcmp(word, "not") == 0)
								   tok = createOperator(NOTOP, tok);
							 else if(strcmp(word, "div") == 0)
								   tok = createOperator(DIVOP, tok);
							 else if(strcmp(word, "mod") == 0)
								   tok = createOperator(MODOP, tok);
							 else 
								   tok = createIdentifier(word, tok);
					     break;
					 case 4:
						   if(strcmp(word, "case") == 0)
								   tok = createReserved(CASE, tok);
							 else if(strcmp(word, "else") == 0)
								   tok = createReserved(ELSE, tok);
							 else if(strcmp(word, "file") == 0)
								   tok = createReserved(FILEFILE, tok);
							 else if(strcmp(word, "goto") == 0)
								   tok = createReserved(GOTO, tok);
							 else if(strcmp(word, "then") == 0)
								   tok = createReserved(THEN, tok);
							 else if(strcmp(word, "type") == 0)
								   tok = createReserved(TYPE, tok);
							 else if(strcmp(word, "with") == 0)
								   tok = createReserved(WITH, tok);
							 else 
								   tok = createIdentifier(word, tok);
							 break;
					 case 5:
						   if(strcmp(word, "array") == 0)
								   tok = createReserved(ARRAY, tok);
							 else if(strcmp(word, "begin") == 0)
								   tok = createReserved(BEGINBEGIN, tok);
							 else if(strcmp(word, "const") == 0)
								   tok = createReserved(CONST, tok);
							 else if(strcmp(word, "label") == 0)
								   tok = createReserved(LABEL, tok);
							 else if(strcmp(word, "until") == 0)
								   tok = createReserved(UNTIL, tok);
							 else if(strcmp(word, "while") == 0)
								   tok = createReserved(WHILE, tok);
							 else 
								   tok = createIdentifier(word, tok);
							 break;
					 default:
					     if(strcmp(word, "downto") == 0)
								   tok = createReserved(DOWNTO, tok);
							 else if(strcmp(word, "function") == 0)
								   tok = createReserved(FUNCTION, tok);
							 else if(strcmp(word, "packed") == 0)
								   tok = createReserved(PACKED, tok);
							 else if(strcmp(word, "procedure") == 0)
								   tok = createReserved(PROCEDURE, tok);
							 else if(strcmp(word, "program") == 0)
								   tok = createReserved(PROGRAM, tok);
							 else if(strcmp(word, "record") == 0)
								   tok = createReserved(RECORD, tok);
							 else if(strcmp(word, "repeat") == 0)
								   tok = createReserved(REPEAT, tok);
							 else 
								   tok = createIdentifier(word, tok);
							 break;
				}
		}
}

// generates the values of the current stringtok
TOKEN getstring (TOKEN tok)
{		
		int counter = 0; //  charater counter
		char word[16]; // buffer for the string value
		
	  getchar(); // eat the '
		int c = peekchar();
		int cc = peek2char();
    while(c != EOF)
		{
				if(c == '\'')
				{	
					  //treat double ' as a single ' in the string
					  if(cc == '\'')
				        getchar();
						// end string after a single '
						else
							  break;
				}
				// only write first 15 chars, eat any remaining
				if(counter < 15)
				    word[counter++] = getchar();
				else  // eat remaining characters
					  getchar();
				
				// set up for next iteration
				c = peekchar();
				cc = peek2char();
		}
		getchar(); // eat final '
		word[counter] = '\0';
		tok = createString(word, tok);
}

// generates the values of the current stringtok
TOKEN special (TOKEN tok)
{
    char c = peekchar();
    char cc = peek2char();
    switch(c)
		{
			case '+': tok = createOperator(PLUSOP, tok);
			    break;
			case '-': tok = createOperator(MINUSOP, tok);
				  break;
			case '*': tok = createOperator(TIMESOP, tok);
			    break;
			case '/': tok = createOperator(DIVIDEOP, tok);
			    break;
			case ':': if(cc == '=') {
					         tok = createOperator(ASSIGNOP, tok);
							     getchar();
					      }else
						       tok = createDelimiter(COLON, tok);
					      break;
			case '=': tok = createOperator(EQOP, tok);
			    break;
			case '<': if(cc == '>'){
				           tok = createOperator(NEOP, tok);
									 getchar();
								}else if(cc == '='){
									 tok = createOperator(LEOP, tok);
									 getchar();
								}else
									 tok = createOperator(LTOP, tok);
								break;
			case '>': if(cc == '='){
				           tok = createOperator(GEOP, tok);
								   getchar();
								}else
									 tok = createOperator(GTOP, tok);
								break;
			case '^': tok = createOperator(POINTEROP, tok);
		      break;
			case '.': if(cc == '.'){
				           tok = createDelimiter(DOTDOT, tok);
									 getchar();
								}else
				           tok = createOperator(DOTOP, tok);
								break;
			case ',': tok = createDelimiter(COMMA, tok);
			    break;
			case ';': tok = createDelimiter(SEMICOLON, tok);
			    break;
			case '(': tok = createDelimiter(LPAREN, tok);
			    break;
			case ')': tok = createDelimiter(RPAREN, tok);
			    break;
			case '[': tok = createDelimiter(LBRACKET, tok);
			    break;
			case ']': tok = createDelimiter(RBRACKET, tok);
			    break;
			// unnessicary error checking
			default: printf("Unrecognized symbol: %c \n", c);
		}
    getchar(); // eat the operator
		return tok;
}

// symbol table for float values with negative exponents
double dcl[100]; // negative decimal conversion list

// flag set to 1 if createDCL function has been called
int dclCreated = 0;

// function to create the symbol table 
void createDCL(){
		dcl[0] = 1;
		int i = 1;
    for(; i < 100; i++)
		    dcl[i] = dcl[i-1] * .1;
}    

// symbol table for float values with positive exponents
double pdcl[100]; // positive decimal conversion list

// flag set to 1 if createPDCL function has been called
int pdclCreated = 0;

// function to create the symbol table 
void createPDCL(){
		pdcl[0] = 1;
		int i = 1;
    for(; i < 100; i++)
		    pdcl[i] = pdcl[i-1] * 10;
}    

/* Get and convert unsigned numbers of all types. */
TOKEN number (TOKEN tok)
{   
	  int  c, charval;
		
		// potential values of the number being read
	  long lnum = 0; // integer values
    double dnum = 0; // 
		int expVal = 0;
		
		// counters
    int sigDigCounter= 0; // used to count the number of significant digits read
		int dclCounter = 1; // used to count decimal numbers read
		int firstSigDigLoc = 0; // used as a primary exponent for a float
		
		// flags 
		int nonZeroFlag = 0; // remains zero until a non-zero number is hit.
		int realNumFlag = 0; // will remain zero if token reps and int, will switch to 1 if it's a double
		int expSignFlag = 0; // zero if pos, 1 if neg
		int intErrorFlag = 0; // 1 if int can't be stored in 32 bits
		int floatErrorFlag = 0; // 1 if net exponent is not between -38 and 38
		
		// get digits before decimal or e
    while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
    {   
			  c = getchar();
        charval = (c - '0');
				if(charval != 0)
				    nonZeroFlag = 1;
        lnum = lnum * 10 + charval;
				if(nonZeroFlag == 1){
					  if(sigDigCounter >= 8)
						    charval = 0;
				    dnum = dnum * 10 + charval;
						sigDigCounter++;
				}
    }
    firstSigDigLoc = sigDigCounter - 1;
    
    
    // int error checking
    if(sigDigCounter > 10 || lnum < 0 || lnum > 2147483647)
		    intErrorFlag = 1;
    
		// create decimal conversion look-up tables, if not already created
	  if(dclCreated == 0) {
				createDCL();
			  dclCreated =1;
		}
		if(pdclCreated == 0){
			  createPDCL();
				pdclCreated = 1;
		}
    // get values after decimal, if present
    if((c = peekchar()) == '.' && CHARCLASS[peek2char()] == NUMERIC)
		{

			  realNumFlag = 1;
        getchar(); //eat the period
				double dcharval;
				while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
				{  
				    c = getchar();
						dcharval = (c - '0');
						if(dcharval != 0)
				        nonZeroFlag = 1;
						if(nonZeroFlag == 0)
						    firstSigDigLoc--;
						dcharval *= dcl[dclCounter++];
				    if(nonZeroFlag == 1 && sigDigCounter < 8){
				        ++sigDigCounter;
						    dnum += dcharval;
						}
				}
		}
		
		// move sig digits to the ones place
		if(firstSigDigLoc > 0)
		    dnum *= dcl[firstSigDigLoc];
		else if(firstSigDigLoc < 0)
			  dnum *= pdcl[-firstSigDigLoc];
				
		// get exponent if present
		if((c = peekchar()) == 'e' || c == 'E') 
		{
		    realNumFlag = 1;
			  getchar(); // eat the 'e'
			  
			  // eat sign char, if present
				c = peekchar();
				if(c == '+')
				    getchar();
				else if(c == '-'){
				    expSignFlag = 1;
						getchar();
				}
				
				// reset and reuse flags
				nonZeroFlag = 0;
				
				// get exponent value
				while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
        {   
			      c = getchar();
            charval = (c - '0');
				    if(charval != 0)
				        nonZeroFlag = 1;
						if(nonZeroFlag == 1){
				        expVal = expVal * 10 + charval;
						}
        }
        // produce net exponent value
        if(expVal < 0)
				    floatErrorFlag = 1;
        if(expSignFlag == 1)
				    expVal = -expVal;
		}
		
		// final error checking and token assignment
    tok->tokentype = NUMBERTOK;
		if(realNumFlag == 1){
			  // compute net expVale
			  expVal += firstSigDigLoc;
			  
				// print an error if out of bounds or compute final floating point value
		    if(expVal > 38 || expVal < -38 ||  floatErrorFlag == 1)
		    {
	          dnum = 0;
	          printf("Floating number out of range\n");
	      }else{
		        if(expVal > 0)
				        dnum *= pdcl[expVal];
				    else if(expVal < 0){
							  // wanted to use the dcl look up table, but produced crazy values with large neg exponents
							  // large negative exponents are more accurate if computed from dnum directly
						    int i = 0;
						    for(; i > expVal; i--) 
					          dnum *= .1;
						}
	      }
	      // store final float
		    tok->datatype = REAL;
		    tok->realval = dnum;
		}else{
			  // pring an error message if the int value is out of bounds
			  if(intErrorFlag == 1)
		    {
	          lnum = 0;
	          printf("Integer number out of range\n");
	      }
	      // store final float
        tok->datatype = INTEGER;
        tok->intval = lnum;
		}
    return (tok);
}

