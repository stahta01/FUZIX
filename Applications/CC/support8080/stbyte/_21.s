
	.export __stbyte21

	.setcpu 8080
	.code
__stbyte21:
	mov a,l
	lxi h,21

	mov m,a
	mov l,a
	ret