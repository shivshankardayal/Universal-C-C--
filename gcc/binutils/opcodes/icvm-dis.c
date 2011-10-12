
/* 
   Copyright 1986, 87, 89, 91, 92, 93, 94, 95, 96, 97, 1998
   Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "dis-asm.h"
#include "floatformat.h"
#include <libiberty.h>

#include "opcode/icvm.h"

/* Local function prototypes */

static int
fetch_data PARAMS ((struct disassemble_info *, bfd_byte *));

static void
dummy_print_address PARAMS ((bfd_vma, struct disassemble_info *));

static int
fetch_arg PARAMS ((unsigned char *, int, int, disassemble_info *));

static int twoop;

static int
print_insn_arg PARAMS ((const char *, unsigned char *, unsigned char *,
			bfd_vma, disassemble_info *,bfd_vma));


static char *const reg_names[] = {
  "%r0", "%r1", "%r2", "%r3", "%r4", "%r5", "%r6", "%r7",
  "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%fp", "%sp",
  "%ps", "%pc"};

/* Sign-extend an (unsigned char). */
#if __STDC__ == 1
#define COERCE_SIGNED_CHAR(ch) ((signed char)(ch))
#else
#define COERCE_SIGNED_CHAR(ch) ((int)(((ch) ^ 0x80) & 0xFF) - 128)
#endif

#define PALIGN2(p,ma) ((ma & 1) ? p++,ma++ : 0)

#ifdef BOB_ALIGN32
#define PALIGN4(p,ma) (PALIGN2(p,ma),((ma & 2) ? NEXTWORD(p,ma) : 0))
#else
#define PALIGN4(p,ma) PALIGN2(p,ma)
#endif

#define ICVM_BIGENDIAN (info->current_endian==BFD_ENDIAN_BIG)

/* Get a 1 byte signed integer.  */
#define NEXTBYTE(p,ma)\
      (ma+=2,p += 2, FETCH_DATA (info, p),\
	 ICVM_BIGENDIAN ? COERCE_SIGNED_CHAR(p[-1]) :\
	 COERCE_SIGNED_CHAR(p[-2])\
)

/* Get a 2 byte signed integer.  */
#define COERCE16(x) ((int) (((x) ^ 0x8000) - 0x8000))

#define bW(x) (( (int) (x) ) & 255)
#define GETWORD(s) \
	(ICVM_BIGENDIAN ?\
		 ( (bW(s[0])<<8) | bW(s[1]) ) :\
		 ( (bW(s[1])<<8) | bW(s[0]) ) )

#define NEXTWORD(p,ma)  (\
  (PALIGN2(p,ma),p += 2,ma+=2, FETCH_DATA (info, p),(\
	ICVM_BIGENDIAN ? COERCE16 ((p[-2] << 8) + p[-1]) : \
	COERCE16 ((p[-1] << 8) + p[-2]) \
)))

/* Get a 4 byte signed integer.  */
#define COERCE32(x) ((int) (((x) ^ 0x80000000) - 0x80000000))

#define NEXTLONG(p,ma) ( \
  (PALIGN4(p,ma),ma+=4,p += 4), FETCH_DATA (info, p),( \
   ICVM_BIGENDIAN ?\
   COERCE32 ((((((p[-4] << 8) + p[-3]) << 8) + p[-2]) << 8) + p[-1]) :\
   COERCE32 ((((((p[-1] << 8) + p[-2]) << 8) + p[-3]) << 8) + p[-4])\
))

long long toll(void *b,int bige){
	unsigned char *p=(unsigned char *)b;
	unsigned long long l;
	if(bige){
		l=p[-8];l<<=8;					
		l|=p[-7];l<<=8;					
		l|=p[-6];l<<=8;					
		l|=p[-5];l<<=8;					
		l|=p[-4];l<<=8;					
		l|=p[-3];l<<=8;					
		l|=p[-2];l<<=8;					
		l|=p[-1];
	}else{
		l=p[-1];l<<=8;					
		l|=p[-2];l<<=8;					
		l|=p[-3];l<<=8;					
		l|=p[-4];l<<=8;					
		l|=p[-5];l<<=8;					
		l|=p[-6];l<<=8;					
		l|=p[-7];l<<=8;					
		l|=p[-8];

	}		
	return l;
}			

