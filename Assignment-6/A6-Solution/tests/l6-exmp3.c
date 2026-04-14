int main()
{
	string *ptr;
	int a, b;
	int * c;
	string x;
	c = &b;
	a = *c + 1;
	x="hello";
	ptr=&x;
	*ptr="g";
	return 0;
}
