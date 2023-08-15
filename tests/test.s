.global a,b,c
.global d
.section my_section
a: .global e
.extern f
.skip 4
b: .word b, 16
d: .skip 8
e: .word e, j, 0x10
#k: xchg %r1, %r2
int
j: 

#div %r11, %r13
#not %r2
x:add %r1, %r2
beq %r1, %r2, x
ld [%r3], %r2
ld $50, %r2
jmp 0x4c
ld [%r3+45], %r1
ld 75, %r2
y: st %r1, [%r3]
call c
#st %r1, b
st %r1, $b
st %r3, [%r4+75]
push %r2

pop %r2
.section other_section
.word 8, i
ret
csrrd %status, %r13
c: .extern h
iret
halt
i: .end
