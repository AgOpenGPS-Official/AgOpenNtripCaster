/* match.c
 * - wildcard matching functions
 *
 * Copyright (c) 2003
 * German Federal Agency for Cartography and Geodesy (BKG)
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Informatik Centrum Dortmund http://www.icd.de
 *
 * The BKG disclaims any liability nor responsibility to any person or entity
 * with respect to any loss or damage caused, or alleged to be caused,
 * directly or indirectly by the use and application of the NTRIP technology.
 *
 * For latest information and updates, access:
 * http://igs.ifag.de/index_ntrip.htm
 *
 * Georg Weber
 * BKG, Frankfurt, Germany, June 2003-06-13
 * E-mail: euref-ip@bkg.bund.de
 *
 * Based on the GNU General Public License published Icecast 1.3.12
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "avl.h"
#include "ntripcastertypes.h"
#include "sourcetable.h"
#include "match.h"
#include "memory.h"
#include "parser.h"
#include "utility.h"
#include "ntripcaster_string.h"

/* The quoting character -- what overrides wildcards (do not undef)    */
#define QUOTE '\\'

/* The "matches ANYTHING" wildcard (do not undef)                      */
#define WILDS '*'

/* The "matches ANY NUMBER OF NON-SPACE CHARS" wildcard (do not undef) */
#define WILDP '%'

/* The "matches EXACTLY ONE CHARACTER" wildcard (do not undef)         */
#define WILDQ '?'

/* The "matches AT LEAST ONE SPACE" wildcard (undef me to disable!)    */
#define WILDT '~'

