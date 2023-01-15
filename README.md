This library is thought  for heavy duty work with big numbers. The scope of the work is give you the basic operations for implement your algorithms, as fast as possible without any limits but your RAM.
To be fast the choose can’t be that C.  Of course, if you program in C++ like in C, there should be no difference, in other hand if you program in C like in C++, there should be also not big difference, but, if you program in C as you should program in C, well…
The C become famous because gave us the pointers, and we declined it when switch to C++. 
To be fast, I declined the std library, I declined the increments with auxiliary  variables, and use only pointers.
The result shocked me first. I expected an improvment of 1.3 or 1.4 times faster, the benchmark with another library https://www.geeksforgeeks.org/bigint-big-integers-in-c-with-example/, results 3 times faster with factorial of 100,000, 268 secs vs 710 secs, and 4 times faster with  pow, 980 secs vs 4010 secs. If you want try by yourself, change the function power from

if(Exponent[0] & 1)<br>
    a *= Base;<br>
to 

if(Exponent[0] & 1)<br>
{<br>
a *= Base;<br>
if(Exponent==1)<br>
	break;<br>
}<br>
<br>
Otherwise, is going to perform another very expensive multiplication, and the result would be still worse.<br>
 This is the result of using pointers and wise reallocation of the memory, by the way the grout of reallocation in this library is exponential, it means that double up at each reallocation, if you work around the limit of your memory, over 1G digits, may be you have to change it.
<br>
<h1>Usage</h1>
The first step to do is to typedef a 64 bit signed and unsigned types according your compiler. You find the typedef in the first rows of the header :<br>
<b>#if defined(_MSC_VER)<br>
#pragma warning ( disable : 4996)<br>
#define BNLONGLONGPATTERN "%lld"<br>
#define BNULONGLONGPATTERN "%llu"<br>

#else<br>
#define BNLONGLONGPATTERN "%I64d"<br>
#define BNULONGLONGPATTERN "%I64u"<br>
<br>
typedef long long BNi64;<br>
typedef unsigned long long BNu64;<br> 

#endif<br></b>
<br>
this is essential for the library.<br>
In order to use the main.c file, you have to change also :<br> 
BNLONGLONGPATTERN "%I64d"<br>
BNULONGLONGPATTERN "%I64u"<br>
to the pattern required by your compiler to printf() and scanf() 64 bit integer variables. Mostly “%llu” and “%lld”, but mingw uses  "%I64d" for example.
The main.c file shows you the use of each function and  in the header is explained each function and return value. It contains also several funny benchmarks.
Work with big numbers will give you  also the possibility to improve your skills and check various legends.<br>
++i is faster than i++? Difference not appreciable<br>
Is  !(a<b)  faster than (a>=b) ?Difference not appreciable<br>
Is a>b faster than a>=b? (of course you arrange “a” to a slot before), first time it gives me 7 secs of improvment, but in a second test, it was again the same time. Also difference not appreciable<br>
Is pointer arithmetic faster than array deference ? 4 times faster<br>


