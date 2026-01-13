**PROCEDURE: convert_centigrade_to_fahrenheit_
**BEGIN: Three Address Code Statements
	temp0 = ct_ == 0.00
	temp4 = ! temp0
	if(temp4) goto Label1
	stemp1 = 0.00
	goto Label2
Label1:
	temp1 = ct_ * 9.00
	temp2 = temp1 / 5.00
	temp3 = temp2 + 32.00
	stemp1 = temp3
Label2:
	ft_ = stemp1
	stemp0 = ft_
	goto Label0
Label0:
	 return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
	write  "Input temperature in centigrade"
	read  in_
	temp0 = convert_centigrade_to_fahrenheit_(in_)
	out_ = temp0
	write  "Temperature in fahrenheit is "
	write  out_
**END: Three Address Code Statements
