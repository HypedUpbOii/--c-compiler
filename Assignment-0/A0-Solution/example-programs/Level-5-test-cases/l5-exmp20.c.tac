**PROCEDURE: main
**BEGIN: Three Address Code Statements
	a_ = 1
	b_ = 2
	c_ = 3
	temp0 = b_ * c_
	a_ = temp0
	temp1 = p_(c_)
	a_ = temp1
	temp2 = a_ + c_
	b_ = temp2
	temp3 = p_(b_)
	c_ = temp3
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	temp0 = k_ <= 0
	temp1 = ! temp0
	if(temp1) goto Label1
	stemp0 = 1
	goto Label0
	goto Label1
Label1:
	temp3 = k_ - 1
	temp2 = p_(temp3)
	b_ = temp2
	write  b_
	temp4 = c_ + b_
	a_ = temp4
Label0:
	 return stemp0
**END: Three Address Code Statements
