**PROCEDURE: main
**BEGIN: Three Address Code Statements
	r_()
	a_ = 1
	c_ = 2
	p_()
	write  a_
	write  c_
	q_()
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
	a_ = 1
**END: Three Address Code Statements
**PROCEDURE: q_
**BEGIN: Three Address Code Statements
	p_()
	write  a_
**END: Three Address Code Statements
**PROCEDURE: r_
**BEGIN: Three Address Code Statements
	p_()
	write  b_
**END: Three Address Code Statements
