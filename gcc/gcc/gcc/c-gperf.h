/* KR-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -L KR-C -F ', 0, 0' -p -j1 -i 1 -g -o -t -N is_reserved_word -k'1,2,3,6,$' c-parse.gperf  */
/* Command-line: gperf -L KR-C -F ', 0, 0' -p -j1 -i 1 -g -o -t -N is_reserved_word -k* c-parse.gperf  */ 
struct resword { const char *name; short token; enum rid rid; };

#define TOTAL_KEYWORDS 86
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 20
#define MIN_HASH_VALUE 11
#define MAX_HASH_VALUE 218
/* maximum key range = 208, duplicates = 0 */

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
  static unsigned char asso_values[] =
    {
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219,   8,
        7, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219,  22, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219,   1, 219,  22,  20,  74,
       13,   1,  25,  75,   3,   7, 219,   2,  79,  45,
        1,   2,  61, 219,  14,  15,   1,  47,  42,   6,
        4,   1,   2, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219, 219, 219, 219, 219,
      219, 219, 219, 219, 219, 219
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

  static struct resword wordlist[] =
    {
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"in", TYPE_QUAL, RID_IN},
      {"oneway", TYPE_QUAL, RID_ONEWAY},
      {"int", TYPESPEC, RID_INT},
      {"extern", SCSPEC, RID_EXTERN},
      {"__typeof__", TYPEOF, NORID},
      {"inout", TYPE_QUAL, RID_INOUT},
      {"", 0, 0},
      {"__extension__", EXTENSION, NORID},
      {"do", DO, NORID},
      {"", 0, 0}, {"", 0, 0},
      {"while", WHILE, NORID},
      {"", 0, 0},
      {"return", RETURN, NORID},
      {"__inline", SCSPEC, RID_INLINE},
      {"short", TYPESPEC, RID_SHORT},
      {"__inline__", SCSPEC, RID_INLINE},
      {"__restrict", TYPE_QUAL, RID_RESTRICT},
      {"__signed__", TYPESPEC, RID_SIGNED},
      {"__restrict__", TYPE_QUAL, RID_RESTRICT},
      {"", 0, 0}, {"", 0, 0},
      {"__asm__", ASM_KEYWORD, NORID},
      {"", 0, 0},
      {"id", OBJECTNAME, RID_ID},
      {"__iterator__", SCSPEC, RID_ITERATOR},
      {"__typeof", TYPEOF, NORID},
      {"struct", STRUCT, NORID},
      {"__signed", TYPESPEC, RID_SIGNED},
      {"switch", SWITCH, NORID},
      {"@end", END, NORID},
      {"break", BREAK, NORID},
      {"", 0, 0}, {"", 0, 0},
      {"@encode", ENCODE, NORID},
      {"restrict", TYPE_QUAL, RID_RESTRICT},
      {"__iterator", SCSPEC, RID_ITERATOR},
      {"", 0, 0}, {"", 0, 0},
      {"__attribute", ATTRIBUTE, NORID},
      {"", 0, 0},
      {"__attribute__", ATTRIBUTE, NORID},
      {"", 0, 0},
      {"out", TYPE_QUAL, RID_OUT},
      {"@interface", INTERFACE, NORID},
      {"@defs", DEFS, NORID},
      {"", 0, 0},
      {"for", FOR, NORID},
      {"if", IF, NORID},
      {"", 0, 0},
      {"union", UNION, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"byref", TYPE_QUAL, RID_BYREF},
      {"", 0, 0}, {"", 0, 0},
      {"void", TYPESPEC, RID_VOID},
      {"", 0, 0},
      {"double", TYPESPEC, RID_DOUBLE},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"__asm", ASM_KEYWORD, NORID},
      {"", 0, 0},
      {"auto", SCSPEC, RID_AUTO},
      {"__volatile", TYPE_QUAL, RID_VOLATILE},
      {"__saturated__", TYPESPEC, RID_SATURATED},
      {"__volatile__", TYPE_QUAL, RID_VOLATILE},
      {"sizeof", SIZEOF, NORID},
      {"__par256__", TYPESPEC, RID_PAR256 },
      {"__par128__", TYPESPEC, RID_PAR128 },
      {"const", TYPE_QUAL, RID_CONST},
      {"goto", GOTO, NORID},
      {"unsigned", TYPESPEC, RID_UNSIGNED},
      {"", 0, 0},
      {"continue", CONTINUE, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"@implementation", IMPLEMENTATION, NORID},
      {"__label__", LABEL, NORID},
      {"__imag__", IMAGPART, NORID},
      {"", 0, 0},
      {"inline", SCSPEC, RID_INLINE},
      {"typedef", SCSPEC, RID_TYPEDEF},
      {"", 0, 0},
      {"enum", ENUM, NORID},
      {"__const", TYPE_QUAL, RID_CONST},
      {"else", ELSE, NORID},
      {"__const__", TYPE_QUAL, RID_CONST},
      {"", 0, 0},
      {"bycopy", TYPE_QUAL, RID_BYCOPY},
      {"__real__", REALPART, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"__alignof__", ALIGNOF, NORID},
      {"float", TYPESPEC, RID_FLOAT},
      {"register", SCSPEC, RID_REGISTER},
      {"", 0, 0}, {"", 0, 0},
      {"case", CASE, NORID},
      {"char", TYPESPEC, RID_CHAR},
      {"", 0, 0},
      {"typeof", TYPEOF, NORID},
      {"", 0, 0},
      {"@protected", PROTECTED, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"default", DEFAULT, NORID},
      {"", 0, 0},
      {"@private", PRIVATE, NORID},
      {"signed", TYPESPEC, RID_SIGNED},
      {"asm", ASM_KEYWORD, NORID},
      {"", 0, 0}, {"", 0, 0},
      {"__alignof", ALIGNOF, NORID},
      {"", 0, 0},
      {"@selector", SELECTOR, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"volatile", TYPE_QUAL, RID_VOLATILE},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"__complex__", TYPESPEC, RID_COMPLEX},
      {"__complex", TYPESPEC, RID_COMPLEX},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"@compatibility_alias", ALIAS, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0},
      {"long", TYPESPEC, RID_LONG},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"__imag", IMAGPART, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"__real", REALPART, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"@protocol", PROTOCOL, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"static", SCSPEC, RID_STATIC},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"@class", CLASS, NORID},
      {"", 0, 0}, {"", 0, 0}, {"", 0, 0}, {"", 0, 0},
      {"", 0, 0}, {"", 0, 0},
      {"@public", PUBLIC, NORID}
    };


#ifdef __GNUC__
__inline
#endif


struct resword *
is_reserved_word (str, len)
     register char *str;
     register unsigned int len;
{
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