#define NEXTLONGLONG(p,ma) ( \
  (PALIGN4(p,ma),ma+=8,p += 8), FETCH_DATA (info, p),( \
   ICVM_BIGENDIAN ?\
   toll(p,1) :\
   toll(p,0)\
))



/* Get a single precision float.  */
#define NEXTSINGLE(val, p,ma) (\
  ((PALIGN4(p,ma),p += 4,ma+=4), FETCH_DATA (info, p), (\
   ICVM_BIGENDIAN ?\
   floatformat_to_double (&floatformat_ieee_single_big, (char *) p - 4, &val) :\
   floatformat_to_double (&floatformat_ieee_single_little, (char *) p - 4, &val)\
)))

/* Get a double precision float.  */
#define NEXTDOUBLE(val, p,ma) (\
  ((PALIGN4(p,ma),p += 8,ma+=8), FETCH_DATA (info, p), (\
   ICVM_BIGENDIAN ?\
   floatformat_to_double (&floatformat_ieee_double_big, (char *) p - 8, &val) :\
   floatformat_to_double (&floatformat_ieee_double_little, (char *) p - 8, &val)\
)))

/* Get an extended precision float.  */
#define NEXTEXTEND(val, p,ma) \
  ((PALIGN4(p,ma),p += 12,ma+=12), FETCH_DATA (info, p), \
   floatformat_to_double (&floatformat_m68881_ext, (char *) p - 12, &val))

/* Need a function to convert from packed to double
   precision.   Actually, it's easier to print a
   packed number than a double anyway, so maybe
   there should be a special case to handle this... */
#define NEXTPACKED(p,ma) \
  ((PALIGN4(p,ma),p += 12,ma+=12), FETCH_DATA (info, p), 0.0)


/* Maximum length of an instruction.  */
#define MAXLEN 32

#include <setjmp.h>

struct private
{
  /* Points to first byte not fetched.  */
  bfd_byte *max_fetched;
  bfd_byte the_buffer[MAXLEN];
  bfd_vma insn_start;
  jmp_buf bailout;
};

/* Make sure that bytes from INFO->PRIVATE_DATA->BUFFER (inclusive)
   to ADDR (exclusive) are valid.  Returns 1 for success, longjmps
   on error.  */
#define FETCH_DATA(info, addr) \
  ((addr) <= ((struct private *)(info->private_data))->max_fetched \
   ? 1 : fetch_data ((info), (addr)))

static int
fetch_data (info, addr)
     struct disassemble_info *info;
     bfd_byte *addr;
{
  int status;
  struct private *priv = (struct private *)info->private_data;
  bfd_vma start = priv->insn_start + (priv->max_fetched - priv->the_buffer);

  status = (*info->read_memory_func) (start,
				      priv->max_fetched,
				      addr - priv->max_fetched,
				      info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, start, info);
      longjmp (priv->bailout, 1);
    }
  else
    priv->max_fetched = addr;
  return 1;
}

/* This function is used to print to the bit-bucket. */
static int
#ifdef __STDC__
dummy_printer (FILE * file, const char * format, ...)
#else
dummy_printer (file) FILE *file;
#endif
 { return 0; }

static void
dummy_print_address (vma, info)
     bfd_vma vma;
     struct disassemble_info *info;
{
}




#define oe(op,idx) (op[idx].opc)
#define me(op,idx) (op[idx].msk)
#define in(op,idx) (icvmstrpool[op[idx].opcode])
#define is(op,idx) (icvmstrpool[op[idx].operand])




/* Print the icvm instruction at address MEMADDR in debugged memory,
   on INFO->STREAM.  Returns length of the instruction, in bytes.  */

