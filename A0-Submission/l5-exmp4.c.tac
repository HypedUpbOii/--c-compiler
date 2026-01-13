**PROCEDURE: f_
**BEGIN: Three Address Code Statements
	temp0 = n_ == 0
	temp1 = ! temp0
	if(temp1) goto Label1
	ret_ = 1
	stemp0 = ret_
	goto Label0
	goto Label1
Label1:
	temp2 = n_ - 1
	k_ = temp2
	temp3 = f_(k_)
	ret_ = temp3
	temp4 = n_ * ret_
	ret_ = temp4
	stemp0 = ret_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	b_ = 5
	temp0 = f_(b_)
	a_ = temp0
	write  a_
	m_ = 2
	write  m_
**END: Three Address Code Statements
