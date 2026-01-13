**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
	stemp0 = p_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	temp0 = 10 > 5
	x_ = temp0
	temp1 = f1_(x_)
	x_ = temp1
**END: Three Address Code Statements
