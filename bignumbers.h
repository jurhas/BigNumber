#ifndef BIGNUMBERS_H_INCLUDED
#define BIGNUMBERS_H_INCLUDED
#define _CRT_SECURE_NO_WARNINGS
#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "stdio.h"
#include "time.h"

#define BN_IS_DEBUG 1

#if defined(_MSC_VER)
#pragma warning ( disable : 4996)
#define BNLONGLONGPATTERN "%lld"
#define BNULONGLONGPATTERN "%llu"

#else
#define BNLONGLONGPATTERN "%I64d"
#define BNULONGLONGPATTERN "%I64u"


#endif

typedef long long BNi64;
typedef unsigned long long BNu64;

#define BNSTARTBUF 20
#define BNRIGHTBUF 10
#define BNDEFAULTBUF -1

typedef enum
{
    BNFALSE,
    BNTRUE
} BNBOOL;



#define BNreset(a) ( ((a)->start=(a)->end),*(a)->start=0,(a)->start[1]='\0')
#define BNreset_lf(a) ( ((a)->end=(a)->start),*(a)->start=0,(a)->start[1]='\0')
#define BNreset_at(a,b) (((a)->start=(a)->end=(a)->buffer+(b)),*(a)->start=0,(a)->start[1]='\0')
#define BNis_zero(a) ( ((a)->start==(a)->end)&& *(a)->start==0)
#define BNis_one(a) ( ((a)->start==(a)->end)&& *(a)->start==1)
#define BNlen(a) ((a)->end-(a)->start +1)

#define BNis_even(a)( (*(a)->end &1)==0 ) //pari
#define BNis_odd(a)( (*(a)->end &1) ) //dispari

///the struct BigNumber has 4 fields:
///  buffer: is the address of the buffer, to pass for free()
/// start: is the address where starts the number(inclusive)
///end: is the address where the number ends(inclusive), the slot after end must be set to 0
///tail: is the end of the buffer and must be set to 0
/// buffer      start       end           tail
///     ********123456789102123'\0'********'\0'
typedef struct
{
    char * buffer;
    char *start;
    char *end;
    char * tail;
}BigNumber, *pBigNumber;

extern pBigNumber max_ull_size;

/// These are the return value of all the functions(except the BNnew_xxx() functions)
///As you can see, BN_OK is 0 and if error occurs is greater. So you can code if(BN_xxx()) {err_handler}
typedef enum
{
    BN_OK,
    BN_RUN_OUT_OF_MEMORY,
    BN_DIVISION_BY_ZERO,
    BN_NUMBER_NOT_VALID,
    BN_NEGATIVE_NUMBER,
    BN_CANNOT_FIT_IN_A_VARIABLE,
    BN_CANNOT_OPEN_FILE,
    BN_EMPTY_FILE,
    BN_OVERFLOW
}BNERR_NUMBER;

///These functions return a pointer to a valid pBigNumer struct
/// if they fail return a NULL pointer. This means that we run out of memory.
///except for BNnew_from_string() and BNnew_from_file()
///BNnew_from_string can return BN_NUMBER_NOT_VALID or BN_RUN_OUT_OF_MEMORY
///BNnew_from_file can return BN_NUMBER_NOT_VALID or BN_RUN_OUT_OF_MEMORY
///BN_CANNOT_OPEN_FILE,BN_EMPTY_FILE

///BNclone and BNnew_from_bn, do the same work, matter of taste use one or the other
pBigNumber BNnew_default();
pBigNumber BNnew_set(BNi64 buf_sz,BNi64 rigth_buf);
pBigNumber BNnew_from_string(char *s, BNERR_NUMBER*errn);
pBigNumber BNnew_from_int(BNu64 i);
pBigNumber BNnew_from_file(char* file_name, BNERR_NUMBER *errn);
pBigNumber BNnew_from_bn(pBigNumber a );
pBigNumber BNnew_rand(BNu64 len,BNBOOL to_seed);
pBigNumber BNclone(pBigNumber n);


