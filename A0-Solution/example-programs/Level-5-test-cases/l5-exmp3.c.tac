**PROCEDURE: f_
**BEGIN: Three Address Code Statements
	stemp0 = 3.50
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	r_ = 2.00
	temp0 = f_(2, 3)
	r_ = temp0
**END: Three Address Code Statements
