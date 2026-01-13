**PROCEDURE: main
**BEGIN: Three Address Code Statements
	temp0 = a_ * b_
	c_ = temp0
	p_()
	p_()
	temp1 = a_ * b_
	a_ = temp1
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	temp0 = 1 < 2
	temp2 = ! temp0
	if(temp2) goto Label1
	temp1 = a_ * b_
	a_ = temp1
	goto Label0
Label1:
	temp3 = 1 < 4
	temp5 = ! temp3
	if(temp5) goto Label2
	temp4 = a_ * b_
	c_ = temp4
	c_ = a_
	goto Label2
Label2:
Label0:
**END: Three Address Code Statements
