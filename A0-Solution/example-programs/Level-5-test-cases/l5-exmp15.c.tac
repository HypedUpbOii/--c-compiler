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
	temp0 = x_ > 0
	temp1 = ! temp0
	if(temp1) goto Label1
	q_()
	goto Label0
Label1:
Label2:
	temp2 = a_ > b_
	temp4 = ! temp2
	if(temp4) goto Label3
	write  a_
	temp3 = c_ + 1
	x_ = temp3
	goto Label2
Label3:
Label0:
	write  b_
	temp5 = c_ + b_
	a_ = temp5
**END: Three Address Code Statements
**PROCEDURE: q_
**BEGIN: Three Address Code Statements
	write  a_
**END: Three Address Code Statements