///BNdestroy() free the mem
void BNdestroy(pBigNumber a);

///these functions check the buffer and if need realloc,if you don't add functions to
///this library you should not use these functions.
///They return BN_OK or BN_RUN_OUT_OF_MEMORY, in this case, the


BNERR_NUMBER BNcheck_buffer(pBigNumber n,BNu64 len);
BNERR_NUMBER BNcheck_rg_buffer(pBigNumber n,BNu64 len);
BNERR_NUMBER BNcheck_buffer_both_sides(pBigNumber n,BNu64 len,BNu64 rg_len);

///these functions realloc, you shouldn't use them also if you add functions to this library
/// use check_buffer family functions
BNERR_NUMBER BNrealloc(pBigNumber n,BNi64 buf_sz,BNi64 rigth_buf );
BNERR_NUMBER BNrealloc_def(pBigNumber n);

///these functions set the value of a existing pBigNumber object
///they return BN_OK,BN_RUN_OUT_OF_MEMORY and the function
///BNstr_ro_bn can return BN_NUMBER_NOT_VALID
BNERR_NUMBER BNstr_to_bn(pBigNumber n,char *s,BNu64 len);
BNERR_NUMBER BNint_to_bn(pBigNumber n,BNu64 in_i);
BNERR_NUMBER BNcopy(pBigNumber a,pBigNumber ans);

///these are IO functions
///printf_bn(), printf the number safe, it is just for debug purpose, therefore has no BN prefix
///ATTENTION BNto_string_in_place() set the number to a valid string character in place
///YOU MUST EVER CALL BNto_number_in_place() after you call BNto_string_in_place()
///BNto_string_dup() allocates a buffer and you have to call free() when done
///BNto_u() set the output parameter u to the current value, if the pBigNumber is greater as
/// the limit of the unsigned long long max value, it returns BN_CANNOT_FIT_IN_A_VARIABLE
///and sets the the value of u to 18446744073709551615
void printf_bn(pBigNumber a );
char* BNto_string_inplace(pBigNumber n);
void BNto_number_inplace(pBigNumber n);
char* BNto_string_dup(pBigNumber n);
BNERR_NUMBER BNto_u(pBigNumber n, BNu64 *u );


///this function does the comparison between two BigNumbers. If they differ in length is fast
///but if they have the same length and the first n character are equal it becomes O(n^2) if
///you use in a while() or for() (if you use in a single operation there is no problem)
///If it is possible, use reverse decrement, until BNis_one() or BNis_zero() otherwise, move
///to a native type such unsigned long long and use this.
///return value is the same as strcmp()
int BNcmp(pBigNumber a,pBigNumber b);
#define BNgt(a,b) (BNcmp(a,b)>0)
#define BNgt_eq(a,b) (BNcmp(a,b)>=0)
#define BNeq(a,b) (BNcmp(a,b)==0)
#define BNlt(a,b) (BNcmp(a,b)<0)
#define BNlt_eq(a,b) (BNcmp(a,b)<=0)



///these function are the sum family, they return BN_OK or BN_RUN_OUT_OF_MEMORY
///BNplus_plus() add one to the argument
///BNplus_eq() is equivalent to n+=other
///BNplus() is equivalent to ans=a+b
///BNplus_u() b must be >=0 and <=9 and is equivalent to ans=a+b
///BNplus_u_eq() b must be >=0 and <=9 and is equivalent to a+=b
BNERR_NUMBER BNplus_plus(pBigNumber n);
BNERR_NUMBER BNplus_eq(pBigNumber n,pBigNumber other);
BNERR_NUMBER BNplus(pBigNumber a,pBigNumber b,pBigNumber ans);
BNERR_NUMBER BNplus_u(pBigNumber a,unsigned int b,pBigNumber ans);
BNERR_NUMBER BNplus_u_eq(pBigNumber a,unsigned int b);


