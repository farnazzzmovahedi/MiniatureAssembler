	sub	2,2,2		#undefined Label in Line 11
	add	1,2,3
	nand	3,2,1
	beq	1,1,lbl1
	halt
	lw	2,2,5
	sw	1,2,3
	beq	5,2,lbl2	
lbl1	.fill	6
lbl2	.fill	4
	lw	1,5,lbl3