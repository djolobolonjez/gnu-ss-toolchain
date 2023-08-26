.global y, h
.section my_section
a:
.extern f
.skip 4
b: .word b, 16
d: .skip 8
e: .word e, j, 0x10
y: xchg %r1, %r2
xchg %r2, %r3
jmp f
h:halt
j: 
halt
.end