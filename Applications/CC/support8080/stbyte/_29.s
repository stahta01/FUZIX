
	.export __stbyte29

	.setcpu 8080
	.code
__stbyte29:
	mov a,l
	lxi h,29

	mov m,a
	mov l,a
	ret