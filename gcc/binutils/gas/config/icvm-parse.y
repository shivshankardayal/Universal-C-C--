/* icvm.y -- bison grammar for icvm operand parsing
   Copyright (C) 1995, 96, 1997 Free Software Foundation, Inc.
   Written by Ken Raeburn and Ian Lance Taylor, Cygnus Support

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file holds a bison grammar to parse icvm operands.  The icvm
   has a complicated operand syntax, and gas supports two main
   variations of it.  Using a grammar is probably overkill, but at
   least it makes clear exactly what we do support.  */

%{

#include "as.h"
#include "tc-icvm.h"
#include "icvm-parse.h"

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror,
   etc), as well as gratuitiously global symbol names If other parser
   generators (bison, byacc, etc) produce additional global names that
   conflict at link time, then those parser generators need to be
   fixed instead of adding those names to this list. */

#define	yymaxdepth icvm_maxdepth
#define	yyparse	icvm_parse
#define	yylex	icvm_lex
#define	yyerror	icvm_error
#define	yylval	icvm_lval
#define	yychar	icvm_char
#define	yydebug	icvm_debug
#define	yypact	icvm_pact	
#define	yyr1	icvm_r1			
#define	yyr2	icvm_r2			
#define	yydef	icvm_def		
#define	yychk	icvm_chk		
#define	yypgo	icvm_pgo		
#define	yyact	icvm_act		
#define	yyexca	icvm_exca
#define yyerrflag icvm_errflag
#define yynerrs	icvm_nerrs
#define	yyps	icvm_ps
#define	yypv	icvm_pv
#define	yys	icvm_s
#define	yy_yys	icvm_yys
#define	yystate	icvm_state
#define	yytmp	icvm_tmp
#define	yyv	icvm_v
#define	yy_yyv	icvm_yyv
#define	yyval	icvm_val
#define	yylloc	icvm_lloc
#define yyreds	icvm_reds		/* With YYDEBUG defined */
#define yytoks	icvm_toks		/* With YYDEBUG defined */
#define yylhs	icvm_yylhs
#define yylen	icvm_yylen
#define yydefred icvm_yydefred
#define yydgoto	icvm_yydgoto
#define yysindex icvm_yysindex
#define yyrindex icvm_yyrindex
#define yygindex icvm_yygindex
#define yytable	 icvm_yytable
#define yycheck	 icvm_yycheck

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

/* Internal functions.  */

static enum icvm_register icvm_reg_parse PARAMS ((char **));
static int yylex PARAMS ((void));
static void yyerror PARAMS ((const char *));

/* The parser sets fields pointed to by this global variable.  */
static struct icvm_op *op;

%}

%union
{
  struct icvm_indexreg indexreg;
  enum icvm_register reg;
  struct icvm_exp exp;
  unsigned long mask;
  int onereg;
}

%token <reg> DR QR MR XR VR AR SP FP  FPR FPCR LPC ZAR ZDR LZPC CREG RR
%token <indexreg> INDEXREG
%type <exp> expression 
%token <exp> EXPR 


%%

 

/* An operand.  */

operand:
	  generic_operand
	| motorola_operand
	;


/* A generic operand.  */

generic_operand:
	  DR
		{
		  op->mode = MODE_r;
		  op->reg = $1;
		}
	|  QR
		{
		  op->mode = MODE_c;
		  op->reg = $1;
		}
	|  MR
		{
		  op->mode = MODE_m;
		  op->reg = $1;
		}
	|  VR
		{
		  op->mode = MODE_v;
		  op->reg = $1;
		}
	|  XR
		{
		  op->mode = MODE_x;
		  op->reg = $1;
		}
	| AR
		{
		  op->mode = AREG;
		  op->reg = $1;
		}
	| SP
		{
		  op->mode = MODE_s;
		  op->reg = STACKPOINTER;
		}
	| FP
		{
		  op->mode = MODE_b;
		  op->reg = FRAMEPOINTER;
		}
	| LPC
		{
		  op->mode = MODE_p;
		  op->reg = PC;
		}
	| FPR
		{
		  op->mode = MODE_F;
		  op->reg = $1;
		}
	| CREG
		{
		  op->mode = CONTROL;
		  op->reg = $1;
		}
	| expression
		{
		  op->mode = MODE_EorB;
		  op->disp = $1;
		}
	| '#' expression
		{
		  op->mode = MODE_IMMED;
		  op->disp = $2;
		}
	;

