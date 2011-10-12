;;- Machine description for GNU compiler, Motorola 68000 Version
;;  Copyright (C) 1987, 88, 93, 94, 95, 96, 1997 Free Software Foundation, Inc.

;; This file is part of GNU CC.

;; GNU CC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU CC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU CC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.


;;- @@The original PO technology requires these to be ordered by speed,
;;- @@    so that assigner will pick the fastest.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;;- When naming insn's (operand 0 of define_insn) be careful about using
;;- names from other targets machine descriptions.








;; Call subroutine with no return value.
(define_insn "call"
  [(call (match_operand:QI 0 "general_operand" "g")
	 (match_operand:SI 1 "general_operand" "g"))]
  ""
  "*{
   char *s=buildsyscall(operands[0]);
  if(s)
    return s;
  if (GET_CODE (operands[0]) == MEM
      && GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF)
		return \"bsrl %0\";
  else	
		return \"jsrl %0\";
 
}")


;; Call subroutine, returning value in operand 0
;; (which must be a hard register).
;; See comments before "call" regarding PIC calls.
(define_insn "call_value"
  [(set (match_operand 0 "" "=g")
	(call (match_operand:QI 1 "general_operand" "g")
     (match_operand:SI 2 "general_operand" "g")))]
  ""
  "*{
   char *s=buildsyscall(operands[1]);
  if(s)
    return s;
  if (GET_CODE (operands[1]) == MEM
      && GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF)
		return \"bsrl %1\";
  else	
		return \"jsrl %1\";
}")


;; UNSPEC_VOLATILE is considered to use and clobber all hard registers and
;; all of memory.  This blocks insns from being moved across this point.

(define_insn "blockage"
  [(unspec_volatile [(const_int 0)] 0)]
  ""
  "")

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")


(define_expand "return"
  [(return)]
  "USE_RETURN_INSN"
  "")

(define_insn "return_internal"
  [(return)]
  "reload_completed"
  "rtsframe")

(define_insn "return_pop_internal"
  [(return)
   (use (match_operand:SI 0 "const_int_operand" ""))]
  "reload_completed"
  "rtsframew %0")






;; @@@@@@@@@@@@@@@@@@@ BOB's STUFF @@@@@@@@@@@@@@@@@@@@@@@@@@




;; *************************
;;  @move instructions
;; *************************



(define_insn "movsi"
  [(set (match_operand:SI 0 "general_operand" "=omb<")
	(match_operand:SI 1 "general_operand" "ombi>"))]
  ""
  "*{
	 return output_moveit(operands,\"zerol\",\"movewl\",\"movel\");
}
")

(define_insn "movsisf"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(match_operand:SI 1 "general_operand" "ombi>"))]
  ""
  "*{
	 return output_moveit(operands,\"zerol\",\"movewl\",\"movel\");
}
")

(define_insn "movsfsi"
  [(set (match_operand:SI 0 "general_operand" "=omb<")
	(match_operand:SF 1 "general_operand" "omZs>"))]
  ""
  "*{
	 return output_moveit(operands,\"zerol\",\"movewl\",\"movel\");
}
")


(define_insn "movhi"
  [(set (match_operand:HI 0 "general_operand" "=omW<")
	(match_operand:HI 1 "general_operand" "omWi>"))]
  ""
  "*{
	 return output_moveit(operands,\"zerow\",0,\"movew\");
}
")

(define_insn "movqi"
  [(set (match_operand:QI 0 "general_operand" "=omB<")
	(match_operand:QI 1 "general_operand" "omBi>"))]
  ""
  "*{
	 return output_moveit(operands,\"zerob\",0,\"moveb\");
}
")


(define_insn "movsf"
  [(set (match_operand:SF 0 "general_operand" "I=omZ<")
	(match_operand:SF 1 "general_operand" "IomZs>"))]
  ""
  "* return output_moveit(operands,\"zerof\",0,\"movef\");")



(define_insn "movdf"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(match_operand:DF 1 "general_operand" "omfs>"))]
  ""
  "* return output_moveit(operands,\"zerod\",0,\"moved\");")


(define_insn "movdi"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(match_operand:DI 1 "general_operand" "omqi>"))]
  ""
  "* return output_moveit(operands,\"zeroq\",0,\"moveq\");")