#undef tolower
#define tolower(c) tolowertab[c]
static unsigned char tolowertab[] =
{
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
   0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
   0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
   0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
   0x40, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
   'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
   'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
   'x', 'y', 'z', 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
   0x60, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
   'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
   'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
   'x', 'y', 'z', 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
   0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
   0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
   0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
   0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
   0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
   0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
   0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
   0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
   0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
   0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/*========================================================================*
 * Features:  Forward, case-insensitive, ?, *, %, ~(optional)             *
 * Best use:  Generic string matching, such as in IrcII-esque bindings    *
 *========================================================================*/
int
wild_match(register unsigned const char *m, register unsigned const char *n)
{
   unsigned const char *ma = m, *lsm = 0, *lsn = 0, *lpm = 0, *lpn = 0;
   int match = 1, saved = 0;
   register unsigned int sofar = 0;
#ifdef WILDT
   int space;
#endif

   /* take care of null strings (should never match) */
   if ((m == 0) || (n == 0) || (!*n))
      return NOMATCH;
   /* (!*m) test used to be here, too, but I got rid of it.  After all,
      If (!*n) was false, there must be a character in the name (the
      second string), so if the mask is empty it is a non-match.  Since
      the algorithm handles this correctly without testing for it here
      and this shouldn't be called with null masks anyway, it should be
      a bit faster this way */

   while (*n) {
      /* Used to test for (!*m) here, but this scheme seems to work better */
#ifdef WILDT
      if (*m == WILDT) {  /* Match >=1 space       */
   space = 0;   /* Don't need any spaces */
   do {
      m++;
      space++;
   }      /* Tally 1 more space ... */
   while ((*m == WILDT) || (*m == ' '));  /*  for each space or ~  */
   sofar += space;  /* Each counts as exact  */
   while (*n == ' ') {
      n++;
      space--;
   }      /* Do we have enough?    */
   if (space <= 0)
      continue;   /* Had enough spaces!    */
      }
      /* Do the fallback       */
      else {
#endif
   switch (*m) {
   case 0:
      do
         m--;   /* Search backwards      */
      while ((m > ma) && (*m == '?'));  /* For first non-? char  */
      if ((m > ma) ? ((*m == '*') && (m[-1] != QUOTE)) : (*m == '*'))
         return MATCH;  /* nonquoted * = match   */
      break;
   case WILDP:
      while (*(++m) == WILDP);  /* Zap redundant %s      */
      if (*m != WILDS) {  /* Don't both if next=*  */
         if (*n != ' ') { /* WILDS can't match ' ' */
      lpm = m;
      lpn = n;  /* Save % fallback spot  */
      saved += sofar;
      sofar = 0;  /* And save tally count  */
         }
         continue;  /* Done with %           */
      }
      /* FALL THROUGH */
   case WILDS:
      do
         m++;   /* Zap redundant wilds   */
      while ((*m == WILDS) || (*m == WILDP));
      lsm = m;
      lsn = n;
      lpm = 0;    /* Save * fallback spot  */
      match += (saved + sofar); /* Save tally count      */
      saved = sofar = 0;
      continue;   /* Done with *           */
   case WILDQ:
      m++;
      n++;
      continue;   /* Match one char        */
   case QUOTE:
      m++;    /* Handle quoting        */
   }

   if (tolower(*m) == tolower(*n)) {  /* If matching           */
      m++;
      n++;
      sofar++;
      continue;   /* Tally the match       */
   }
#ifdef WILDT
      }
#endif
      if (lpm) {    /* Try to fallback on %  */
   n = ++lpn;
   m = lpm;
   sofar = 0;   /* Restore position      */
   if ((*n | 32) == 32)
      lpm = 0;    /* Can't match 0 or ' '  */
   continue;    /* Next char, please     */
      }
      if (lsm) {    /* Try to fallback on *  */
   n = ++lsn;
   m = lsm;   /* Restore position      */
   /* Used to test for (!*n) here but it wasn't necessary so it's gone */
   saved = sofar = 0;
   continue;    /* Next char, please     */
      }
      return NOMATCH;   /* No fallbacks=No match */
   }
   while ((*m == WILDS) || (*m == WILDP))
      m++;      /* Zap leftover %s & *s  */
   return (*m) ? NOMATCH : MATCH; /* End of both = match   */
}

int match_sourcetable_entry(list_t *expression_list, sourcetable_entry_t *entry) {
  expression_t *root;
  sourcetable_field_t *field;
  list_enum_t *en;
  int res;

  if(entry->type == unknown_e)
      return 0;

  en = list_get_enum(expression_list);
  field = (sourcetable_field_t *)entry->fields;

  while (!((field->type == unknown_type_e) && (field->name == NULL))) {
    root = list_next(en);
    if (root != NULL) {
      res = match_sourcetable_field(root, field);

//printf("match_sourcetable_entry: match_sourcetable_field with field of type %d returned [%d]\r\n", field->type, res);

      if (res != 1) {
        nfree(en);
        return 0;
      }
    }
    field++;
  }

  nfree(en);
  return 1;
}

int match_sourcetable_field(expression_t *root, sourcetable_field_t *field) {
  int res;

  if (root == NULL) return 0;

//printf("match_sourcetable_field: root type %d, field type %d\r\n", root->type, field->type);

  switch (root->type) {
    case LOGIC_AND:
      if (match_sourcetable_field(root->left, field) == 0) return 0;
      if (match_sourcetable_field(root->right, field) == 0) return 0;
      return 1;
    case LOGIC_OR:
      if (match_sourcetable_field(root->left, field) == 1) return 1;
      if (match_sourcetable_field(root->right, field) == 1) return 1;
      return 0;
    case LOGIC_NOT:
      if (match_sourcetable_field(root->left, field) == 1) return 0;
      return 1;
    case EQUAL:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res == 0) return 1;
      }
      return 0;

    case NOT_EQUAL:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res == 0) return 0;
        else return 1;
      }
      return 0;
    case LESS:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res > 0) return 1;
      }
      return 0;
    case GREATER:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res < 0) return 1;
      }
      return 0;
    case LESS_EQUAL:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res < 0) return 0;
        else return 1;
      }
      return 0;
    case GREATER_EQUAL:
      if (((expression_t *)root->left)->type == CONSTANT) {
        res = compare_entry_fields(((sourcetable_field_t *)((expression_t *)root->left)->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

        if (res > 0) return 0;
        else return 1;
      }
      return 0;
    case CONSTANT:
      res = compare_entry_fields(((sourcetable_field_t *)root->left), field);

//printf("expression_match: compare_entry_fields returned %d\r\n", res);

      if (res == 0) return 1;
      return 0;
    default:
      break;
  }

  return 0;
}

