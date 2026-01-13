**PROCEDURE: main
**BEGIN: Three Address Code Statements
	a_ = 1
	b_ = 2
	c_ = 3
	temp0 = b_ * c_
	a_ = temp0
	p_()
	temp1 = a_ + c_
	b_ = temp1
	write  a_
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	write  a_
	write  b_
	temp0 = c_ + 1
	b_ = temp0
	write  b_
	temp1 = c_ + b_
	a_ = temp1
**END: Three Address Code Statements
