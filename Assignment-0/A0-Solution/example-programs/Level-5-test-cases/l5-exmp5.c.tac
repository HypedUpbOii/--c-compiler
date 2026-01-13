**PROCEDURE: func1_
**BEGIN: Three Address Code Statements
	i2_ = 5
	f2_ = 5.00
**END: Three Address Code Statements
**PROCEDURE: func2_
**BEGIN: Three Address Code Statements
	temp0 = a_ + 10
	a_ = temp0
	i1_ = 5
	temp1 = a_ + i1_
	temp2 = temp1 + i2_
	temp3 = temp2 + 2
	stemp0 = temp3
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: func3_
**BEGIN: Three Address Code Statements
	temp0 = b_ + f2_
	b_ = temp0
	temp1 = c_ + 10.50
	c_ = temp1
	temp2 = b_ + c_
	stemp0 = temp2
	goto Label1
Label1:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: func4_
**BEGIN: Three Address Code Statements
	i1_ = 4
	temp0 = i1_ + i1_
	stemp0 = temp0
	goto Label2
Label2:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	func1_()
	temp0 = func4_()
	i2_ = temp0
	temp1 = func2_(i1_, f2_, i2_)
	i2_ = temp1
	temp2 = func3_(i1_, f1_, f2_)
	f2_ = temp2
	write  f1_
	write  f2_
**END: Three Address Code Statements