/* creates a list of filter expression trees. Contains a NULL element for each
 * empty sourcetable filter field ("...;;..."). ajd */
list_t *get_filter_expression_list(char *filter,int matchonly) {
  char line[BUFSIZE];
  char expr[BUFSIZE];
  expression_t *root;
  list_t *l = list_create();

// printf("get_filter_expression_list: filter [%s] %d\r\n", filter,matchonly);

  strcpy(line, filter);

  while ((splitc(expr, line, ';') != NULL) && (l->size < 25)) {
    if (expr[0] != '\0') {

//printf("get_filter_expression_list: adding expression [%s]\r\n", expr);

      root = parse_expression(expr);
    } else {

//printf("get_filter_expression_list: adding NULL expression\r\n");

      root = NULL;
    }
    list_add(l, root);
  }

  if (line[0] != '\0')
    root = parse_expression(line);
  else
    root = NULL;
  list_add(l, root);

  return l;
}

/* parses an expression represented by a string and
 * returns the root of the parse tree. ajd */
expression_t *parse_expression(char *expr) {
  void *pParser;
  token_t *sToken;
  expression_t root;
  list_t *tokenList;
  list_enum_t *listEnum;

//printf("parse_expression: expression [%s]\r\n", expr);

  root.left = NULL;

/* must be freed. ajd */
  tokenList = get_token_list(expr);
  listEnum = list_get_enum(tokenList);
  pParser = ParseAlloc( parser_malloc );
//  ParseTrace(stdout, "ParseTrace: ");

  do {
    sToken = list_next(listEnum);
//printf("parse_expression: token %d value [%s]\r\n", sToken->type, sToken->value);
    Parse(pParser, sToken->type, sToken, &root);
  } while (sToken->type > 0);

  ParseFree(pParser, parser_free );
  nfree(listEnum);
  list_dispose_with_data(tokenList, dispose_token);

  return (expression_t *)root.left;
}

void dispose_parse_tree(expression_t *root) {

//printf("dispose_parse_tree: freeing expression [%d] left [%s] right [%s]\r\n", root->type, (root->left==NULL)?"null":"not null", (root->right==NULL)?"null":"not null");

  if (root != NULL) {
    if (root->type == CONSTANT) {
      if (root->left != NULL) dispose_entry_field((sourcetable_field_t *)root->left);
      if (root->right != NULL)
      {
        nfree(root->right);
      }
    } else {
      if (root->left != NULL) dispose_parse_tree((expression_t *)root->left);
      if (root->right != NULL) dispose_parse_tree((expression_t *)root->right);
    }
    nfree(root);
  }
}

list_t *get_token_list(char *expr) {
  list_t *l = list_create();
  token_t *sToken;
  char *p = expr;

  do {
    sToken = get_next_token(&p);
    list_add(l, sToken);
//printf("get_token_list: added token type [%d] value [%s]\r\n", sToken->type, (sToken->value != NULL)?sToken->value:"null");
  } while (sToken->type > 0);

  return l;
}

