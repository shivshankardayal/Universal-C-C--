
/* i370-dis.c -- Disassemble I370 instructions
   Copyright 1994 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support
   Hacked by Linas Vepstas <linas@linas.org>,  needs more work still

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
2, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "sysdep.h"
#include "dis-asm.h"
#include "opcode/i370.h"

/* This file provides several disassembler functions, all of which use
   the disassembler interface defined in dis-asm.h.  
*/

int
print_insn_i370 (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[6];
  int status;
  i370_insn_t insn;
  const struct i370_opcode *opcode;
  const struct i370_opcode *opcode_end;
  unsigned int op, iop;

  // printf ("Sorry duude not implemented see opcodes/i370-dis.c for more\n");
  // (*info->fprintf_func) (info->stream, "like whoa wowow\n" );

  status = (*info->read_memory_func) (memaddr, buffer, 4, info);
/*
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
*/
  // insn = bfd_getb32 (buffer);

  /* Get the major opcode of the instruction.  */
  iop = buffer[0];

  /* Find the first match in the opcode table.  We could speed this up
     a bit by doing a binary search on the major opcode.  */
  opcode_end = i370_opcodes + i370_num_opcodes;
  for (opcode = i370_opcodes; opcode < opcode_end; opcode++)
    {
      unsigned long table_op;
      const unsigned char *opindex;
      const struct i370_operand *operand;
      int invalid;
      int need_comma;
      int need_paren;

      /* if ((insn & opcode->mask) != opcode->opcode
       *    || (opcode->flags & dialect) == 0)
       * continue;
       */
      
      op = opcode->opcode.b[0];

      if (op != iop) continue;

      /* Make two passes over the operands.  First see if any of them
         have extraction functions, and, if they do, make sure the
         instruction is valid.  */
      invalid = 0;
      for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
          operand = i370_operands + *opindex;
          if (operand->extract)
            (*operand->extract) (insn, &invalid);
        }
      if (invalid) continue; 

      /* The instruction is valid.  */
      (*info->fprintf_func) (info->stream, "%s", opcode->name);
      if (opcode->operands[0] != 0)
        (*info->fprintf_func) (info->stream, "\t");

      /* Now extract and print the operands.  */
      need_comma = 0;
      need_paren = 0;
      for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
          long value;

          operand = i370_operands + *opindex;

          /* Extract the value from the instruction.  */
          if (operand->extract)
            value = (*operand->extract) (insn, (int *) NULL);
          else
            {
              value = (insn.i[0] >> operand->shift) & ((1 << operand->bits) - 1);
              if ((operand->flags & I370_OPERAND_SIGNED) != 0
                  && (value & (1 << (operand->bits - 1))) != 0)
                value -= 1 << operand->bits;
            }

          /* Print the operand as directed by the flags.  */
          if ((operand->flags & I370_OPERAND_GPR) != 0)
            (*info->fprintf_func) (info->stream, "r%ld, ", value);
          else
            (*info->fprintf_func) (info->stream, "%ld, ", value);

        }

      return opcode->len;

    }


  /* We could not find a match.  */
  // (*info->fprintf_func) (info->stream, ".long 0x%lx", insn);
  (*info->fprintf_func) (info->stream, "duuude unknown insn: %x %x ", buffer[0], buffer[1]);

  return 2;
}

#ifdef JUNK
xxxxxxxxxxxxxxxxxxxxxxxxxxxx

  /* Find the first match in the opcode table.  We could speed this up
     a bit by doing a binary search on the major opcode.  */
  opcode_end = powerpc_opcodes + powerpc_num_opcodes;
  for (opcode = powerpc_opcodes; opcode < opcode_end; opcode++)
    {
      unsigned long table_op;
      const unsigned char *opindex;
      const struct powerpc_operand *operand;
      int invalid;
      int need_comma;
      int need_paren;

      table_op = PPC_OP (opcode->opcode);
      if (op < table_op)
        break;
      if (op > table_op)
        continue;

      if ((insn & opcode->mask) != opcode->opcode
          || (opcode->flags & dialect) == 0)
        continue;

      /* Make two passes over the operands.  First see if any of them
         have extraction functions, and, if they do, make sure the
         instruction is valid.  */
      invalid = 0;
      for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
          operand = powerpc_operands + *opindex;
          if (operand->extract)
            (*operand->extract) (insn, &invalid);
        }
      if (invalid)
        continue;

      /* The instruction is valid.  */
      (*info->fprintf_func) (info->stream, "%s", opcode->name);
      if (opcode->operands[0] != 0)
        (*info->fprintf_func) (info->stream, "\t");

      /* Now extract and print the operands.  */
      need_comma = 0;
      need_paren = 0;
      for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
          long value;

          operand = powerpc_operands + *opindex;

          /* Operands that are marked FAKE are simply ignored.  We
             already made sure that the extract function considered
             the instruction to be valid.  */
          if ((operand->flags & PPC_OPERAND_FAKE) != 0)
            continue;

          /* Extract the value from the instruction.  */
          if (operand->extract)
            value = (*operand->extract) (insn, (int *) NULL);
          else
            {
              value = (insn >> operand->shift) & ((1 << operand->bits) - 1);
              if ((operand->flags & PPC_OPERAND_SIGNED) != 0
                  && (value & (1 << (operand->bits - 1))) != 0)
                value -= 1 << operand->bits;
            }

          /* If the operand is optional, and the value is zero, don't
             print anything.  */
          if ((operand->flags & PPC_OPERAND_OPTIONAL) != 0
              && (operand->flags & PPC_OPERAND_NEXT) == 0
              && value == 0)
            continue;

          if (need_comma)
            {
              (*info->fprintf_func) (info->stream, ",");
              need_comma = 0;
            }

          /* Print the operand as directed by the flags.  */
          if ((operand->flags & PPC_OPERAND_GPR) != 0)
            (*info->fprintf_func) (info->stream, "r%ld", value);
          else if ((operand->flags & PPC_OPERAND_FPR) != 0)
            (*info->fprintf_func) (info->stream, "f%ld", value);
          else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0)
            (*info->print_address_func) (memaddr + value, info);
          else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0)
            (*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
          else if ((operand->flags & PPC_OPERAND_CR) == 0
                   || (dialect & PPC_OPCODE_PPC) == 0)
            (*info->fprintf_func) (info->stream, "%ld", value);
          else
            {
              if (operand->bits == 3)
                (*info->fprintf_func) (info->stream, "cr%d", value);
              else
                {
                  static const char *cbnames[4] = { "lt", "gt", "eq", "so" };
                  int cr;
                  int cc;

                  cr = value >> 2;
                  if (cr != 0)
                    (*info->fprintf_func) (info->stream, "4*cr%d", cr);
                  cc = value & 3;
                  if (cc != 0)
                    {
                      if (cr != 0)
                        (*info->fprintf_func) (info->stream, "+");
                      (*info->fprintf_func) (info->stream, "%s", cbnames[cc]);
                    }
                }
            }

          if (need_paren)
            {
              (*info->fprintf_func) (info->stream, ")");
              need_paren = 0;
            }

          if ((operand->flags & PPC_OPERAND_PARENS) == 0)
            need_comma = 1;
          else
            {
              (*info->fprintf_func) (info->stream, "(");
              need_paren = 1;
            }
        }

      /* We have found and printed an instruction; return.  */
      return 4;
    }

  /* We could not find a match.  */
  (*info->fprintf_func) (info->stream, ".long 0x%lx", insn);

  return 4;
}
#endif
