/* KR-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -L KR-C -F ', 0, 0' -p -j1 -g -o -t -N is_reserved_word -k'1,4,$,7' gxx.gperf  */
/* Command-line: gperf -L KR-C -F ', 0, 0' -p -j1 -g -o -t -N is_reserved_word -k1,4,$,7 gplus.gperf  */
struct resword { const char *name; short token; enum rid rid;};

#define TOTAL_KEYWORDS 109
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 16
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 269
/* maximum key range = 266, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register char *str;
     register unsigned int len;
{
  static unsigned short asso_values[] =
    {
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
        5, 270, 270,   0, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270,   0, 270, 103,  15, 117,
       25,   0,  92,  18,   4,  76, 270,   2,   0,  96,
       23,  56,  31,   8,   3,  10,  12, 123,   4,   2,
        8,   6, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270, 270, 270, 270, 270,
      270, 270, 270, 270, 270, 270
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#endif
struct resword *
is_reserved_word (str, len)
     register char *str;
     register unsigned int len;
{
  static struct resword wordlist[] =
    {
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"else", ELSE, NORID,},
      {"", 0, 0},
      {"__real", REALPART, NORID},
      {"while", WHILE, NORID,},
      {"__real__", REALPART, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"xor", '^', NORID,},
      {"__restrict__", CV_QUALIFIER, RID_RESTRICT},
      {"true", CXX_TRUE, NORID,},
      {"__asm__", ASM_KEYWORD, NORID},
      {"", 0, 0},
      {"bool", TYPESPEC, RID_BOOL,},
      {"", 0, 0},
      {"try", TRY, NORID,},
      {"xor_eq", ASSIGN, NORID,},
      {"virtual", SCSPEC, RID_VIRTUAL,},
      {"", 0, 0},
      {"__restrict", CV_QUALIFIER, RID_RESTRICT},
      {"", 0, 0}, {"", 0, 0},
      {"new", NEW, NORID,},
      {"extern", SCSPEC, RID_EXTERN,},
      {"short", TYPESPEC, RID_SHORT,},
      {"delete", DELETE, NORID,},
      {"switch", SWITCH, NORID,},
      {"", 0, 0},
      {"__alignof__", ALIGNOF, NORID},
      {"", 0, 0},
      {"this", THIS, NORID,},
      {"not_eq", EQCOMPARE, NORID,},
      {"not", '!', NORID,},
      {"", 0, 0},
      {"long", TYPESPEC, RID_LONG,},
      {"", 0, 0},
      {"private", VISSPEC, RID_PRIVATE,},
      {"typeid", TYPEID, NORID,},
      {"__extension__", EXTENSION, NORID},
      {"static_cast", STATIC_CAST, NORID,},
      {"double", TYPESPEC, RID_DOUBLE,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"__inline", SCSPEC, RID_INLINE},
      {"", 0, 0},
      {"__inline__", SCSPEC, RID_INLINE},
      {"reinterpret_cast", REINTERPRET_CAST, NORID,},
      {"void", TYPESPEC, RID_VOID,},
      {"", 0, 0}, {"", 0, 0},
      {"or", OROR, NORID,},
      {"", 0, 0},
      {"template", TEMPLATE, RID_TEMPLATE,},
      {"signed", TYPESPEC, RID_SIGNED,},
      {"", 0, 0}, {"", 0, 0},
      {"__complex__", TYPESPEC, RID_COMPLEX},
      {"", 0, 0},
      {"or_eq", ASSIGN, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"__typeof__", TYPEOF, NORID},
      {"__complex", TYPESPEC, RID_COMPLEX},
      {"export", SCSPEC, RID_EXPORT,},
      {"throw", THROW, NORID,},
      {"", 0, 0},
      {"__const__", CV_QUALIFIER, RID_CONST},
      {"__volatile", CV_QUALIFIER, RID_VOLATILE},
      {"bitor", '|', NORID,},
      {"__volatile__", CV_QUALIFIER, RID_VOLATILE},
      {"", 0, 0}, {"", 0, 0},
      {"do", DO, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"__signed__", TYPESPEC, RID_SIGNED},
      {"__const", CV_QUALIFIER, RID_CONST},
      {"", 0, 0},
      {"protected", VISSPEC, RID_PROTECTED,},
      {"register", SCSPEC, RID_REGISTER,},
      {"int", TYPESPEC, RID_INT,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"explicit", SCSPEC, RID_EXPLICIT,},
      {"", 0, 0},
      {"for", FOR, NORID,},
      {"__attribute", ATTRIBUTE, NORID},
      {"", 0, 0},
      {"__attribute__", ATTRIBUTE, NORID},
      {"", 0, 0}, {"", 0, 0},
      {"__imag__", IMAGPART, NORID},
      {"", 0, 0}, {"", 0, 0},
      {"false", CXX_FALSE, NORID,},
      {"sizeof", SIZEOF, NORID,},
      {"__signed", TYPESPEC, RID_SIGNED},
      {"typeof", TYPEOF, NORID,},
      {"__asm", ASM_KEYWORD, NORID},
      {"__label__", LABEL, NORID},
      {"__par256__", TYPESPEC, RID_PAR256},
      {"", 0, 0},
      {"volatile", CV_QUALIFIER, RID_VOLATILE,},
      {"typename", TYPENAME_KEYWORD, NORID,},
      {"and_eq", ASSIGN, NORID,},
      {"__par128__", TYPESPEC, RID_PAR128},
      {"__saturated__", TYPESPEC, RID_SATURATED},
      {"__imag", IMAGPART, NORID},
      {"case", CASE, NORID,},
      {"", 0, 0},
      {"friend", SCSPEC, RID_FRIEND,},
      {"__alignof", ALIGNOF, NORID},
      {"break", BREAK, NORID,},
      {"operator", OPERATOR, NORID,},
      {"char", TYPESPEC, RID_CHAR,},
      {"", 0, 0},
      {"__null", CONSTANT, RID_NULL},
      {"", 0, 0},
      {"and", ANDAND, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"goto", GOTO, NORID,},
      {"namespace", NAMESPACE, NORID,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"__wchar_t", TYPESPEC, RID_WCHAR  /* Unique to ANSI C++ */,},
      {"class", AGGR, RID_CLASS,},
      {"", 0, 0},
      {"const", CV_QUALIFIER, RID_CONST,},
      {"static", SCSPEC, RID_STATIC,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"bitand", '&', NORID,},
      {"", 0, 0},
      {"struct", AGGR, RID_RECORD,},
      {"", 0, 0},
      {"compl", '~', NORID,},
      {"public", VISSPEC, RID_PUBLIC,},
      {"return", RETURN_KEYWORD, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"inline", SCSPEC, RID_INLINE,},
      {"default", DEFAULT, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"__typeof", TYPEOF, NORID},
      {"sigof", SIGOF, NORID		/* Extension */,},
      {"", 0, 0},
      {"signature", AGGR, RID_SIGNATURE	/* Extension */,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"using", USING, NORID,},
      {"if", IF, NORID,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"__sigof__", SIGOF, NORID		/* Extension */,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"__signature__", AGGR, RID_SIGNATURE	/* Extension */,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"enum", ENUM, NORID,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"asm", ASM_KEYWORD, NORID,},
      {"typedef", SCSPEC, RID_TYPEDEF,},
      {"", 0, 0}, {"", 0, 0},
      {"mutable", SCSPEC, RID_MUTABLE,},
      {"union", AGGR, RID_UNION,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"float", TYPESPEC, RID_FLOAT,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"auto", SCSPEC, RID_AUTO,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"unsigned", TYPESPEC, RID_UNSIGNED,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"catch", CATCH, NORID,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"continue", CONTINUE, NORID,},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"const_cast", CONST_CAST, NORID,},
      {"", 0, 0}, {"", 0, 0},
      {"dynamic_cast", DYNAMIC_CAST, NORID,}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
