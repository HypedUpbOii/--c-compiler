**PROCEDURE: main
**BEGIN: Three Address Code Statements
	a_ = 5
	b_ = 3
	c_ = 7
	d_ = 2
	p_()
	temp0 = a_ + 2
	a_ = temp0
	temp1 = c_ + d_
	e_ = temp1
	temp2 = a_ * b_
	d_ = temp2
	temp3 = a_ + c_
	temp4 = temp3 + e_
	x_ = temp4
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	b_ = 2
	temp0 = b_ < d_
	temp2 = ! temp0
	if(temp2) goto Label1
	temp1 = a_ + b_
	c_ = temp1
	goto Label0
Label1:
	q_()
Label0:
	temp3 = c_ + d_
	x_ = temp3
**END: Three Address Code Statements
**PROCEDURE: q_
**BEGIN: Three Address Code Statements
	a_ = 1
	temp0 = a_ * b_
	a_ = temp0
**END: Three Address Code Statements
