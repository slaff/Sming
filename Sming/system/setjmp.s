/*
	setjmp.S
	
	replacement for the closed-source version (Call0 ABI) included with the 
	ESP8266 SDK, see also include/machine/setjmp.h 
	
	(tius2000)
*/


	.text
	
//=============================================================================
//	int _setjmp(jmp_buf context)
	
	.align	4
	.literal_position
	.global	setjmp
	.type	setjmp, @function

setjmp:	
	s32i.n  a0, a2, 0	
	s32i.n  a1, a2, 4
	s32i.n  a12, a2, 8
	s32i.n  a13, a2, 12
	s32i.n  a14, a2, 16
	s32i.n  a15, a2, 20
	
	movi.n  a2, 0
	
	ret.n
	
	.size	setjmp, . - setjmp
	

//=============================================================================
//	void _longjmp(jmp_buf context, int val)
	
	.align	4
	.literal_position
	.global	longjmp
	.type	longjmp, @function
	
longjmp:
	l32i.n  a0, a2, 0
	l32i.n  a12, a2, 8
	l32i.n  a13, a2, 12
	l32i.n  a14, a2, 16
	l32i.n  a15, a2, 20
	l32i.n  a1, a2, 4
	
	//	if (!val) val = 1
	movi.n  a2, 1
	movnez  a2, a3, a3
	
	ret.n
	
	.size	longjmp, .-longjmp
