/*
struct test
{
	int a, b, c, d, e, f, g, h, i;
};

int global = 9;
int global2 = 10;

int MyTest( int a, unsigned char ll, unsigned char b, struct test t ) 
{
	int r;
	int k;
	a+=353*4;
	global +=5;
	ll = ~ll;
	r = a+ll+b;
	r = r - 5;
	r = 5 - r;
	k = t.g * ll;
	if( k == 36 )
	{
		int m = r * k;
		r += m;
	}
	else
	{
		r+= 9;
	}
	r = t.a;
	r += 9+a;
	r += global;

	return r * k;
}

int quickret( int a )
{
	return a+5;
}


int _start()
{
	global++;
	global2++;
}
*/

int unused_int_value;
int a;
int mid[10];
int my_test_value;
extern int my_test_extern;
int test();
const int const_int_test = 99;

int _start()
{
	a = 5;
	my_test_extern = 9;
	my_test_value = 6;
	my_test_value = const_int_test;
	return my_test_extern;
}

int bt()
{
	my_test_extern = 9;
}
