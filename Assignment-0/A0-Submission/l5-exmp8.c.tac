**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
	stemp0 = s_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	s1_ = "Hello"
	temp0 = f1_(s1_)
	s2_ = temp0
	write  s1_
**END: Three Address Code Statements
