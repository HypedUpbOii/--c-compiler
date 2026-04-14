int main()
{
	int a,b,c;

	a = 1;
	b = 10;
	c = 10;
	// int ******x, **y, **z, *p, *q, *r;
	// int d[4];
	// float arr[4][5];
	// string x[4][5];

	// x = &d;
	// x = & p;
	// p = &c;
	// y = ****x;

	for (a = 0; a < b; a = a + 1;) {
		c = c * 5;
		if (c > 25000) {
			break;
		}
		print c;
		print "\n";
	}
	print c;

	// **y = a;
	// y = &r;
	// y = z;
	// p = *x;

	// **y = *p + 5;

	// d[1] = 67;
	// arr[1][a + 2] = 3.0;

	// temp0 = 2 * 5
	// temp1 = temp0 + 3
	// temp2 = temp1 * 6
	// temp3 = temp2 + 4
	// temp4 = temp3 * 7
	// temp5 = temp4 + 1
	// temp6 = temp5 * 8
	// arr[temp6] = 6.7
	// temp7 = arr + temp6
	// *temp7 = 6.7
	// 2 * 5
	// (2 * 5) + 3
	// ((2 * 5) + 3) * 6
	// (((2 * 5) + 3) * 6) + 4
	// ((((2 * 5) + 3) * 6) + 4) * 7
	// (((((2 * 5) + 3) * 6) + 4) * 7) + 1
	return 0;
}