int
print_insn_icvm (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  register int i;
  register unsigned char *p;
  unsigned char *save_p;
  int eaten;
  register const char *d;
  register unsigned long bestmask;
  const struct icvm_opcode *best = 0;
  unsigned int arch_mask;
  struct private priv;
  bfd_byte *buffer = priv.the_buffer;
  fprintf_ftype save_printer = info->fprintf_func;
  void (*save_print_address) PARAMS((bfd_vma, struct disassemble_info*))
    = info->print_address_func;
  int major_opcode;
  static int numopcodes[16];
  static const struct icvm_opcode **opcodes[16];
  bfd_vma ma;
  bfd_vma pcma;
  bfd_vma save_ma;
  int inputop;
  int inputop1;
  int inputop1loaded=0;
  if (!opcodes[0])
    {
      /* Speed up the matching by sorting the opcode table on the upper
	 four bits of the opcode.  */
      const struct icvm_opcode **opc_pointer[16];

      /* First count how many opcodes are in each of the sixteen buckets.  */
      for (i = 0; i < icvm_numopcodes; i++)
	numopcodes[( oe(icvm_opcodes,i) >> 28) & 15]++;

      /* Then create a sorted table of pointers that point into the
	 unsorted table.  */
      opc_pointer[0] = ((const struct icvm_opcode **)
			xmalloc (sizeof (struct icvm_opcode *)
				 * icvm_numopcodes));
      opcodes[0] = opc_pointer[0];
      for (i = 1; i < 16; i++)
	{
	  opc_pointer[i] = opc_pointer[i - 1] + numopcodes[i - 1];
	  opcodes[i] = opc_pointer[i];
	}

      for (i = 0; i < icvm_numopcodes; i++)
	*opc_pointer[(oe(icvm_opcodes,i) >> 28) & 15]++ = &icvm_opcodes[i];

    }

  info->private_data = (PTR) &priv;
  /* Tell objdump to use two bytes per chunk and six bytes per line for
     displaying raw data.  */
  info->bytes_per_chunk = 2;
  info->bytes_per_line = 6;
  info->display_endian = BFD_ENDIAN_BIG;
  priv.max_fetched = priv.the_buffer;
  priv.insn_start = memaddr;
  if (setjmp (priv.bailout) != 0)
    /* Error return.  */
    return -1;
  arch_mask = (unsigned int) -1;

  bestmask = 0;
  p = buffer;
  ma=memaddr;
  PALIGN4(p,ma);
  pcma=ma;
  inputop=NEXTWORD(p,ma);


  major_opcode = (inputop >> 12) & 15;
  for (i = 0; i < numopcodes[major_opcode]; i++)
    {
      const struct icvm_opcode *opc = opcodes[major_opcode][i];
      unsigned long opcode = oe(opc,0);
      unsigned long match = me(opc,0);


      if (((0xff & (inputop>>8) & (match >> 24)) == (0xff & (opcode >> 24)))
		  && ((0xff & inputop & (match >> 16)) == (0xff & (opcode >> 16))))
	    {
			if(match & 0xffff){ /* extended opcodes */
				if(inputop1loaded==0){
					inputop1loaded=1;
					save_p = p;
				  	save_ma = ma;
				  	inputop1=NEXTWORD(p,ma);
				  	p=save_p;
			  		ma=save_ma;
				}

		      if (((0xff & (inputop1>>8) & (match >> 8)) == (0xff & (opcode >> 8)))
				  && ((0xff & inputop1 & (match)) == (0xff & (opcode))))
					goto matches;


			
			}else{
matches:
	            if(match>bestmask){
		            best = opc;
		            bestmask = match;
		        }
			}

	    }
    }

  if (best == 0)
    goto invalid;
  twoop=0;											
  if(bestmask & 0xffff){ /* extended opcode */
  	NEXTWORD(p,ma);
	twoop=1;
  }
		


  FETCH_DATA (info, p);
  
  d = is(best,0);

  /* We can the operands twice.  The first time we don't print anything,
     but look for errors. */

  save_p = p;
  save_ma = ma;
  info->print_address_func = dummy_print_address;
  info->fprintf_func = (fprintf_ftype)dummy_printer;
  for ( ; *d; d += 2)
    {
/*      eaten = print_insn_arg (d, buffer, p, memaddr + p - buffer, info,ma); */
      eaten = print_insn_arg (d, buffer, p, pcma, info,ma);
      if (eaten >= 0){
	    p += eaten;
	    ma += eaten;
     }
      else if (eaten == -1)
	    goto invalid;
      else
	{
	  (*info->fprintf_func)(info->stream,
				"<internal error in opcode table: %s %s>\n",
				in(best,0),
				is(best,0));
	  goto invalid;
	}

    }

  p = save_p;
  ma = save_ma;
  info->fprintf_func = save_printer;
  info->print_address_func = save_print_address;

  d = is(best,0);

  (*info->fprintf_func) (info->stream, "%s", in(best,0));

  if (*d)
    (*info->fprintf_func) (info->stream, " ");
  ma=memaddr+2;
  while (*d)
    {
/*      eaten = print_insn_arg (d, buffer, p, memaddr + p - buffer, info,ma); */
      eaten = print_insn_arg (d, buffer, p, pcma, info,ma);
      p += eaten;
      ma += eaten;
      d += 2;
      if (*d)
	(*info->fprintf_func) (info->stream, ",");
    }

  PALIGN4(p,ma);
  return p - buffer;

 invalid:
  /* Handle undefined instructions.  */
  info->fprintf_func = save_printer;
  info->print_address_func = save_print_address;
  (*info->fprintf_func) (info->stream, "??? 0%x",
			 (buffer[0] << 8) + buffer[1]);
  return 2;
}

