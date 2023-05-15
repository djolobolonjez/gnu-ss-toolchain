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

#define UND 0

#endif // DEFS_H_