token_t *get_next_token(char **pp) {
  token_t *next = NULL;
  char valbuf[TOKEN_VALUE_LENGTH+1];
  register char *p = *pp;
  register int c = 0;
  int type;

  while (*p == ' ') p++;

  if (*p == '\0') {
    next = create_token(0, NULL);
  } else if (*p == '+') {
    next = create_token(LOGIC_AND, NULL);
    p++;
  } else if (*p == '|') {
    next = create_token(LOGIC_OR, NULL);
    p++;
  } else if (*p == '!') {
    if (*(p+1) == '=') {
      next = create_token(NOT_EQUAL, NULL);
      p++;
    } else
      next = create_token(LOGIC_NOT, NULL);
    p++;
  } else if (*p == '=') {
    next = create_token(EQUAL, NULL);
    if (*(p+1) == '=') p++;
    p++;
  } else if (*p == '>') {
    if (*(p+1) == '=') {
      next = create_token(GREATER_EQUAL, NULL);
      p++;
    } else
      next = create_token(GREATER, NULL);
    p++;
  } else if (*p == '<') {
    if (*(p+1) == '=') {
      next = create_token(LESS_EQUAL, NULL);
      p++;
    } else
      next = create_token(LESS, NULL);
    p++;
  } else if (*p == '(') {
    next = create_token(LEFT_PAREN, NULL);
    p++;
  } else if (*p == ')') {
    next = create_token(RIGHT_PAREN, NULL);
    p++;
  } else if (*p == '~') {
    next = create_token(APPROX, NULL);
    p++;
  } else if (isdigit(*p)) {
    type = INT_VALUE;
    valbuf[c] = *p;
    p++;
    c++;
    while ((isdigit(*p) || (*p == '.') || (*p == ',')) && (c < TOKEN_VALUE_LENGTH)) {
      if ((*p == '.') || (*p == ',')) type = REAL_VALUE;
      valbuf[c] = *p;
      p++;
      c++;
    }
    valbuf[c] = '\0';
    next = create_token(type, valbuf);
  } else {
    valbuf[c] = *p;
    p++;
    c++;
    while ((isalpha(*p) || isdigit(*p) || !((*p == '\0') || (*p == '=') || (*p == '~') || (*p == '+') || (*p == '|') || (*p == '!') || (*p == '>') || (*p == '<') || (*p == '(') || (*p == ')'))) && (c < TOKEN_VALUE_LENGTH)) {
      valbuf[c] = *p;
      p++;
      c++;
    }
    valbuf[c] = '\0';
    next = create_token(STRING, valbuf);
  }

  *pp = p;

  return next;
}

expression_t *create_expression(int type, void *left, void *right) {
  expression_t *new = nmalloc(sizeof(expression_t));

//printf("create_expression: creating expression [%d] left [%s] right [%s]\r\n", type, (left==NULL)?"null":"not null", (right==NULL)?"null":"not null");

  new->type = type;
  new->left = left;
  new->right = right;

  return new;
}

token_t *create_token(int type, char* value) {
  token_t *new = nmalloc(sizeof(token_t));

  new->type = type;
  if (value != NULL)
    new->value = nstrdup(value);
  else
    new->value = NULL;

  return new;
}

void dispose_token(token_t *t) {
//printf("dispose_token: freeing token type [%d] value [%s]\r\n", t->type, (t->value != NULL)?t->value:"null");
  if (t->value != NULL)
  {
    nfree(t->value);
  }
  nfree(t);
}
/*
void test_expression_match() {
  expression_t *root;
  sourcetable_field_t *field;
  int match = 0;
  char *exp, *test;

  exp = "TEST|FEST";
  test = "TEST";
  printf("Parsing expression %s\r\n", exp);
  root = parse_expression(exp);
  field = create_entry_field(string_e, NULL, test);
  printf("Testing %s\r\n", test);
  match = expression_match(root, field);
  printf("%s\r\n", (match==1)?"MATCH!":"NO MATCH!");
dispose_entry_field(field);
dispose_parse_tree(root);

  exp = "==12.7";
  test = "12.7";
  printf("Parsing expression %s\r\n", exp);
  root = parse_expression(exp);
  field = create_entry_field(real_e, NULL, test);
  printf("Testing %s\r\n", test);
  match = expression_match(root, field);
  printf("%s\r\n", (match==1)?"MATCH!":"NO MATCH!");

dispose_entry_field(field);
dispose_parse_tree(root);


  exp = "(>50&<60)|(>10&<20)";
  test = "33";
  printf("Parsing expression %s\r\n", exp);
  root = parse_expression(exp);
  field = create_entry_field(integer_e, NULL, test);
  printf("Testing %s\r\n", test);
  match = expression_match(root, field);
  printf("%s\r\n", (match==1)?"MATCH!":"NO MATCH!");

}
*/
