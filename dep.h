#ifndef GLIBC_DEPENDS__
#define GLIBC_DEPENDS__

#define USE_MULTIARCH

#define IS_IN(lib)    (1)

# define C_SYMBOL_NAME(name) name
#define ALIGNARG(log2) 1<<log2
# define C_LABEL(name)	name##:
# define cfi_startproc			.cfi_startproc
# define _CET_ENDBR
#define CALL_MCOUNT		/* Do nothing.  */

#define L(body) .L##body


#define	ENTRY(name)							      \
  .globl C_SYMBOL_NAME(name);						      \
  .type C_SYMBOL_NAME(name),@function;					      \
  .align ALIGNARG(4);							      \
  C_LABEL(name)								      \
  cfi_startproc;							      \
  _CET_ENDBR;								      \
  CALL_MCOUNT

# define cfi_endproc			.cfi_endproc
#define ASM_SIZE_DIRECTIVE(name) .size name,.-name;

#define END(name)							      \
  cfi_endproc;								      \
  ASM_SIZE_DIRECTIVE(name)


#endif //GLIBC_DEPENDS__
