**PROCEDURE: main
**BEGIN: Three Address Code Statements
	temp0 = b_ + c_
	a_ = temp0
Label1:
	temp1 = a_ > d_
	temp3 = ! temp1
	if(temp3) goto Label2
	temp2 = a_ + 2
	a_ = temp2
	goto Label1
Label2:
	x_ = "hello"
	temp4 = p_(x_)
	z_ = temp4
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	a_ = 2
	y_ = "Hello"
	write  b_
	temp0 = a_ > c_
	temp1 = ! temp0
	if(temp1) goto Label4
	write  b_
	goto Label3
Label4:
	write  a_
Label3:
	x_ = y_
	stemp0 = x_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
