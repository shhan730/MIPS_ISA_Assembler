# MIPS ISA Assembler



## Introduction

### 1. Overview

This project is to implement a MIPS (subset) ISA assembler. 

The assembler is the tool which converts assembly codes to a binary file. 

This is a simplified assembler which do not support the linking process, and In this project, only one assemble file will be the whole program.

This program implements the assembler which can convert a subset of the instruction set shown in the following table. 

In addition, assembler handles labels for jump/branch targets, and labels for the static data section.



### 2. Instuction Set

The detailed information regarding instructions are in the attached [MIPS_Green_Sheet.pdf](MIPS_Green_Sheet.pdf) page.

![Instructions](./README_Image/Instructions.png)



- Only instructions for unsigned operations need to be implemented. (addu, addiu, subu, sltiu, sltu, sll, srl)

- However, the immediate fields for certain instructions are sign extended to allow negative numbers (addui, beq, bne, lw, sw, sltui)

- Only loads and stores with 4B word need to be implemented.

- The assembler must support decimal and hexadecimal numbers (0x) for the immediate field, and .data section.

- The register name is always “$n” n is from 0 to 31.

- la (load address) is a pseudo instruction; it should be converted to one or two assembly instructions.

  la $2, VAR1: VAR1 is a label in the data section 

  ➜ It should be converted to lui and ori instructions.

  lui \$register, upper 16bit address 

  ori $register, lower 16bit address

  If the lower 16bit address is 0x0000, the ori instruction is useless. 

  Case1) load address is 0x1000 0000

  ​	lui $2, 0x1000

  Case2) load address is 0x1000 0004

  ​	lui \$2, 0x1000

  ​	ori \$2, $2, 0x0004

  

  ## 3. Usage Example & Testing

  

  ### 3-1) Directives

  .text

  \- indicates that following items are stored in the user text segment, typically instructions - It always starts from 0x400000

  .data

  \- indicates that following data items are stored in the data segment - It always starts from 0x10000000

  .word

  \- store n 32-bit quantities in successive memory words

​		<img src="/Users/hanseunghun/Google 드라이브/2020-1/컴구 (1)/sce212-project1-master/README_Image/Memory_Allocation.png" alt="Memory_Allocation" style="zoom:50%;" />

### 	3-2) Running Test

#### Test Codes are provided, you can use that to test the program.

* **Input**

```bash
$ make
$ make test
```

* **Output**

```bash
gcc -g -Wall   -c -o assembler.o assembler.c
gcc -o assembler assembler.o

Testing example1
	Test seems correct

Testing example2_mod
	Test seems correct

Testing example3
	Test seems correct

Testing example4
	Test seems correct

Testing example5
	Test seems correct
```



### 	3-3) Testing details

The output of the assembler is an object file. We use a simplified custom format.
 	\- The first two words (32bits) are the size of text section, and data section.
 	\- The next bytes are the instructions in binary. 
		The length must be equal to the specified text section length.
	 \- After the text section, the rest of bytes are the initial values of the data section.

​	The following must be the final binary format:

<img src="/Users/hanseunghun/Google 드라이브/2020-1/컴구 (1)/sce212-project1-master/README_Image/Output_format.png" alt="Output_format" style="zoom:50%;" />





* example1.s

**INPUT_FILE**

```assembly
	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.text
main:
	and	$17, $17, $0
	and	$18, $18, $0
	la	$8, data1
	la	$9, data2
	and	$10, $10, $0
lab1:
	and	$11, $11, $0
lab2:
	addiu	$17, $17, 0x1
	addiu	$11, $11, 0x1
	or	$9, $9, $0
	bne	$11, $8, lab2
lab3:
	addiu	$18, $18, 0x2
	addiu	$11, $11, 1
	sll	$18, $17, 1
	srl	$17, $18, 1
	and	$19, $17, $18
	bne	$11, $9, lab3
lab4:
	addu	$5, $5, $31
	nor	$16, $17, $18
	beq	$10, $8, lab5
	j	lab1
lab5:
	ori	$16, $16, 0xf0f0
```

**OUTPUT_FILE**

```bash
000000000000000000000000010110000000000000000000000000000000110000000010001000001000100000100100000000100100000010010000001001000011110000001000000100000000000000111100000010010001000000000000001101010010100100000000000001000000000101000000010100000010010000000001011000000101100000100100001001100011000100000000000000010010010101101011000000000000000100000001001000000100100000100101000101010110100011111111111111000010011001010010000000000000001000100101011010110000000000000001000000000001000110010000010000000000000000010010100010000100001000000010001100101001100000100100000101010110100111111111111110100000000010111111001010000010000100000010001100101000000000100111000100010100100000000000000000010000100000010000000000000000011000110110000100001111000011110000000000000000000000000000011001000000000000000000000000001100100000010010001101000101011001111000
```



* example2.s

**INPUT_FILE**

