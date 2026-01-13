**PROCEDURE: main
**BEGIN: Three Address Code Statements
	temp0 = b_ * c_
	a_ = temp0
	p_()
	temp1 = a_ + c_
	b_ = temp1
	write  a_
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
Label0:
	temp0 = a_ > b_
	temp2 = ! temp0
	if(temp2) goto Label1
	write  a_
	temp1 = c_ + 1
	x_ = temp1
	goto Label0
Label1:
	write  b_
	temp3 = c_ + b_
	a_ = temp3
**END: Three Address Code Statements