/* Returns number of bytes "eaten" by the operand, or
   return -1 if an invalid operand was found, or -2 if
   an opcode tabe error was found. */

static int
print_insn_arg (d, buffer, p0, addr, info,ma)
     const char *d;
     unsigned char *buffer;
     unsigned char *p0;
     bfd_vma addr;		/* PC for this arg to be relative to */
     disassemble_info *info;
     bfd_vma ma;
{
  register int val = 0;
  register int val1 = 0;
  register int val2 = 0;
  register int place = d[1];
  register unsigned char *p = p0;
  int regno;
  register CONST char *regname;
  double flval;
  int flt_p;
  switch (*d)
    {


    case 'd':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "@(%d,%%fp)",val);
      break;

    case '0':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "((%d,%%fp)+)",val);
      break;



    case '5':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      val2 = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(%d,(%d,%%fp),(%d,%%fp))",val,val1,val2);
      break;

    case '6':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(@(%d,%%fp),(%d,%%fp))",val,val1);
      break;

    case '7':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(@(%d,%%fp,(%d,%%fp)@)",val,val1);
      break;


    case '8':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      val2 = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(%d,(%d,%%fp),(%d,%%fp)@)",val,val1,val2);
      break;

    case 'f':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(%d,(%d,%%fp))",val,val1);
      break;


    case 'r':
      (*info->fprintf_func) (info->stream, "%%r%c",d[1]);
      break;

    case 'a':
      (*info->fprintf_func) (info->stream, "(r%c)",d[1]);
      break;

    case 'A':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,%%r%c)",val,d[1]);
      break;

	case '1':
		val1=d[1]-'2';
		if(val1)
			val2=0;
		else
			val2=1;
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,%s,%s)",
			     val,reg_names[val1+2],reg_names[val2+2]);
		break;			

	case '2':
		val1=d[1]-'2';
		if(val1)
			val2=0;
		else
			val2=1;
      (*info->fprintf_func) (info->stream, "(%s,%s)",
			     reg_names[val1+2],reg_names[val2+2]);
		break;			

	case '3':
		val1=d[1]-'2';
		if(val1)
			val2=0;
		else
			val2=1;
      (*info->fprintf_func) (info->stream, "(%s,%s@)",
			     reg_names[val1+2],reg_names[val2+2]);
		break;			

	case '4':
		val1=d[1]-'2';
		if(val1)
			val2=0;
		else
			val2=1;
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,%s,%s@)",
			     val,reg_names[val1+2],reg_names[val2+2]);
		break;

    case 'G':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(r%c,(%d,%%fp))",
			d[1],
			val	
	  );
      break;

    case 'H':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(r%c,(%d,%%fp)@)",
			d[1],
			val	
	  );
      break;

    case 'K':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(@ (%d,%%fp),r%c@)",val,
			d[1]
	  );
      break;


    case 'V':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,r%c,(%d,%%fp))",val,
			d[1],
			val1	
	  );
      break;

    case 'Y':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,(%d,%%fp),r%c@)",val,val1,
			d[1]
	  );
      break;

    case 'W':
      val = NEXTWORD(p,ma);
      val1 = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,r%c,(%d,%%fp)@)",val,
			d[1],
			val1	
	  );
      break;


    case 'c':
      (*info->fprintf_func) (info->stream, "%%q%c",d[1]);
      break;

	case 'M':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func) (info->stream, "(%d,%%m%d)",val,
			fetch_arg(buffer,place,4,info));
      break;

    case 'm':
      (*info->fprintf_func) (info->stream, "%%m%d",
			fetch_arg(buffer,place,4,info));
      break;

    case 'v':
      (*info->fprintf_func) (info->stream, "%%v%d",
			fetch_arg(buffer,place,4,info));
      break;


    case 'x':
      (*info->fprintf_func) (info->stream, "%%x%d",
			fetch_arg(buffer,place,4,info));

      break;





    case 'F':
      (*info->fprintf_func) (info->stream, "%%fp%c",d[1]);
      break;


    case '+':
      (*info->fprintf_func)
    	(info->stream, "(%%sp)+");
      break;

    case '-':
      (*info->fprintf_func)
	    (info->stream, "-(%%sp)");
      break;


    case 'E': 
    	val = NEXTLONG (p,ma);
      (*info->print_address_func) (val, info);
        break;

    case '#':
        switch(place){
    	  case 'l':
        	val = NEXTLONG (p,ma);
            break;

    	  case 'q':
		  {
        	long long llval = NEXTLONGLONG(p,ma);
      			(*info->fprintf_func) (info->stream, "#0x%qx", llval);
			goto noprint;
	    	 }
    		break;
 
    	  case 'w':
        	val = NEXTWORD(p,ma);
            break;

    	  case 'b':
        	val = NEXTBYTE(p,ma);
            break;

    	  case 'f':
	        NEXTSINGLE(flval, p,ma);
	        (*info->fprintf_func) (info->stream, "#0r%g", flval);
	        goto noprint;

    	  case 'd':
	        NEXTDOUBLE(flval, p,ma);
	        (*info->fprintf_func) (info->stream, "#0r%g", flval);
	        goto noprint;

	      case 'x':
	        NEXTEXTEND(flval, p,ma);
	        (*info->fprintf_func) (info->stream, "#0r%g", flval);
	        goto noprint;
          default:
        	return -2;
        }       


      (*info->fprintf_func) (info->stream, "#0x%x", val);
