#ifndef DEFS_H_
#define DEFS_H_

#define ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#define ST_BIND(val) (val >> 4)
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
#define ARITHMETIC 0x50
#define ADD ARITHMETIC
#define SUB 0x51
#define MUL 0x52
#define DIV 0x53
#define LOGICAL 0x60
#define NOT LOGICAL
#define AND 0x61
#define OR 0x62
#define XOR 0x63
#define SHL 0x70
#define SHR 0x71
#define ST 0x80
#define LD 0x90
#define CALL 0x20
#define JMP 0x30
#define BEQ 0x39
#define BNE 0x3A
#define BGT 0x3B

#define MAX_DISPLACEMENT 1 << 12

enum {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15};
enum {sp = 14, pc = 15};
enum {status, handler, cause};

#endif // DEFS_H_