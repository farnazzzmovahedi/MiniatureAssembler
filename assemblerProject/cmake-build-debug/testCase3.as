	nand	4,5,6	#invalid offset in Line 25
	add	0,1,2
	slt	2,3,4
	sub	1,2,3
	or	3,4,5
	ori	7,8,9
	addi	5,6,7
	slti	6,7,8
	sw	10,11,13
	lui	10,11
	lw	12,13,14
	jalr	14,15
	beq	10,10,10
	j	11
	add	1,2,2
	halt
	nand	2,3,6
	sub	1,2,3
	sw	0,1,12
	or	3,2,11
	slti	2,7,2
	addi	4,2,5
	slt	2,2,2
	lui	0,12
	beq	1,1,66000
	lw	0,1,14
	jalr	4,15
	j	14
	halt
	slt	5,1,2