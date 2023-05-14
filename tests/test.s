.global a,b,c
.global d
.section my_section
a: .global e
.extern f
b: .extern g
.section other_section
c: .extern h
i: .end
