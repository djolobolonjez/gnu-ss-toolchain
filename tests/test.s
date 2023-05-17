.global a,b,c
.global d
.section my_section
a: .global e
.equ k, 20
.extern f
.skip 4
#.ascii "Hello world!\n"
b: .word b, 16
.equ l, a + b - 20
d: .skip 8
e: .word e, j, 0x10
j:
.section other_section
.word 8
c: .extern h
i: .end