noprint:
      break;





    case 'B':
     if (place == 'L')
	    val = NEXTLONG (p,ma);
    else
	    return -2;
    (*info->print_address_func) (addr + val, info);
    break;



    case 'S':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(%d,%%sp)",val);
      break;

    case 's':
      (*info->fprintf_func)
	(info->stream, "%%sp",val);
      break;

    case 'b':
      (*info->fprintf_func)
	(info->stream, "%%fp",val);
      break;

    case 'p':
      (*info->fprintf_func)
	(info->stream, "%%pc",val);
      break;


    case 'U':
      val = NEXTWORD(p,ma);
      (*info->fprintf_func)
	(info->stream, "(%d,%%fp)",val);
      break;

#ifdef NOT_NOW

    case 'L':
    case 'l':
	if (place == 'w')
	  {
	    char doneany;
	    p1 = buffer + 2;
	    val = NEXTWORD (p1,ma);
	    /* Move the pointer ahead if this point is farther ahead
	       than the last.  */
	    p = p1 > p ? p1 : p;
	    if (val == 0)
	      {
		(*info->fprintf_func) (info->stream, "#0");
		break;
	      }
	    if (*d == 'l')
	      {
		register int newval = 0;
		for (regno = 0; regno < 16; ++regno)
		  if (val & (0x8000 >> regno))
		    newval |= 1 << regno;
		val = newval;
	      }
	    val &= 0xffff;
	    doneany = 0;
	    for (regno = 0; regno < 16; ++regno)
	      if (val & (1 << regno))
		{
		  int first_regno;
		  if (doneany)
		    (*info->fprintf_func) (info->stream, "/");
		  doneany = 1;
		  (*info->fprintf_func) (info->stream, "%s", reg_names[regno]);
		  first_regno = regno;
		  while (val & (1 << (regno + 1)))
		    ++regno;
		  if (regno > first_regno)
		    (*info->fprintf_func) (info->stream, "-%s",
					   reg_names[regno]);
		}
	  }
	else if (place == '3')
	  {
	    /* `fmovem' insn.  */
	    char doneany;
	    val = fetch_arg (buffer, place, 8, info);
	    if (val == 0)
	      {
		(*info->fprintf_func) (info->stream, "#0");
		break;
	      }
	    if (*d == 'l')
	      {
		register int newval = 0;
		for (regno = 0; regno < 8; ++regno)
		  if (val & (0x80 >> regno))
		    newval |= 1 << regno;
		val = newval;
	      }
	    val &= 0xff;
	    doneany = 0;
	    for (regno = 0; regno < 8; ++regno)
	      if (val & (1 << regno))
		{
		  int first_regno;
		  if (doneany)
		    (*info->fprintf_func) (info->stream, "/");
		  doneany = 1;
		  (*info->fprintf_func) (info->stream, "%%fp%d", regno);
		  first_regno = regno;
		  while (val & (1 << (regno + 1)))
		    ++regno;
		  if (regno > first_regno)
		    (*info->fprintf_func) (info->stream, "-%%fp%d", regno);
		}
	  }
	else
	  return -2;
      break;
