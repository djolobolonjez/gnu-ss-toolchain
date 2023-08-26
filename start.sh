ASSEMBLER=assembler
LINKER=linker

${ASSEMBLER} -o test.o test.s
${ASSEMBLER} -o test2.o test2.s
${LINKER} -hex \
  -place=my_section@0x40000000 -place=other_section@0xF0000000 \
  -o program.hex \
  test.o test2.o