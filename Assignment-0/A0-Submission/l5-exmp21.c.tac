**PROCEDURE: A_
**BEGIN: Three Address Code Statements
	temp0 = i_ <= 0
	temp1 = j_ <= 0
	temp2 = temp0 || temp1
	temp3 = ! temp2
	if(temp3) goto Label1
	stemp0 = 1
	goto Label0
	goto Label1
Label1:
	temp5 = i_ - 1
	temp4 = A_(temp5, j_)
	x_ = temp4
	stemp0 = x_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
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
Label2:
	temp0 = a_ > b_
	temp3 = ! temp0
	if(temp3) goto Label3
	temp1 = A_(3, 3)
	y_ = temp1
	temp2 = c_ + 1
	x_ = temp2
	goto Label2
Label3:
	write  b_
	temp4 = c_ + b_
	a_ = temp4
**END: Three Address Code Statements
