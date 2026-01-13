**PROCEDURE: main
**BEGIN: Three Address Code Statements
	temp0 = a_ * b_
	c_ = temp0
	temp1 = p_(a_)
	a_ = temp1
	temp2 = p_(c_)
	c_ = temp2
	temp3 = a_ * b_
	a_ = temp3
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	temp0 = k_ < 0
	temp1 = ! temp0
	if(temp1) goto Label1
	stemp0 = 1
	goto Label0
	goto Label1
Label1:
	temp2 = 1 < 2
	temp6 = ! temp2
	if(temp6) goto Label3
	temp4 = k_ - 1
	temp3 = p_(temp4)
	b_ = temp3
	temp5 = a_ * b_
	a_ = temp5
	goto Label2
Label3:
	temp7 = 1 < 4
	temp11 = ! temp7
	if(temp11) goto Label4
	temp8 = a_ * b_
	c_ = temp8
	temp10 = k_ - 1
	temp9 = p_(temp10)
	c_ = temp9
	goto Label4
Label4:
Label2:
	stemp0 = 0
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
