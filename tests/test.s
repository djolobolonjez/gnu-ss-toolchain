.global a,b,c
.global d
.section my_section
a: .global e
.extern f
.skip 4
b: .word b, 16
d: .skip 8
e: .word e, j, 0x10
k: xchg %r1, %r2
int
j: 

div %r11, %r13
not %r2
add %r1, %r2
ld %r1, %r2
.section other_section
.word 8, i
c: .extern h
halt
i: .end
