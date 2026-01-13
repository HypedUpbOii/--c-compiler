**PROCEDURE: main
**BEGIN: Three Address Code Statements
	a_ = 2
	D1234_ = 3
	temp0 = a_ > 5
	temp1 = ! temp0
	if(temp1) goto Label2
	stemp0 = 10
	goto Label0
	goto Label1
Label2:
	stemp0 = a_
	goto Label0
Label1:
Label0:
	 return stemp0
**END: Three Address Code Statements
