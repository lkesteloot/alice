
typedef union 
{
	button_struct *b;
	popup_struct *p;
	char *string;
} YYSTYPE;
extern YYSTYPE yylval;
# define MENU 257
# define POPUP 258
# define COLOR 259
# define BACKCOLOR 260
# define HIGHCOLOR 261
# define ACTION 262
# define TITLE 263
