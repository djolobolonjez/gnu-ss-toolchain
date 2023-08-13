#ifndef DEFS_H_
#define DEFS_H_

#define ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#define ST_BIND(val) ((unsigned char)(val >> 4))
#define ST_TYPE(val) ((val) & 0xf)

#define STBIND_LOCAL 0
#define STBIND_GLOBAL 1
#define STBIND_EXTERN 2

#define STTYPE_NOTYPE 0
#define STTYPE_SECTION 1 

#define RELA_INFO(sym, type) ((((unsigned long)(sym)) << 32) + (type))
#define RELA_SYM(val) ((val) >> 32)
#define RELA_TYPE(val) ((val) & 0xffffffff)

#define UND 0

#define ARG_SYM 0
#define ARG_LIT 1
#define ARG_OTHER 2

#define WORD_SIZE 1 << 2
#define BYTEMASK(value) ((unsigned char)value & 0xff)

#define R_X86_64_PC32 0
#define R_X86_64_32 1

#define HALT 0x00000000
#define INT  0x10000000
#define XCHG 0x40
#define ARITHMETIC 0x5
#define LOGICAL 0x6
#define SHL 0x70
#define SHR 0x71
#define ST 0x80
#define LD 0x90
#define CALL 0x20

#define MAX_DISPLACEMENT 1 << 12

#endif // DEFS_H_