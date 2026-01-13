**PROCEDURE: main
**BEGIN: Three Address Code Statements
	p_()
	write  a_
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	temp0 = 1 > 2
	temp1 = ! temp0
	if(temp1) goto Label0
	a_ = 5
	p_()
	write  b_
	goto Label0
Label0:
**END: Three Address Code Statements