```assembly
	.data
array:	.word	3
	.word	123
	.word	4346
array2:	.word	0x11111111
	.text
main:
	addiu	$2, $0, 1024
	addu	$3, $2, $2
	or	$4, $3, $2
	sll	$6, $5, 16
	addiu	$7, $6, 9999
	subu	$8, $7, $2
	nor	$9, $4, $3
	ori	$10, $2, 255
	srl	$11, $6, 5
	la	$4, array2
	and	$13, $11, $5
	andi	$14, $4, 100
	lui	$17, 100
	addiu	$2, $0, 0xa

```

**OUTPUT_FILE**

```bash
000000000000000000000000001111000000000000000000000000000001000000100100000000100000010000000000000000000100001000011000001000010000000001100010001000000010010100000000000001010011010000000000001001001100011100100111000011110000000011100010010000000010001100000000100000110100100000100111001101000100101000000000111111110000000000000110010110010100001000111100000001000001000000000000001101001000010000000000000011000000000101100101011010000010010000110000100011100000000001100100001111000001000100000000011001000010010000000010000000000000101000000000000000000000000000000011000000000000000000000000011110110000000000000000000100001111101000010001000100010001000100010001
```



* example3.s

**INPUT_FILE**

```assembly
	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.word	0x12341
	.text
main:
	lui	$3, 0x1000
	lw	$5, 0($3)
	lw	$8, 4($3)
	lw	$9, 8($3)
	lw	$10, 12($3)
	addiu	$5, $5, 24
	addiu	$6, $0, 124
	addu	$7, $5, $6
	sw	$5, 16($3)
	sw	$6, 20($3)
	sw	$7, 24($3)
	addiu	$3, $3, 12
	lw	$12, -4($3)
	lw	$13, -8($3)
	lw	$14, -12($3)

```

**OUTPUT_FILE**

```bash
000000000000000000000000001111000000000000000000000000000001000000111100000000110001000000000000100011000110010100000000000000001000110001101000000000000000010010001100011010010000000000001000100011000110101000000000000011000010010010100101000000000001100000100100000001100000000001111100000000001010011000111000001000011010110001100101000000000001000010101100011001100000000000010100101011000110011100000000000110000010010001100011000000000000110010001100011011001111111111111100100011000110110111111111111110001000110001101110111111111111010000000000000000000000000001100100000000000000000000000000110010000001001000110100010101100111100000000000000000010010001101000001
```



* example4.s

**INPUT_FILE**

```assembly
	.data
	.text
main:
	addu	$2, $4, $5
	addu	$2, $6, $7
	subu	$9, $3, $2
lab1:
	and	$11, $11, $0
	addiu	$10, $10, 0x1
	or	$6, $6, $0
	jal	lab3
lab3:
	sll	$7, $6, 2
	srl	$5, $4, 2
	sltiu	$9, $10, 100
	beq	$9, $0, lab4
	jr	$31
lab4:
	sltu	$4, $2, $3
	bne	$4, $0, lab5
	j	lab1
lab5:
	ori	$16, $16, 0xf0f0

```

**OUTPUT_FILE**

```bash
000000000000000000000000010000000000000000000000000000000000000000000000100001010001000000100001000000001100011100010000001000010000000001100010010010000010001100000001011000000101100000100100001001010100101000000000000000010000000011000000001100000010010100001100000100000000000000000111000000000000011000111000100000000000000000000100001010001000001000101101010010010000000001100100000100010010000000000000000000010000001111100000000000000000100000000000010000110010000000101011000101001000000000000000000000010000100000010000000000000000001100110110000100001111000011110000
```



* example5.s

**INPUT_FILE**

```assembly
	.data
data1:	.word	0x12c
data2:	.word	0xc8
	.text
main:
	and	$10, $10, $0
	and	$11, $11, $0
	la	$8, data1
	la	$9, data2
	addiu	$10, $10, 0x1
	sll	$10, $10, 1
	sll	$11, $11, 1
loop:
	addiu	$10, $10, 0x1
	addiu	$11, $11, 1
	or	$9, $9, $0
	subu	$18, $18, $10
	sll	$18, $17, 1
	sll	$17, $18, 1
	addu	$11, $11, $31
	nor	$16, $17, $18	
	bne	$11, $8, loop
	j	exit
exit:
	andi	$15, $15, 0x0f

```

**OUTPUT_FILE**

```bash
0000000000000000000000000100110000000000000000000000000000001000000000010100000001010000001001000000000101100000010110000010010000111100000010000001000000000000001111000000100100010000000000000011010100101001000000000000010000100101010010100000000000000001000000000000101001010000010000000000000000001011010110000100000000100101010010100000000000000001001001010110101100000000000000010000000100100000010010000010010100000010010010101001000000100011000000000001000110010000010000000000000000010010100010000100000000000001011111110101100000100001000000100011001010000000001001110001010101101000111111111111011100001000000100000000000000010010001100011110111100000000000011110000000000000000000000010010110000000000000000000000000011001000
```