/* An operand in Motorola syntax.  This includes MRI syntax as well,
   which may or may not be different in that it permits commutativity
   of index and base registers, and permits an offset expressionession to
   appear inside or outside of the parentheses.  */

motorola_operand:
	  '(' DR ')'
		{
		  op->mode = MODE_a;
		  op->reg = $2;
		}

    |  '@' DR 
		{
		  op->mode = MODE_a;
		  op->reg = $2;
		}

	|   '(' expression ',' DR ',' DR  ')'
		{
		  op->mode = MODE_1;
		  op->disp = $2;
		  if($4<$6){
			  op->reg = $4;
			  op->index.reg = $6;
		  }else{
			  op->reg = $6;
			  op->index.reg = $4;
		  }
		}

	|   '(' DR ',' DR ')'
		{
		  op->mode = MODE_2;
		  if($2<$4){
		  	op->reg = $2;
		  	op->index.reg = $4;
		  }else{
		  	op->reg = $4;
		  	op->index.reg = $2;
          }
		}

	|   '(' '@' DR ',' DR ')'
		{
		  op->mode = MODE_2;
		  if($3<$5){
		  	op->reg = $3;
		  	op->index.reg = $5;
		  }else{
		  	op->reg = $5;
		  	op->index.reg = $3;

		  }	
		}

	| '(' DR  ',' '(' expression ',' FP ')'  ')'
		{
		  op->reg = $2;
		  op->disp = $5;
		  op->mode = MODE_G;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' '@' DR  ',' '(' expression ',' FP ')' ')'
		{
		  op->reg = $3;
		  op->disp = $6;
		  op->mode = MODE_G;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' '@' '(' expression ',' FP ')'  ',' DR  ')'
		{
		  op->reg = $9;
		  op->disp = $4;
		  op->mode = MODE_G;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}



	| '(' DR  ',' '(' expression ',' FP ')'  '@' ')'
		{
		  op->reg = $2;
		  op->disp = $5;
		  op->mode = MODE_H;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' '@' DR  ',' '(' expression ',' FP ')' '@' ')'
		{
		  op->reg = $3;
		  op->disp = $6;
		  op->mode = MODE_H;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' '@' '(' expression ',' FP ')'  ',' DR '@'  ')'
		{
		  op->reg = $9;
		  op->disp = $4;
		  op->mode = MODE_K;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}



	| '(' expression  ',' '(' expression ',' FP ')'  ',' DR  ')'
		{
		  op->reg = $10;
		  op->disp = $2;
		  op->disp1 = $5;
		  op->mode = MODE_V;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' expression  ',' DR ',' '(' expression ',' FP ')'   ')'
		{
		  op->reg = $4;
		  op->disp = $2;
		  op->disp1 = $7;
		  op->mode = MODE_V;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}


	| '(' expression  ',' DR ',' '(' expression ',' FP ')'  '@'  ')'
		{
		  op->reg = $4;
		  op->disp = $2;
		  op->disp1 = $7;
		  op->mode = MODE_W;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' expression  ',' '(' expression ',' FP ')'  ',' DR  '@' ')'
		{
		  op->reg = $10;
		  op->disp = $2;
		  op->disp1 = $5;
		  op->mode = MODE_Y;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}


	|   '(' DR ',' DR  '@' ')'
		{
		  op->mode = MODE_3;
		  op->reg = $2;
		  op->index.reg = $4;
		}

	|   '(' '@' DR ',' DR  '@' ')'
		{
		  op->mode = MODE_3;
		  op->reg = $3;
		  op->index.reg = $5;
		}

	|   '(' expression ',' DR ',' DR  '@' ')'
		{
		  op->mode = MODE_4;
		  op->reg = $4;
		  op->index.reg = $6;
		  op->disp = $2;
		}

	|  '(' expression ',' DR ')'
		{
		  op->mode = MODE_A;
		  op->reg = $4;
		  op->disp = $2;
		}

	|  '(' expression ',' MR ')'
		{
		  op->mode = MODE_M;
		  op->reg = $4;
		  op->disp = $2;
		}

	| '(' SP ')' '+'
		{
		  op->mode = MODE_AINC;
		  op->reg = 0;
		}

	| '-' '(' SP ')'
		{
		  op->mode = MODE_ADEC;
		  op->reg = 0;
		}




	| '(' expression ',' SP  ')'
		{
		  op->reg = 0;
		  op->disp = $2;
		  op->mode = SPDISP;
		}



	| '(' expression ',' FP  ')'
		{
		  op->reg = 0;
		  op->disp = $2;
		  op->mode = MODE_U;
		}


	|  '(' '@' expression ','  FP  ')'  
		{
		  op->reg = 0;
		  op->disp = $3;
		  op->mode = MODE_d;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	|  '@' '('  expression ','  FP  ')'  
		{
		  op->reg = 0;
		  op->disp = $3;
		  op->mode = MODE_d;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}


	| '(' expression ',' '(' expression ','  FP ')'  ',' '(' expression ',' FP ')'  ')'
		{
		  op->reg = 0;
		  op->disp = $2;
		  op->disp1 = $5;
		  op->disp2 = $11;
		  op->mode = MODE_5;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}
	| '('  '@' '(' expression ',' FP  ')'  ',' '(' expression ',' FP ')'  ')'
		{
		  op->reg = 0;
		  op->disp = $4;
		  op->disp1 = $10;
		  op->mode = MODE_6;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}

	| '(' '@' '(' expression ',' FP ')'  ',' '(' expression ',' FP ')'  '@' ')'
		{
		  op->reg = 0;
		  op->disp = $4;
		  op->disp1 = $10;
		  op->mode = MODE_7;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}


	| '(' expression ',' '(' expression ',' FP ')'  ',' '(' expression ','  FP ')'  '@'  ')'
		{
		  op->reg = 0;
		  op->disp = $2;
		  op->disp1 = $5;
		  op->disp2 = $11;
		  op->mode = MODE_8;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}
	| '('  expression ',''(' expression ','  FP ')'   ')'
		{
		  op->reg = 0;
		  op->disp = $2;
		  op->disp1 = $5;
		  op->mode = MODE_f;
		  op->index.reg = 0;
		  op->index.size = SIZE_UNSPEC;
		  op->index.scale = 0;
		}
    ;

expression:
	EXPR
		{$$=$1;}
 | '[' EXPR ']' EXPR 
		{$$= target_big_endian ? $2 : $4;}
 | EXPR '[' EXPR ']'  
		{$$= target_big_endian ? $3 : $1;}
 | '(' expression ')'
	{$$=$2;}
;
 

 

%%

/* The string to parse is stored here, and modified by yylex.  */

static char *str;

/* The original string pointer.  */

static char *strorig;

/* If *CCP could be a register, return the register number and advance
   *CCP.  Otherwise don't change *CCP, and return 0.  */

static enum icvm_register
icvm_reg_parse (ccp)
     register char **ccp;
{
  char *start = *ccp;
  char c;
  char *p;
  symbolS *symbolp;

  if (flag_reg_prefix_optional)
    {
      if (*start == REGISTER_PREFIX)
	start++;
      p = start;
    }
  else
    {
      if (*start != REGISTER_PREFIX)
	return 0;
      p = start + 1;
    }

  if (! is_name_beginner (*p))
    return 0;

  p++;
  while (is_part_of_name (*p) && *p != '.' && *p != ':' && *p != '*')
    p++;

  c = *p;
  *p = 0;
  symbolp = symbol_find (start);
  *p = c;

  if (symbolp != NULL && S_GET_SEGMENT (symbolp) == reg_section)
    {
      *ccp = p;
      return S_GET_VALUE (symbolp);
    }

  /* In MRI mode, something like foo.bar can be equated to a register
     name.  */
  while (flag_mri && c == '.')
    {
      ++p;
      while (is_part_of_name (*p) && *p != '.' && *p != ':' && *p != '*')
	p++;
      c = *p;
      *p = '\0';
      symbolp = symbol_find (start);
      *p = c;
      if (symbolp != NULL && S_GET_SEGMENT (symbolp) == reg_section)
	{
	  *ccp = p;
	  return S_GET_VALUE (symbolp);
	}
    }

  return 0;
}

/* The lexer.  */

static int
yylex ()
{
  enum icvm_register reg;
  char *s;
  int parens;
  int c = 0;
  int tail = 0;
  char *hold;


  if (*str == ' ')
    ++str;

  if (*str == '\0')
    return 0;

  /* Various special characters are just returned directly.  */
  switch (*str)
    {
    case '@':
    case '#':
    case '^':
    case '&':
    case ',':
    case ')':
    case '/':
    case '[':
    case ']':
    case '{':
    case '}':
    case '<':
    case '>':
      return *str++;
    case '+':
    case '*':
      if (str[1] == '\0' || str[1]==')')
	return *str++;
      break;
    case '-':
      /* A '-' can only appear in -(ar), rn-rn, or ar@-.  If it
         appears anywhere else, it must be a unary minus on an
         expression.  */
      if (str[1] == '\0')
	return *str++;
      s = str + 1;
      if (*s == '(')
	++s;
      if (icvm_reg_parse (&s) != 0)
	return *str++;
      break;
    case '(':
      /* A '(' can only appear in `(reg)', `(expr,...', `([', `@(', or
         `)('.  If it appears anywhere else, it must be starting an
         expression.  */
      if (str[1] == '['
	  || (str > strorig
	      && (str[-1] == '@'
		  || str[-1] == ')')))
	return *str++;
      s = str + 1;
      if (icvm_reg_parse (&s) != 0)
	return *str++;
      /* Check for the case of '(expression,...' by scanning ahead.  If we
         find a comma outside of balanced parentheses, we return '('.
         If we find an unbalanced right parenthesis, then presumably
         the '(' really starts an expressionession.  */
      parens = 0;
      for (s = str + 1; *s != '\0'; s++)
	{
	  if (*s == '(')
	    ++parens;
	  else if (*s == ')')
	    {
	      if (parens == 0)
		break;
	      --parens;
	    }
	  else if (*s == ',' && parens == 0)
	    {
	      /* A comma can not normally appear in an expressionession, so
		 this is a case of '(expression,...'.  */
	      return *str++;
	    }
	}
    }

  /* See if it's a register.  */

  reg = icvm_reg_parse (&str);
  if (reg != 0)
    {
      int ret;

      yylval.reg = reg;

    if (reg >= DATA0 && reg <= DATA7)
	  ret = DR;
    else if (reg >= QDATA0 && reg <= QDATA3)
	  ret = QR;
    else if (reg >= XDATA0 && reg <= XDATA15)
	  ret = XR;
    else if (reg >= MDATA0 && reg <= MDATA15)
	  ret = MR;
    else if (reg >= VDATA0 && reg <= VDATA15)
	  ret = VR;
    else if (reg==STACKPOINTER)
    ret = SP;
      else if (reg==FRAMEPOINTER)
    ret = FP;
      else if (reg >= ADDR0 && reg <= ADDR7)
	ret = AR;
      else if (reg >= FP0 && reg <= FP3)
	return FPR;
      else if (reg == FPI
	       || reg == FPS
	       || reg == FPC)
	return FPCR;
      else if (reg == PC)
	return LPC;
      else if (reg >= ZDATA0 && reg <= ZDATA7)
	ret = ZDR;
      else if (reg >= ZADDR0 && reg <= ZADDR7)
	ret = ZAR;
      else if (reg == ZPC)
	return LZPC;
      else
	return CREG;

      /* If we get here, we have a data or address register.  We
	 must check for a size or scale; if we find one, we must
	 return INDEXREG.  */

      s = str;

      if (*s != '.' && *s != ':' && *s != '*')
	return ret;

      yylval.indexreg.reg = reg;

      if (*s != '.' && *s != ':')
	yylval.indexreg.size = SIZE_UNSPEC;
      else
	{
	  ++s;
	  switch (*s)
	    {
	    case 'w':
	    case 'W':
	      yylval.indexreg.size = SIZE_WORD;
	      ++s;
	      break;
	    case 'l':
	    case 'L':
	      yylval.indexreg.size = SIZE_LONG;
	      ++s;
	      break;
	    default:
	      yyerror ("illegal size specification");
	      yylval.indexreg.size = SIZE_UNSPEC;
	      break;
	    }
	}

      yylval.indexreg.scale = 1;

      if (*s == '*' || *s == ':')
	{
	  expressionS scale;

	  ++s;

	  hold = input_line_pointer;
	  input_line_pointer = s;
	  expression (&scale);
	  s = input_line_pointer;
	  input_line_pointer = hold;

	  if (scale.X_op != O_constant)
	    yyerror ("scale specification must resolve to a number");
	  else
	    {
	      switch (scale.X_add_number)
		{
		case 1:
		case 2:
		case 4:
		case 8:
		  yylval.indexreg.scale = scale.X_add_number;
		  break;
		default:
		  yyerror ("invalid scale value");
		  break;
		}
	    }
	}

      str = s;

      return INDEXREG;
    }

  /* It must be an expressionession.  Before we call expressionession, we need to
     look ahead to see if there is a size specification.  We must do
     that first, because otherwise foo.l will be treated as the symbol
     foo.l, rather than as the symbol foo with a long size
     specification.  The grammar requires that all expressionessions end at
     the end of the operand, or with ',', '(', ']', ')'.  */

  parens = 0;
  for (s = str; *s != '\0'; s++)
    {
      if (*s == '(')
	{
	  if (parens == 0
	      && s > str
	      && (s[-1] == ')' || isalnum ((unsigned char) s[-1])))
	    break;
	  ++parens;
	}
      else if (*s == ')')
	{
	  if (parens == 0)
	    break;
	  --parens;
	}
      else if (parens == 0
	       && (*s == ',' || *s == ']' || *s=='}'))
	break;
    }

  yylval.exp.size = SIZE_UNSPEC;
  if (s <= str + 2
      || (s[-2] != '.' && s[-2] != ':'))
    tail = 0;
  else
    {
      switch (s[-1])
	{
	case 's':
	case 'S':
	case 'b':
	case 'B':
	  yylval.exp.size = SIZE_BYTE;
	  break;
	case 'w':
	case 'W':
	  yylval.exp.size = SIZE_WORD;
	  break;
	case 'l':
	case 'L':
	  yylval.exp.size = SIZE_LONG;
	  break;
	default:
	  break;
	}
      if (yylval.exp.size != SIZE_UNSPEC)
	tail = 2;
    }

#ifdef OBJ_ELF
  {
    /* Look for @PLTPC, etc.  */
    char *cp;

    yylval.exp.pic_reloc = pic_none;
    cp = s - tail;
    if (cp - 6 > str && cp[-6] == '@')
      {
	if (strncmp (cp - 6, "@PLTPC", 6) == 0)
	  {
	    yylval.exp.pic_reloc = pic_plt_pcrel;
	    tail += 6;
	  }
	else if (strncmp (cp - 6, "@GOTPC", 6) == 0)
	  {
	    yylval.exp.pic_reloc = pic_got_pcrel;
	    tail += 6;
	  }
      }
    else if (cp - 4 > str && cp[-4] == '@')
      {
	if (strncmp (cp - 4, "@PLT", 4) == 0)
	  {
	    yylval.exp.pic_reloc = pic_plt_off;
	    tail += 4;
	  }
	else if (strncmp (cp - 4, "@GOT", 4) == 0)
	  {
	    yylval.exp.pic_reloc = pic_got_off;
	    tail += 4;
	  }
      }
  }
#endif

  if (tail != 0)
    {
      c = s[-tail];
      s[-tail] = 0;
    }

  hold = input_line_pointer;
  input_line_pointer = str;

  expression (&yylval.exp.exp);


  str = input_line_pointer;
  input_line_pointer = hold;

  if (tail != 0)
    {
      s[-tail] = c;
      str = s;
    }

  return EXPR;
}

/* Parse an icvm operand.  This is the only function which is called
   from outside this file.  */

int
icvm_ip_op (s, oparg)
     char *s;
     struct icvm_op *oparg;
{
  memset (oparg, 0, sizeof *oparg);
  oparg->error = NULL;
  oparg->index.reg = ZDATA0;
  oparg->index.scale = 1;
  oparg->disp.exp.X_op = O_absent;
  oparg->odisp.exp.X_op = O_absent;

  str = strorig = s;
  op = oparg;

  return yyparse ();
}

/* The error handler.  */

static void
yyerror (s)
     const char *s;
{
  op->error = s;
}