///these function are the subtraction family, they return BN_OK, BN_RUN_OUT_OF_MEMORY or BN_NEGATIVE_NUMBER
///BNminus_minus() subtract one to the argument
///BNminus_eq() is equivalent to n-=other
///BNminus() is equivalent to ans=a-b
///BNminus_u() b must be >=0 and <=9 and is equivalent to ans=a-b
///BNminus_u_eq() b must be >=0 and <=9 and is equivalent to a-=b
BNERR_NUMBER BNminus_minus(pBigNumber n);
BNERR_NUMBER BNminus_eq(pBigNumber n,pBigNumber other);
BNERR_NUMBER BNminus(pBigNumber a,pBigNumber b,pBigNumber ans);
BNERR_NUMBER BNminus_u(pBigNumber a,unsigned int b,pBigNumber ans);
BNERR_NUMBER BNminus_u_eq(pBigNumber a,unsigned int b);


///these function are the multiplication family, they return BN_OK or BN_RUN_OUT_OF_MEMORY
///and the function BNtimes() can return BN_OVERFLOW
///BNtimes_u() b must be >=0 and <=9 and is equivalent to ans=a*b
/// BNtimes_u_eq() b must be >=0 and <=9 and is equivalent to a*=b
///BNtimes_10x_eq() is equivalent to a*=10^x . If you use this function repeatedly, use BNreset_lf() to reset the big number
///BNtimes_10x() is equivalent to ans=a*10^x . If you use this function repeatedly, use BNreset_lf() to reset the big number

BNERR_NUMBER BNtimes_u(pBigNumber a,unsigned int b, pBigNumber ans);
BNERR_NUMBER BNtimes_u_eq(pBigNumber a,unsigned int b);
BNERR_NUMBER BNtimes_10x_eq(pBigNumber a,BNu64 x);
BNERR_NUMBER BNtimes_10x(pBigNumber a,BNu64 x, pBigNumber ans);

///These function are the multiplication, there are two family:normal and safe:
/// don't be scared of the word "safe", The problem comes when you multiply two numbers with both more
///than 100M digits . The safe functions are safer in two ways, require less memory and they not overflow
///but are 3 or 4 times slower. If the memory is not a problem, you can change the typedef bn_times_buffert_t
///to long long (signed type or it will no detect the overflow),and practically you are safe.
///the "not safe" functions   return therefore BN_OK,  BN_RUN_OUT_OF_MEMORY or BN_OVERFLOW
///the "safe" functions   return BN_OK,  BN_RUN_OUT_OF_MEMORY
///BN_times() is equivalent to ans=a*b
///BN_times_eq() is equivalent to a*=b
typedef int bn_times_buffert_t;
BNERR_NUMBER BNtimes(pBigNumber a,pBigNumber b,pBigNumber ans );
BNERR_NUMBER BNtimes_eq(pBigNumber a,pBigNumber b);

BNERR_NUMBER BNtimes_safe(pBigNumber a,pBigNumber b,pBigNumber ans );
BNERR_NUMBER BNtimes_eq_safe(pBigNumber a,pBigNumber b);

///BNdivide()  divides a/b so are equivalent to ans=a/b and remainder=a%b
///it can return BN_OK, BN_RUN_OUT_OF_MEMORY, BN_DIVISION_BY_ZERO and theoretically BN_NEGATIVE_VALUE
///but if this happen, please alert me, because it would mean there is a bug
BNERR_NUMBER BNdivide(pBigNumber a,pBigNumber b,pBigNumber ans,pBigNumber remainder );
///BNdivide_by_2() equivalent to ans=a/2 returns BN_OK or BN_RUN_OUT_OF_MEMORY
///BNdivide_by_2_eq() equivalent to a/=2 returns BN_OK or  BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNdivide_by_2(pBigNumber a,pBigNumber ans);
BNERR_NUMBER BNdivide_by_2_eq(pBigNumber a);

