/* This file was automatically generated.  Do not edit! */
#define ParseTOKENTYPE  token_t *
#define ParseARG_PDECL , expression_t *root
void Parse(void *yyp,int yymajor,ParseTOKENTYPE yyminor ParseARG_PDECL);
void ParseFree(void *p,void(*freeProc)(void *));
void *ParseAlloc(void *(*mallocProc)(size_t));
#if !defined(NDEBUG)
void ParseTrace(FILE *TraceFILE,char *zTracePrompt);
#endif
#define ParseARG_STORE yypParser->root  = root
#define ParseARG_FETCH  expression_t *root  = yypParser->root
#define ParseARG_SDECL  expression_t *root ;
#define REAL_VALUE                     15
#define INT_VALUE                      14
#define STRING                         13
#define RIGHT_PAREN                    12
#define LEFT_PAREN                     11
#define APPROX                         10
#define LESS_EQUAL                      9
#define GREATER_EQUAL                   8
#define LESS                            7
#define GREATER                         6
#define NOT_EQUAL                       5
#define EQUAL                           4
#define LOGIC_NOT                       3
#define LOGIC_OR                        2
#define LOGIC_AND                       1
#define INTERFACE 0
