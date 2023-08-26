.global a,b,c
.global d
.section my_section
call a
halt
a: int 
.global e
.global f
.skip 4
b: .word b, 16
d: .skip 8
e: .word e, j, 0x10
#k: xchg %r1, %r2
call x
int
j: 

#div %r11, %r13
#not %r2
x:add %r1, %r2
beq %r1, %r2, x
#ld [%r3], %r2
#ld $50, %r2
jmp y
ld [%r3+45], %r1
ld 75, %r2
st %r1, [%r3]
call c
#st %r1, b
st %r1, $b
st %r3, [%r4+75]
push %r2

pop %r2
.section other_section
xchg %r1, %r2
jmp j
#halt
.word 8, i
ret
csrrd %status, %r13
c: .extern h
f: xchg %r1, %r2
jmp h
halt
i: .end
