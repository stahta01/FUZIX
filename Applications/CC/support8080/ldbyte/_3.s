
	.export __ldbyte3

	.setcpu 8080
	.code
__ldbyte3:
	lxi h,3

	mov l,m
	ret