#endif

    default:
      return -2;
    }
  return p - p0;
}

/* Fetch BITS bits from a position in the instruction specified by CODE.
   CODE is a "place to put an argument", or 'x' for a destination
   that is a general address (mode and register).
   BUFFER contains the instruction.  */

static int
fetch_arg (buffer, code, bits, info)
     unsigned char *buffer;
     int code;
     int bits;
     disassemble_info *info;
{
  register int val = 0;
  if(twoop)
     buffer+=2;
  switch (code)
    {

    case 's':
      val = GETWORD(buffer) & 255;
      break;

    case 'd':			/* Destination, for register or quick.  */
      val = GETWORD(buffer);
      val >>= 9;
      break;

    case 'h':			/* Destination, for register or quick.  */
      val = GETWORD(buffer);
      val >>= 1;
      break;


    case 'x':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 6;
      break;


    case 'y':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 3;
      break;

    case 'u':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 2;
      break;

    case 'v':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 4;
      break;

    case 'e':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 5;
      break;

    case 'z':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 7;
      break;

    case 'p':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 8;
      break;

    case 'q':			/* Destination, for general arg */
      val = GETWORD(buffer);
      val >>= 10;
      break;


    default:
      abort ();
    }

  switch (bits)
    {
    case 1:
      return val & 1;
    case 2:
      return val & 3;
    case 3:
      return val & 7;
    case 4:
      return val & 017;
    case 5:
      return val & 037;
    case 6:
      return val & 077;
    case 7:
      return val & 0177;
    case 8:
      return val & 0377;
    case 12:
      return val & 07777;
    default:
      abort ();
    }
}

