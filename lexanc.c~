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


TOKEN createOperator(int opVal, TOKEN tok)
{
    tok->tokentype = OPERATOR;
    tok->whichval = opVal;
    return (tok);
}

TOKEN createDelimiter(int delimVal, TOKEN tok)
{
    tok->tokentype = DELIMITER;
    tok->whichval = delimVal - DELIMITER_BIAS;
    return (tok);
}

TOKEN createReserved(int resVal, TOKEN tok)
{
    tok->tokentype = RESERVED;
    tok->whichval = resVal - RESERVED_BIAS;
    return (tok);
}

TOKEN createIdentifier(char* identifier, TOKEN tok)
{
    tok->tokentype = IDENTIFIERTOK;
    strncpy(tok->stringval, identifier, 16); // should this be 15 or 16?
    return (tok);
}

TOKEN createString(char* word, TOKEN tok)
{
    tok->tokentype = STRINGTOK;
    strncpy(tok->stringval, word, 16); // should this be 15 or 16?
    return (tok);
}

/* Skip blanks, whitespace and comments. */
void skipblanks ()
{
    int c;
    int cc;
    while ((c = peekchar()) != EOF
             && (c == ' ' || c == '\n' || c == '\t' || c == '{' 
             || (c == '(' && (cc = peek2char()) == '*')))
    {
        if(c == '{'){
            do{
                getchar();
            }while ((c = peekchar()) != EOF && c != '}');
        }else if(c == '('){
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

        // move pointer to end of current comment or whitespace 
        if(c != EOF)
            getchar();
    }
}

/* Get identifiers and reserved words */
TOKEN identifier (TOKEN tok)
{
	  int c;
    int numericFlag = 0;
		int counter = 0;
		char word[16];
    do{
        word[counter++] = getchar();
				c = peekchar();
				if(CHARCLASS[c] == NUMERIC)
				    numericFlag = 1;
		}while(c != EOF && counter < 15 && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC));
		
		while(c != EOF && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) {
		    getchar();
				c = peekchar();}
		word[counter] = '\0';
		
		
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

// this function currently erases the doubled apostrophe in the stringval of tok
TOKEN getstring (TOKEN tok)
{
	  getchar();
		int c = peekchar();
		int cc = peek2char();
		int counter = 0;
		char word[16];
		//printf("Value of c is: %c", c);
    while(c != EOF)
		{
				if(c == '\'')
				{	
					  if(cc == '\'')
				        getchar();  // THIS COULD BE UN-DESIRED BEHAVIOR
						else
							  break;
				}
				// only write first 15 chars, eat any remaining
				if(counter < 15)
				    word[counter++] = getchar();
				else
					  getchar();
				c = peekchar();
				cc = peek2char();
		}
		getchar();
		word[counter] = '\0';
		tok = createString(word, tok);
}

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
			default: printf("Unrecognized symbol: %c \n", c);
		}
    getchar();
		return tok;
}

double dcl[100]; // decimal conversion list
int dclCreated = 0;

void createDCL(){
		dcl[0] = 1;
		int i = 1;
    for(; i < 100; i++)
		    dcl[i] = dcl[i-1] * .1;
}    

/* Get and convert unsigned numbers of all types. */
TOKEN number (TOKEN tok)
{   long lnum = 0;
    double dnum = 0;
    int  c, charval;
    int counter= 0;
		int nonZeroFlag = 0; // remains zero until a non-zero number is hit.
		int realNumFlag = 0; // will remain zero if token reps and int, will switch to 1 if it's a double
		// get primary number
    while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
    {   
			  c = getchar();
        charval = (c - '0');
				if(charval != 0)
				    nonZeroFlag = 1;
        lnum = lnum * 10 + charval;
				dnum = dnum * 10 + charval;
				if(nonZeroFlag = 1)
				    ++counter;
    }
    // get decimal value if present
    if((c = peekchar()) == '.' && CHARCLASS[peek2char()] == NUMERIC)
		{
			  if(dclCreated == 0) {
				    createDCL();
						dclCreated =1;
				}
			  realNumFlag = 1;
        getchar(); //eat the period
				counter = 1;
				double dcharval;
				while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
				{  
				    c = getchar();
						dcharval = (c - '0') * dcl[counter++];
						dnum += dcharval;
				}
		}
    tok->tokentype = NUMBERTOK;
		if(realNumFlag == 1){
		    tok->datatype = REAL;
		    tok->realval = dnum;
		}else{
        tok->datatype = INTEGER;
        tok->intval = lnum;
		}
    return (tok);
}