(define_insn "movdidf"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(match_operand:DI 1 "general_operand" "omqi>"))]
  ""
  "* return output_moveit(operands,\"zeroq\",0,\"moveq\");")

(define_insn "movdfdi"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(match_operand:DF 1 "general_operand" "omfs>"))]
  ""
  "* return output_moveit(operands,\"zeroq\",0,\"moveq\");")




;;****************************************
;; @truncation instructions
;; ***************************************




(define_insn "truncsihi2"
  [(set (match_operand:HI 0 "general_operand" "=omW<")
	(truncate:HI
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "*{
	 return output_moveit(operands,0,0,\"movelw\");
}
")


(define_insn "truncsiqi2"
  [(set (match_operand:QI 0 "general_operand" "=omB<")
	(truncate:QI
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "*{
	 return output_moveit(operands,0,0,\"movelb\");
}
")


(define_insn "trunchiqi2"
  [(set (match_operand:QI 0 "general_operand" "=omB<")
	(truncate:QI
	 (match_operand:HI 1 "general_operand" "omWi>")))]
  ""
  "*{
	 return output_moveit(operands,0,0,\"movewb\");
}
")


(define_insn "truncdisi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(truncate:SI
	 (match_operand:DI 1 "general_operand" "omqi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveql\");")

(define_insn "truncdihi2"
  [(set (match_operand:HI 0 "general_operand" "=omW<")
	(truncate:HI
	 (match_operand:DI 1 "general_operand" "omqi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveqw\");")

(define_insn "truncdiqi2"
  [(set (match_operand:QI 0 "general_operand" "=omB<")
	(truncate:QI
	 (match_operand:DI 1 "general_operand" "omqi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveqb\");")


(define_insn "truncdfsf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(float_truncate:SF
	 (match_operand:DF 1 "general_operand" "omfs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movedf\");")


(define_insn "fix_truncdfsi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(fix:SI
	 (match_operand:DF 1 "general_operand" "omfs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movedl\");")


(define_insn "fix_truncdfdi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(fix:DI
	 (match_operand:DF 1 "general_operand" "omfs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movedq\");")



(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(fix:SI
	 (match_operand:SF 1 "general_operand" "omZs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movefl\");")


(define_insn "fix_truncsfdi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(fix:DI
	 (match_operand:SF 1 "general_operand" "omZs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movefq\");")



;; **************************
;; sign @extend instructions

;; **************************

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(sign_extend:SI
	 (match_operand:HI 1 "general_operand" "omWi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movewl\");")

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(sign_extend:SI
	 (match_operand:QI 1 "general_operand" "omBi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movebl\");")

(define_insn "extendqihi2"
  [(set (match_operand:HI 0 "general_operand" "=omW<")
	(sign_extend:HI
	 (match_operand:QI 1 "general_operand" "omBi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movebw\");")

(define_insn "extendsidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(sign_extend:DI
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movelq\");")

(define_insn "extendhidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(sign_extend:DI
	 (match_operand:HI 1 "general_operand" "omWi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movewq\");")

(define_insn "extendqidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(sign_extend:DI
	 (match_operand:QI 1 "general_operand" "omBi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movebq\");")




(define_insn "extendsfdf2"
  [(set (match_operand:DF 0 "general_operand" "==omf<")
	(float_extend:DF
	 (match_operand:SF 1 "general_operand" "omZs>")))]
  ""
  "* return output_moveit(operands,0,0,\"movefd\");")


(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(float:SF
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "* return output_moveit(operands,0,0,\"movelf\");")

(define_insn "floatdisf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(float:SF
	 (match_operand:DI 1 "general_operand" "omqi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveqf\");")



(define_insn "floatsidf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(float:DF
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveld\");")


(define_insn "floatdidf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(float:DF
	 (match_operand:DI 1 "general_operand" "omqi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveqd\");")


;; **************************
;; zero @extend instructions

;; **************************

(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<")
	(zero_extend:SI
	 (match_operand:HI 1 "general_operand" "omWi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveuwl\");")

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "general_operand" "=omd<,=omd<")
	(zero_extend:SI
	 (match_operand:QI 1 "general_operand" "omBi>,W")))]
  ""
  "* return output_moveit(operands,0,0,\"moveubl\");")

(define_insn "zero_extendqihi2"
  [(set (match_operand:HI 0 "general_operand" "=omW<")
	(zero_extend:HI
	 (match_operand:QI 1 "general_operand" "omBi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveubw\");")

(define_insn "zero_extendsidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(zero_extend:DI
	 (match_operand:SI 1 "general_operand" "omdi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveulq\");")

(define_insn "zero_extendhidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(zero_extend:DI
	 (match_operand:HI 1 "general_operand" "omWi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveuwq\");")

(define_insn "zero_extendqidi2"
  [(set (match_operand:DI 0 "general_operand" "=omq<")
	(zero_extend:DI
	 (match_operand:QI 1 "general_operand" "omBi>")))]
  ""
  "* return output_moveit(operands,0,0,\"moveubq\");")



;;******************************************
;; Unconditional and other jump instructions
;;******************************************


(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "*
  return \"bral %l0\";
")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "general_operand" "omdi>"))]
  ""
  "movel %0,%%pc")


;; Jump to variable address from dispatch table of absolute addresses.
(define_insn "tablejump"
  [(set (pc) (match_operand:SI 0 "nonimmediate_simpleoperand" "omd>"))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "*{

  operands[1]=gen_rtx_MEM(SImode,operands[1]);
  return \"addral %1,%0,-(%%sp)\\n\\trts\";
}")




;; ***************************
;; @and instructions
;; ***************************


(define_insn "andsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(and:SI (match_operand:SI 1 "general_simpleoperand" "%omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"andl\",\"andwl\",IG_CANSWAP | IG_TRIPLE);")



(define_insn "andhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(and:HI (match_operand:HI 1 "general_simpleoperand" "%omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"andw\",0,IG_CANSWAP | IG_TRIPLE);")




(define_insn "andqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(and:QI (match_operand:QI 1 "general_simpleoperand" "%omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"andb\",0,IG_CANSWAP | IG_TRIPLE);")


(define_insn "anddi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(and:DI (match_operand:DI 1 "general_simpleoperand" "%omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"andq\",\"andwq\",IG_CANSWAP | IG_TRIPLE);")
									
									

	
;; ***************************
;; inclusive @or instructions
;; ***************************


(define_insn "iorsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(ior:SI (match_operand:SI 1 "general_simpleoperand" "%omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"orl\",\"orwl\",IG_CANSWAP | IG_TRIPLE);")



(define_insn "iorhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(ior:HI (match_operand:HI 1 "general_simpleoperand" "%omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"orw\",0,IG_CANSWAP | IG_TRIPLE);")



(define_insn "iorqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(ior:QI (match_operand:QI 1 "general_simpleoperand" "%omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"orb\",0,IG_CANSWAP | IG_TRIPLE);")

(define_insn "iordi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(ior:DI (match_operand:DI 1 "general_simpleoperand" "%omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"orq\",\"orwq\",IG_CANSWAP | IG_TRIPLE);")

										
										

;; ***************************
;; exclusive @or instructions
;; ***************************



(define_insn "xorsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(xor:SI (match_operand:SI 1 "general_simpleoperand" "%omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"eorl\",\"eorwl\",IG_CANSWAP | IG_TRIPLE);")


(define_insn "xorhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(xor:HI (match_operand:HI 1 "general_simpleoperand" "%omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"eorw\",0,IG_CANSWAP | IG_TRIPLE);")



(define_insn "xorqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(xor:QI (match_operand:QI 1 "general_simpleoperand" "%omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"eorb\",0,IG_CANSWAP | IG_TRIPLE);")


(define_insn "xordi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(xor:DI (match_operand:DI 1 "general_simpleoperand" "%omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"eorq\",\"eorwq\",IG_CANSWAP | IG_TRIPLE);")




;; ******************************
;; @add instructions
;; ******************************



(define_insn "addsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omb<,=omb<")
	(plus:SI (match_operand:SI 1 "general_simpleoperand" "%omb>,0")
		 (match_operand:SI 2 "general_simpleoperand" "ombi>,ombi>")))]
  ""
  "* return output_general(operands,\"movel\",\"addl\",\"addwl\",IG_CANSWAP | IG_TRIPLE);")



(define_insn "addhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(plus:HI (match_operand:HI 1 "general_simpleoperand" "%omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"addw\",0,IG_CANSWAP | IG_TRIPLE);")


(define_insn "addqi3_1"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(plus:QI (match_operand:QI 1 "general_simpleoperand" "%omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"addb\",0,IG_CANSWAP | IG_TRIPLE);")


(define_insn "adddi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(plus:DI (match_operand:DI 1 "general_simpleoperand" "%omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"addq\",\"addwq\",IG_CANSWAP | IG_TRIPLE);")



(define_insn "addsf3"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<,=omZ<")
	(plus:SF (match_operand:SF 1 "nonimmediate_simpleoperand" "%omZ>,0")
		 (match_operand:SF 2 "general_simpleoperand" "omZs>,omZs>")))]
  ""
  "* return output_general(operands,\"movef\",\"addf\",0,IG_CANSWAP | IG_TRIPLE);")


(define_insn "adddf3"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<,=omf<")
	(plus:DF (match_operand:DF 1 "nonimmediate_simpleoperand" "%omf>,0")
		 (match_operand:DF 2 "general_simpleoperand" "omfs>,omfs>")))]
  ""
  "* return output_general(operands,\"moved\",\"addd\",0,IG_CANSWAP | IG_TRIPLE);")




;; ******************************
;; @sub instructions
;; ******************************



(define_insn "subsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(minus:SI (match_operand:SI 1 "nonimmediate_simpleoperand" "omd>,0")
		 (match_operand:SI 2 "nonimmediate_simpleoperand" "omd>,omd>")))]
  ""
  "* return output_general(operands,\"movel\",\"subl\",\"subwl\",IG_TRIPLE);")


(define_insn "subhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(minus:HI (match_operand:HI 1 "nonimmediate_simpleoperand" "0")
		 (match_operand:HI 2 "nonimmediate_simpleoperand" "omW>")))]
  ""
  "* return output_general(operands,\"movew\",\"subw\",0,0);")


(define_insn "subqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(minus:QI (match_operand:QI 1 "nonimmediate_simpleoperand" "0")
		 (match_operand:QI 2 "nonimmediate_simpleoperand" "omB>")))]
  ""
  "* return output_general(operands,\"moveb\",\"subb\",0,0);")


(define_insn "subdi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(minus:DI (match_operand:DI 1 "nonimmediate_simpleoperand" "0")
		 (match_operand:DI 2 "nonimmediate_simpleoperand" "omq>")))]
  ""
  "* return output_general(operands,\"moveq\",\"subq\",\"subwq\",0);")


(define_insn "subsf3"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<")
	(minus:SF (match_operand:SF 1 "nonimmediate_simpleoperand" "0")
		 (match_operand:SF 2 "nonimmediate_simpleoperand" "omZ>")))]
  ""
  "* return output_general(operands,\"movef\",\"subf\",0,0);")



(define_insn "subdf3"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<")
	(minus:DF (match_operand:DF 1 "nonimmediate_simpleoperand" "0")
		 (match_operand:DF 2 "nonimmediate_simpleoperand" "omf>")))]
  ""
  "* return output_general(operands,\"moved\",\"subd\",0,0);")




;; ***************************
;; @asr instructions
;; ***************************


(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(ashiftrt:SI (match_operand:SI 1 "general_simpleoperand" "omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"asrl\",\"asrwl\", IG_TRIPLE);")




(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(ashiftrt:HI (match_operand:HI 1 "general_simpleoperand" "omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"asrw\",0,IG_TRIPLE);")

(define_insn "ashrqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(ashiftrt:QI (match_operand:QI 1 "general_simpleoperand" "omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"asrb\",0,IG_TRIPLE);")


(define_insn "ashrdi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(ashiftrt:DI (match_operand:DI 1 "general_simpleoperand" "omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"asrq\",\"asrwq\",IG_TRIPLE);")






;; ***************************
;; lsr instructions
;; ***************************


(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(lshiftrt:SI (match_operand:SI 1 "general_simpleoperand" "omdi>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"lsrl\",\"lsrwl\",IG_TRIPLE);")


(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(lshiftrt:HI (match_operand:HI 1 "general_simpleoperand" "omWi>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"lsrw\",0,IG_TRIPLE);")


(define_insn "lshrqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(lshiftrt:QI (match_operand:QI 1 "general_simpleoperand" "omBi>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"lsrb\",0,IG_TRIPLE);")


(define_insn "lshrdi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(lshiftrt:DI (match_operand:DI 1 "general_simpleoperand" "omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"lsrq\",\"lsrwq\",IG_TRIPLE);")



;; ***************************
;; @asl instructions
;; ***************************


(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(ashift:SI (match_operand:SI 1 "general_simpleoperand" "omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "i,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"asll\",\"aslwl\",IG_TRIPLE);")


(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(ashift:HI (match_operand:HI 1 "general_simpleoperand" "omW>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"aslw\",0,IG_TRIPLE);")


(define_insn "ashlqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(ashift:QI (match_operand:QI 1 "general_simpleoperand" "omB>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"aslb\",0,IG_TRIPLE);")


(define_insn "ashldi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(ashift:DI (match_operand:DI 1 "general_simpleoperand" "omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"aslq\",\"aslwq\",IG_TRIPLE);")


;; ******************************
;; @mul instructions
;; ******************************



(define_insn "mulsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<,=omd<")
	(mult:SI (match_operand:SI 1 "general_simpleoperand" "%omd>,0")
		 (match_operand:SI 2 "general_simpleoperand" "omdi>,omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"mull\",\"mulwl\",IG_CANSWAP | IG_TRIPLE);")



(define_insn "mulhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<,=omW<")
	(mult:HI (match_operand:HI 1 "general_simpleoperand" "%omWi>,0")
		 (match_operand:HI 2 "general_simpleoperand" "i,omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"mulw\",0,IG_TRIPLE);")


(define_insn "mulqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<,=omB<")
	(mult:QI (match_operand:QI 1 "general_simpleoperand" "%omBi>,0")
		 (match_operand:QI 2 "general_simpleoperand" "i,omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"mulb\",0,IG_TRIPLE);")


(define_insn "muldi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<,=omq<")
	(mult:DI (match_operand:DI 1 "general_simpleoperand" "%omqi>,0")
		 (match_operand:DI 2 "general_simpleoperand" "i,omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"mulq\",\"mulwq\",IG_TRIPLE);")


(define_insn "mulsf3"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<,=omZ<")
	(mult:SF (match_operand:SF 1 "general_simpleoperand" "%omZ>,0")
		 (match_operand:SF 2 "general_simpleoperand" "omZs>,omZs>")))]
  ""
  "* return output_general(operands,\"movef\",\"mulf\",0,IG_CANSWAP | IG_TRIPLE);")



(define_insn "muldf3"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<,=omf<")
	(mult:DF (match_operand:DF 1 "general_simpleoperand" "%omf>,0")
		 (match_operand:DF 2 "general_simpleoperand" "omfs>,omfs>")))]
  ""
  "* return output_general(operands,\"moved\",\"muld\",0,IG_CANSWAP | IG_TRIPLE);")




;; ******************************
;; @div instructions
;; ******************************


(define_insn "divsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(div:SI (match_operand:SI 1 "general_simpleoperand" "0")
		 (match_operand:SI 2 "general_simpleoperand" "omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"divl\",\"divwl\",0);")


(define_insn "divhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(div:HI (match_operand:HI 1 "general_simpleoperand" "0")
		 (match_operand:HI 2 "general_simpleoperand" "omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"divw\",0,0);")


(define_insn "divqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(div:QI (match_operand:QI 1 "general_simpleoperand" "0")
		 (match_operand:QI 2 "general_simpleoperand" "omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"divb\",0,0);")


(define_insn "divdi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(div:DI (match_operand:DI 1 "general_simpleoperand" "0")
		 (match_operand:DI 2 "general_simpleoperand" "omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"divq\",\"divwq\",0);")



(define_insn "divsf3"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<")
	(div:SF (match_operand:SF 1 "general_simpleoperand" "0")
		 (match_operand:SF 2 "general_simpleoperand" "omZs>")))]
  ""
  "* return output_general(operands,\"movef\",\"divf\",0,0);")


(define_insn "divdf3"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<")
	(div:DF (match_operand:DF 1 "general_simpleoperand" "0")
		 (match_operand:DF 2 "general_simpleoperand" "omfs>")))]
  ""
  "* return output_general(operands,\"moved\",\"divd\",0,0);")


(define_insn "udivsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(udiv:SI (match_operand:SI 1 "general_simpleoperand" "0")
		 (match_operand:SI 2 "general_simpleoperand" "omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"divul\",0,0);")


(define_insn "udivhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(udiv:HI (match_operand:HI 1 "general_simpleoperand" "0")
		 (match_operand:HI 2 "general_simpleoperand" "omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"divuw\",0,0);")


(define_insn "udivqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(udiv:QI (match_operand:QI 1 "general_simpleoperand" "0")
		 (match_operand:QI 2 "general_simpleoperand" "omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"divub\",0,0);")


(define_insn "udivdi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(udiv:DI (match_operand:DI 1 "general_simpleoperand" "0")
		 (match_operand:DI 2 "general_simpleoperand" "omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"divuq\",0,0);")




;; ******************************
;; @mod instructions
;; ******************************


(define_insn "modsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(mod:SI (match_operand:SI 1 "general_simpleoperand" "0")
		 (match_operand:SI 2 "general_simpleoperand" "omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"modl\",\"modwl\",0);")


(define_insn "modhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(mod:HI (match_operand:HI 1 "general_simpleoperand" "0")
		 (match_operand:HI 2 "general_simpleoperand" "omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"modw\",0,0);")


(define_insn "modqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(mod:QI (match_operand:QI 1 "general_simpleoperand" "0")
		 (match_operand:QI 2 "general_simpleoperand" "omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"modb\",0,0);")


(define_insn "moddi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(mod:DI (match_operand:DI 1 "general_simpleoperand" "0")
		 (match_operand:DI 2 "general_simpleoperand" "omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"modq\",\"modwq\",0);")



(define_insn "modsf3"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<")
	(mod:SF (match_operand:SF 1 "general_simpleoperand" "0")
		 (match_operand:SF 2 "general_simpleoperand" "omZs>")))]
  ""
  "* return output_general(operands,\"movef\",\"modf\",0,0);")


(define_insn "moddf3"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<")
	(mod:DF (match_operand:DF 1 "general_simpleoperand" "0")
		 (match_operand:DF 2 "general_simpleoperand" "omfs>")))]
  ""
  "* return output_general(operands,\"moved\",\"modd\",0,0);")


(define_insn "umodsi3"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(umod:SI (match_operand:SI 1 "general_simpleoperand" "0")
		 (match_operand:SI 2 "general_simpleoperand" "omdi>")))]
  ""
  "* return output_general(operands,\"movel\",\"modul\",0,0);")


(define_insn "umodhi3"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(umod:HI (match_operand:HI 1 "general_simpleoperand" "0")
		 (match_operand:HI 2 "general_simpleoperand" "omWi>")))]
  ""
  "* return output_general(operands,\"movew\",\"moduw\",0,0);")


(define_insn "umodqi3"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(umod:QI (match_operand:QI 1 "general_simpleoperand" "0")
		 (match_operand:QI 2 "general_simpleoperand" "omBi>")))]
  ""
  "* return output_general(operands,\"moveb\",\"modub\",0,0);")


(define_insn "umoddi3"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(umod:DI (match_operand:DI 1 "general_simpleoperand" "0")
		 (match_operand:DI 2 "general_simpleoperand" "omqi>")))]
  ""
  "* return output_general(operands,\"moveq\",\"moduq\",0,0);")




;;*************************
;; @negation instructions
;;*************************


(define_insn "negsi2"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(neg:SI (match_operand:SI 1 "general_simpleoperand" "uomdi>")))]
  ""
  "*return output_unary(operands,\"negl\");")

(define_insn "neghi2"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(neg:HI (match_operand:HI 1 "general_simpleoperand" "uomWi>")))]
  ""
  "*return output_unary(operands,\"negw\");")

(define_insn "negqi2"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(neg:QI (match_operand:QI 1 "general_simpleoperand" "uomBi>")))]
  ""
  "*return output_unary(operands,\"negb\");")


(define_insn "negdi2"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(neg:DI (match_operand:DI 1 "general_simpleoperand" "uomqi>")))]
  ""
  "*return output_unary(operands,\"negq\");")

(define_insn "negdf2"
  [(set (match_operand:DF 0 "general_simpleoperand" "=omf<")
	(neg:DF (match_operand:DF 1 "general_simpleoperand" "uomfs>")))]
  ""
  "*return output_unary(operands,\"negd\");")

(define_insn "negsf2"
  [(set (match_operand:SF 0 "general_simpleoperand" "=omZ<")
	(neg:SF (match_operand:SF 1 "general_simpleoperand" "uomZs>")))]
  ""
  "*return output_unary(operands,\"negf\");")



;;*************************
;; @not instructions
;;*************************


(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "general_simpleoperand" "=omd<")
	(not:SI (match_operand:SI 1 "general_simpleoperand" "uomdi>")))]
  ""
  "*return output_unary(operands,\"notl\");")

(define_insn "one_cmplhi2"
  [(set (match_operand:HI 0 "general_simpleoperand" "=omW<")
	(not:HI (match_operand:HI 1 "general_simpleoperand" "uomWi>")))]
  ""
  "*return output_unary(operands,\"notw\");")

(define_insn "one_cmplqi2"
  [(set (match_operand:QI 0 "general_simpleoperand" "=omB<")
	(not:QI (match_operand:QI 1 "general_simpleoperand" "uomBi>")))]
  ""
  "*return output_unary(operands,\"notb\");")


(define_insn "one_cmpldi2"
  [(set (match_operand:DI 0 "general_simpleoperand" "=omq<")
	(not:DI (match_operand:DI 1 "general_simpleoperand" "uomqi>")))]
  ""
  "*return output_unary(operands,\"notq\");")


;;***************************
;; @sin instructions
;;***************************


(define_insn "sindf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(sin:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"sind\");")

(define_insn "sinsf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(sin:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"sinf\");")


;;***************************
;; @cos instructions
;;***************************


(define_insn "cosdf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(cos:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"cosd\");")

(define_insn "cossf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(cos:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"cosf\");")


;;***************************
;; @tan instructions
;;***************************


(define_insn "tandf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(tan:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"tand\");")

(define_insn "tansf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(tan:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"tanf\");")


;;***************************
;; @asin instructions
;;***************************


(define_insn "asindf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(asin:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"asind\");")

(define_insn "asinsf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(asin:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"asinf\");")


;;***************************
;; @acos instructions
;;***************************


(define_insn "acosdf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(acos:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"acosd\");")

(define_insn "acossf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(acos:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"acosf\");")


;;***************************
;; @atan instructions
;;***************************


(define_insn "atandf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(atan:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"atand\");")

(define_insn "atansf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(atan:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"atanf\");")

;;***************************
;; @fabs instructions
;;***************************


(define_insn "fabsdf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(fabs:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"moved\",\"absd\");")

(define_insn "fabssf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(fabs:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  "flag_fast_math"
  "*return output_doublestackop(operands,\"movef\",\"absf\");")



;;***************************
;; @sqrt instructions
;;***************************


(define_insn "sqrtdf2"
  [(set (match_operand:DF 0 "general_operand" "=omf<")
	(sqrt:DF (match_operand:DF 1 "general_operand" "omfs>")))]
  ""
  "*return output_doublestackop(operands,\"moved\",\"sqrtd\");")

(define_insn "sqrtsf2"
  [(set (match_operand:SF 0 "general_operand" "=omZ<")
	(sqrt:SF (match_operand:SF 1 "general_operand" "omZs>")))]
  ""
  "*return output_doublestackop(operands,\"movef\",\"sqrtf\");")




;;***************************
;; @compare instructions
;;***************************


(define_insn "cmpsi"
  [(set (cc0)
	(compare (match_operand:SI 0 "nonimmediate_simpleoperand" "omd>")
		 (match_operand:SI 1 "general_simpleoperand" "omdi>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")


(define_insn "cmphi"
  [(set (cc0)
	(compare (match_operand:HI 0 "nonimmediate_simpleoperand" "omW>")
		 (match_operand:HI 1 "general_simpleoperand" "omWi>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")

(define_insn "cmpqi"
  [(set (cc0)
	(compare (match_operand:QI 0 "nonimmediate_simpleoperand" "omB>")
		 (match_operand:QI 1 "general_simpleoperand" "omBi>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")


(define_insn "cmpdi"
  [(set (cc0)
	(compare (match_operand:DI 0 "nonimmediate_simpleoperand" "omq>")
		 (match_operand:DI 1 "general_simpleoperand" "omqi>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")


(define_insn "cmpdf"
  [(set (cc0)
	(compare (match_operand:DF 0 "nonimmediate_simpleoperand" "omf>")
		 (match_operand:DF 1 "general_simpleoperand" "omfs>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")

(define_insn "cmpsf"
  [(set (cc0)
	(compare (match_operand:SF 0 "nonimmediate_simpleoperand" "omZ>")
		 (match_operand:SF 1 "general_simpleoperand" "omZs>")))]
  ""
  "*{
  r_compare_op0 = operands[0]; /* set up for compare and branch instruction */
  r_compare_op1 = operands[1];
  return \"\";
}")




;; ******************************
;; Basic conditional @jump instructions.
;; ******************************


(define_insn "beq"
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
    return buildcbcc(operands,EQ);
}")

(define_insn "bne"
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
    return buildcbcc(operands,NE);
}")

(define_insn "bgt"
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,GT);
")

(define_insn "bgtu"
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,GTU);
")

(define_insn "blt"
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,LT);
")

(define_insn "bltu"
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,LTU);
")

(define_insn "bge"
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,GE);
")

(define_insn "bgeu"
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,GEU);
")

(define_insn "ble"
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,LE);
")

(define_insn "bleu"
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
    return buildcbcc(operands,LEU);
")

;; Negated conditional jump instructions.

(define_insn ""
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
    return buildcbcc(operands,NE);
}")

(define_insn ""
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
    return buildcbcc(operands,EQ);
}")

(define_insn ""
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,LE);
")

(define_insn ""
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,LEU);
")

(define_insn ""
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,GE);
")

(define_insn ""
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,GEU);
")

(define_insn ""
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,LT);
")

(define_insn ""
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,LTU);
")

(define_insn ""
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,GT);
")

(define_insn ""
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
    return buildcbcc(operands,GTU);
")