///BNmod_u () is equivalent to *ans=n%u, it require that the divisor is a native type, when possible
///use it because is 100 times faster than BNdivide() it can return BN_OK or BN_DIVISION_BY_ZERO
BNERR_NUMBER BNmod_u(pBigNumber n, BNu64 u, BNu64 *ans );

///BNsqrt() computes the square root of the number and store it to ans,
/// the algorithm used is the bisection, I improved
///it choosing a better start point. It returns BN_OK or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNsqrt(pBigNumber a,pBigNumber ans);
///BNsqrt_remainder() computes the square root of the number and store it to ans,
/// the remainder is equivalent to remainder=n-(ans*ans)
///It returns BN_OK or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNsqrt_remainder(pBigNumber a,pBigNumber ans,pBigNumber remainder);
///BNrough_sqrt(), If the number contains more than 2000 digits, (you can benchmark it),
///the BNsqrt() requires several seconds, so if you want have a number to work
///BNrough_sqrt, gives you an approximation , bigger than the BNsqrt()
///it return BN_OK or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNrough_sqrt(pBigNumber a,pBigNumber ans);

///BNpow(), is equivalent to ans=a^pow. It returns BN_OK or BN_RUN_OUT_OF_MEMORY
///the argument pow is a unsigned long long type
BNERR_NUMBER BNpow(pBigNumber a, BNu64 pow, pBigNumber ans);

///BNmodular_pow(), is equivalent to ans=(a^pow)%mod. If mod can fit in a unsigned long long value
///it's pretty fast, otherwise, computes the power and than the division
///It returns BN_OK,BN_DIVISION_BY_ZERO or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans);

///BNfactorial(), is equivalent to ans=a!.
///It returns BN_OK,BN_DIVISION_BY_ZERO or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNfactorial(pBigNumber a,pBigNumber ans);
///BNgcd(), computes the Greatest Common divisor
///It returns BN_OK or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNgcd(pBigNumber a,pBigNumber b, pBigNumber ans);

///BNlcm(), computes the least common multiple
///It returns BN_OK or BN_RUN_OUT_OF_MEMORY or BN_DIVISION_BY_ZERO
BNERR_NUMBER BNlcm(pBigNumber a,pBigNumber b, pBigNumber ans);


///BNfactorize_rho() implements the Rho Pollard algorithm
///It returns BN_OK or BN_RUN_OUT_OF_MEMORY

BNERR_NUMBER BNfactorize_rho(pBigNumber a, pBigNumber ans);

///BNreverse() reverse the number, for example 123 reversed is 321
///It returns BN_OK or BN_RUN_OUT_OF_MEMORY
BNERR_NUMBER BNreverse(pBigNumber a,pBigNumber b);

///BNis_palindrome() checks if a number is palindrome, return 1 if true, 0 if false

int BNis_palindrome(pBigNumber a);

typedef struct
{
    pBigNumber bn;
    int cnt;

} Factorize,*pFactorize;
typedef struct
{
    pFactorize p;
    size_t sz;
    size_t pos;
} VectFact,*pVectFact;
#define BNSTARTVECTFACTLEN 100
///These struct are required from the function BNfactorize, where to store the factors
///are BNnew_vect_fact() for a new vector,returns NULL if there is not enough memory,
/// BNdestroy_vect_fact() frees the mem
///BNpush_n() add a new factor, returns BN_OK or BN_RUN_OUT_OF_MEMORY
///The macro BNincr_counter(a), increments by one the last added BigNumber
pVectFact BNnew_vect_fact();
void BNdestroy_vect_fact(pVectFact vf );

BNERR_NUMBER BNpush_n(pVectFact vf,pBigNumber n );
#define BNincr_counter(a)  ( (a)->p[(a)->pos-1].cnt++)

///BNfactorize() factorizes the given number and store it in the vector ans
BNERR_NUMBER BNfactorize(pBigNumber n, pVectFact ans);

#endif // BIGNUMBERS_H_INCLUDED
