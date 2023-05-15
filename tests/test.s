.global a,b,c
.global d
.section my_section
a: .global e
.extern f
.skip 4
.ascii "Hello world!\n"
b: .word 043, 0xff
d: .skip 8
e: .word e, j, 0xf
j:
.section other_section
c: .extern h
i: .end
