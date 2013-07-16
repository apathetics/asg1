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

/* Skip blanks and whitespace and comments. */
void skipblanks ()
{
    int c;
    int cc;
    while ((c = peekchar()) != EOF
             && (c == ' ' || c == '\n' || c == '\t' || c == '{' 
             || (c == '(' && (cc = peek2char()) == '*')))
    {
        if(c == '{')
        {
            do
            {
                getchar();
            }while ((c = peekchar()) != EOF && c != '}');
        }
        else if(c == '(')
        {
            do
            {
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
    }

TOKEN getstring (TOKEN tok)
  {
    }

TOKEN special (TOKEN tok)
  {
    }

/* Get and convert unsigned numbers of all types. */
TOKEN number (TOKEN tok)
  { long num;
    int  c, charval;
    num = 0;
    while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
      {   c = getchar();
          charval = (c - '0');
          num = num * 10 + charval;
        }
    tok->tokentype = NUMBERTOK;
    tok->datatype = INTEGER;
    tok->intval = num;
    return (tok);
  }

