#include "bignumbers.h"

static  char _a[]= {1,8,4,4,6,7,4,4,0,7,3,7,0,9,5,5,1,6,1,5,0};
BigNumber _max_ull_size = {_a,_a,_a+19,_a+20};
pBigNumber max_ull_size=&_max_ull_size;

void printf_bn(pBigNumber a )
{
    char tmp;
    tmp=a->end[1];
    a->end[1]='\0';
    printf("%s\n",BNto_string_inplace(a));
    BNto_number_inplace(a);
    a->end[1]=tmp;
}

pBigNumber BNnew_set(BNi64 buf_sz,BNi64 rigth_buf)
{
    pBigNumber res=malloc(sizeof(BigNumber));
    BNu64 rg,cntr,tot_buf;
    if(!res) return NULL;
    if(buf_sz==BNDEFAULTBUF)
        cntr=BNSTARTBUF;
    else
        cntr=BNSTARTBUF+buf_sz;
    if(rigth_buf==BNDEFAULTBUF)
        rg=BNRIGHTBUF;
    else
        rg=rigth_buf+BNRIGHTBUF;
    tot_buf=rg+cntr;
    res->buffer=malloc(tot_buf+1);
    if(!res->buffer)
    {
        free(res);
        return NULL;
    }
    res->tail=&res->buffer[tot_buf];
    *res->tail='\0';
    res->start=res->end=&res->buffer[tot_buf-rg-1];
    *res->start=0;
    *(res->start+1)='\0';
    return res;
}
pBigNumber BNnew_default()
{
    return BNnew_set(BNDEFAULTBUF,BNDEFAULTBUF);
}

pBigNumber BNnew_from_string(char *s, BNERR_NUMBER *errn)
{

    BNi64 len=strlen(s);
    pBigNumber res=BNnew_set(len,BNDEFAULTBUF);
    if(!res)
    {
        *errn=BN_RUN_OUT_OF_MEMORY;
        return NULL;
    }
    *errn=BNstr_to_bn(res,s,len);
    if(*errn!=BN_OK)
    {
        BNdestroy(res);
        return NULL;
    }

    return res;

}

pBigNumber BNnew_from_int(BNu64 i)
{
    pBigNumber res;
    BNERR_NUMBER c;
    res=BNnew_default();
    if(!res)
        return NULL;
    c=BNint_to_bn(res,i);
    if(c!=BN_OK)
    {
        BNdestroy(res);
        return NULL;
    }
    return res;
}
pBigNumber BNnew_from_file(char* file_name, BNERR_NUMBER *errn)
{
    pBigNumber res;
    FILE *f;
    BNu64 f_len;
    char *run,c;
    *errn=BN_OK;
    f=fopen(file_name,"rb");
    if(!f)
    {
        *errn=BN_CANNOT_OPEN_FILE;
        return NULL;
    }

    if(fseek(f,0,SEEK_END))
    {
        fclose(f);
        *errn=BN_CANNOT_OPEN_FILE;
        return NULL;
    };

    f_len=ftell(f);
    if(f_len==0)
    {
        fclose(f);
        *errn=BN_EMPTY_FILE;
        return NULL;
    }
    res=BNnew_set(f_len+1,BNDEFAULTBUF);
    if(!res)
    {
        fclose(f);
        *errn=BN_RUN_OUT_OF_MEMORY;
        return NULL;
    }
    if(fseek(f,0,SEEK_SET))
    {
        fclose(f);
        BNdestroy(res);
        *errn=BN_CANNOT_OPEN_FILE;
        return NULL;
    };
    run=res->start=res->end-f_len+1;
    while((c=fgetc(f))!=EOF )
    {
        if(c<'0'|| c>'9' )
        {
            fclose(f);
            BNdestroy(res);
            *errn=BN_NUMBER_NOT_VALID;
            return NULL;
        }
        *run=c-'0';
        ++run;
    }
    fclose(f);
    return res;
}

pBigNumber BNnew_from_bn(pBigNumber a)
{
    pBigNumber res;
    res=BNnew_set(BNlen(a)+1,BNDEFAULTBUF);
    if(!res) return NULL;
    if( (BNcopy(a,res)!=BN_OK ))
    {
        BNdestroy(res);
        //assert(("Copy gone wrong",0));
        return NULL;
    }
    return res;
}
pBigNumber BNnew_rand(BNu64 len,BNBOOL to_seed)
{
    pBigNumber res;
    int first=1;
    assert( len>0  );
    res=BNnew_set(len+1,BNDEFAULTBUF);
    if(!res) return NULL;
    if(to_seed)
        srand((unsigned int)time(NULL));
    while(len>0)
    {
        --len;
        if(first)
        {
            first=0;
            *res->start=rand()%10;
        }
        else
        {
            *--res->start=rand()%10;
        }

    }
    if(*res->start==0)
        *res->start=rand()%9+1;
    return res;
}
BNERR_NUMBER BNcheck_buffer(pBigNumber n,BNu64 len)
{
    BNu64 cur_size=n->end- n->buffer;
    BNERR_NUMBER res=BN_OK;
    if(cur_size<=len)
        res=BNrealloc(n,len+BNSTARTBUF,BNDEFAULTBUF);

    return res;
}
BNERR_NUMBER BNcheck_rg_buffer(pBigNumber n,BNu64 len)
{
    BNERR_NUMBER res=BN_OK;


    if((BNu64)(n->tail-n->end+1)<len)
        res=BNrealloc(n,BNDEFAULTBUF,len);
    return res;
}
BNERR_NUMBER BNcheck_buffer_both_sides(pBigNumber n,BNu64 len,BNu64 rg_len)
{
    BNERR_NUMBER res=BN_OK;
    if((BNu64)(n->end-n->start)<len || (BNu64)(n->tail-n->end)+1<rg_len)
        res=BNrealloc(n,len,rg_len);
    return res;
}
BNERR_NUMBER BNrealloc(pBigNumber n,BNi64 buf_sz,BNi64 rigth_buf )
{
    BNu64 new_len,rg_buf_len;
    char *tmp, *new_i,*old_i;
    BNERR_NUMBER res=BN_OK;
    rg_buf_len=rigth_buf>0?rigth_buf+BNRIGHTBUF:BNRIGHTBUF;

    buf_sz=buf_sz>0?(n->tail-n->buffer)*2+buf_sz:BNSTARTBUF+(n->tail-n->buffer);

    new_len=buf_sz+rg_buf_len+BNSTARTBUF;

    tmp=malloc(new_len+1);
    if(!tmp) return BN_RUN_OUT_OF_MEMORY;

    tmp[new_len]=0;

    old_i=n->end;

/// the char after n->end must be null for when call BNto_string_inplace()
    new_i=&tmp[new_len-rg_buf_len+1];
    ///set the place to null
    *new_i='\0';

    n->end=--new_i;

    while(old_i>=n->start)
    {
        *new_i=*old_i;
        --new_i;
        --old_i;
    }
    ++new_i;
    n->start=new_i;
    free(n->buffer);
    n->buffer=tmp;
    n->tail=n->buffer+new_len;
    return res;
}
BNERR_NUMBER BNrealloc_def(pBigNumber n)
{
    return BNrealloc(n,BNDEFAULTBUF,BNDEFAULTBUF);
}
void BNdestroy(pBigNumber a)
{
    free(a->buffer);
    free(a);
}
BNERR_NUMBER BNcopy(pBigNumber a,pBigNumber ans)
{
    BNERR_NUMBER c,res=BN_OK;
    register char *run_a,*run_ans;

    BNreset(ans);
    if( (c=(BNcheck_buffer(ans,BNlen(a)+1))   )!=BN_OK)
        return c;
    run_a=a->end;
    run_ans=ans->end;
    while(run_a>=a->start)
    {
        *run_ans=*run_a;
        --run_a;
        --run_ans;
    }
    ans->start=++run_ans;
    ans->end[1]='\0';


    return res;
}
pBigNumber BNclone(pBigNumber n)
{
    BNERR_NUMBER c;
    pBigNumber res=BNnew_set(BNlen(n)+1,BNDEFAULTBUF);
    if (!res) return NULL;
    if((c=BNcopy(n,res))!=BN_OK  )
    {
        BNdestroy(res);
        return NULL;
    }
    return res;
}


BNERR_NUMBER BNint_to_bn(pBigNumber n,BNu64 in_i)
{
    size_t len;
    BNERR_NUMBER c=BN_OK;

    BNreset(n);
    if(in_i)
        len=(size_t)log10((double) in_i)+2;
    else
        len=2;
    c=BNcheck_buffer(n,len);
    if(c==BN_RUN_OUT_OF_MEMORY)
        return c;
    do
    {
        *n->start=in_i%10;
        in_i/=10;
        --n->start;
    }
    while(in_i);
    ++n->start;
    return BN_OK;
}

BNERR_NUMBER BNstr_to_bn(pBigNumber n,char *s,BNu64 len)
{
    BNERR_NUMBER errn=BN_OK;
    char *runner;
#if BN_IS_DEBUG
    assert(s);
    assert(len>0);
#endif // BN_IS_DEBUG
    runner=s+len-1;
    errn=BNcheck_buffer(n,len);

    if(errn!=BN_OK)
        return errn;
    BNreset(n);

    while(runner>=s)
    {
        if(*runner<'0' || *runner>'9')
            return BN_NUMBER_NOT_VALID;
        *n->start=*runner -'0';
        --runner;
        --n->start;
    }
    ++n->start;
    while(*n->start==0 && n->start<n->end)
        ++n->start;
    return errn;
}
char* BNto_string_inplace(pBigNumber n)
{
    char * runner=n->start;
#if BN_IS_DEBUG
    assert(*runner>=0 && *runner <=9);
#endif // BN_IS_DEBUG
    while(runner<=n->end)
    {
        *runner+='0';
        ++runner;
    }
    return n->start;
}
void BNto_number_inplace(pBigNumber n)
{
    char * runner=n->start;
    assert(*runner>='0' && *runner <='9');
    while(runner<=n->end)
    {
        *runner-='0';
        runner++;
    }
}
char* BNto_string_dup(pBigNumber n)
{
    size_t len=BNlen(n);
    char *res,*run_dest,*run_src;
    assert(*n->start>=0 && *n->start<=9);
    res=malloc(len+1);
    if(!res)
        return NULL;
    res[len]='\0';
    run_dest=res;
    run_src=n->start;
    while(run_src<=n->end)
    {
        *run_dest=*run_src+'0';
        run_dest++;
        run_src++;
    }
    return res;
}
BNERR_NUMBER BNto_u(pBigNumber n, BNu64 *u )
{
    BNERR_NUMBER res=BN_OK;
    char *run_n;
    if(BNgt(n,max_ull_size))
    {
        res=BN_CANNOT_FIT_IN_A_VARIABLE;
        *u=0xFFFFFFFFFFFFFFFF;
    }
    else
    {
        *u=0;
        run_n=n->start;
        while(run_n<=n->end)
        {
            *u=(*u<<3)+(*u<<1)+*run_n;
            run_n++;
        }
    }
    return res;

}
int BNcmp(pBigNumber a,pBigNumber b)
{
    register char *run_a,*run_b;
    char *run_stop;
    //assert(*a->start && *b->start);
    BNi64 c=(BNi64)(a->end-a->start)-(BNi64)(b->end-b->start);
    if(c>0)
        return 1;
    else if(c<0)
        return -1;

    run_a=a->start;
    run_b=b->start;
    run_stop=a->end+1;
    while(*run_a==*run_b && run_a<run_stop)
    {
        run_a++;
        run_b++;
    }
    return *run_a-*run_b;
}

BNERR_NUMBER BNplus_plus(pBigNumber n)
{
    register char * runnner;
    register char carryover=0;
    BNERR_NUMBER c;
    runnner=n->end;

    do
    {
        if((carryover=(*runnner==9 ))  )

            *runnner=0;
        else
            ++(*runnner);
        --runnner;
    }
    while(carryover && (runnner>=n->start));

    if(carryover )
    {
        if(n->start<=n->buffer)
        {
            if( (c=BNrealloc_def(n))!=BN_OK  )
                return c;
        }


        *--n->start=1;
    }

    return BN_OK;

}

BNERR_NUMBER BNplus_eq(pBigNumber n,pBigNumber other)
{
    BNERR_NUMBER res=BN_OK;
    register char *run_a;
    register char *run_b;
    register char carryover;
    char * n_stop,*other_stop;
    BNu64  n_buf_len,nlen=BNlen(n), other_len=BNlen(other) ;
    nlen=BNlen(n);
    other_len=BNlen(other);
    n_buf_len=nlen>other_len?nlen+1:other_len+1;
    if( (res=BNcheck_buffer(n,n_buf_len))!=BN_OK)
        return res;
    run_a=n->end;
    run_b=other->end;
    carryover=0;
    if(nlen>=other_len)
    {
        other_stop=other->start-1;
        while( run_b>other_stop)
        {

            *run_a+=*run_b+carryover;
            if((carryover=*run_a>9))
                *run_a-=10;
            --run_a;
            --run_b;
        }
        n_stop=n->start-1;
        while(carryover && run_a>n_stop)
        {
            if( (carryover= (*run_a==9)))
            {
                *run_a=0;
                run_a--;
            }
            else
            {
                ++(*run_a);
                break;
            }

        }
        if(carryover)
            *--n->start=1;
    }
    else
    {
        n_stop=n->start-1;
        while( run_a>n_stop)
        {

            *run_a+=*run_b+carryover;
            if((carryover=*run_a>9))
                *run_a-=10;
            --run_a;
            --run_b;
        }
        other_stop=other->start-1;
        while(run_b>other_stop)
        {
            if(carryover && *run_b==9)
            {
                *--n->start=0;
            }
            else if(carryover)
            {
                *--n->start=*run_b+1;
                carryover=0;
            }
            else
            {
                *--n->start=*run_b;
                carryover=0;
            }
            --run_b;
        }
        if(carryover)
            *--n->start=1;
    }

    return res;
}

BNERR_NUMBER BNplus(pBigNumber a,pBigNumber b,pBigNumber ans)
{
    pBigNumber _a,_b;
    BNERR_NUMBER c,res=BN_OK;
    register char *run_a,*run_b,*run_ans,carryover;
    if(BNlen(a)<BNlen(b))
    {
        _a=b;
        _b=a;
    }
    else
    {
        _a=a;
        _b=b;
    }
    BNreset(ans);
    c=BNcheck_buffer(ans,BNlen(_a)+1);
    if(c!=BN_OK)
    {
        BNreset(ans);
        return c;
    }
    carryover=0;
    run_a=_a->end;
    run_b=_b->end;
    run_ans=ans->end;
    while(run_b>=_b->start)
    {
        *run_ans=*run_a+*run_b+carryover;
        if((carryover=(*run_ans>9)   ))
            *run_ans-=10;
        --run_ans;
        --run_a;
        --run_b;
    }
    while(run_a>=_a->start)
    {
        *run_ans=*run_a+carryover;
        if((carryover=(*run_ans>9)   ))
            *run_ans-=10;
        --run_ans;
        --run_a;
    }
    ans->start=++run_ans;
    if(carryover)
        *--ans->start=1;

    return res;

}

BNERR_NUMBER BNplus_u(pBigNumber a,unsigned int b,pBigNumber ans)
{
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG
    register char *run_a,*run_ans,carryover;
    BNERR_NUMBER c,res=BN_OK;
    BNreset(ans);
    c=BNcheck_buffer(ans,BNlen(a)+1);
    if(c!=BN_OK)return c;
    run_a=a->end;
    run_ans=ans->end;
    *run_ans=*run_a+b;
    if((carryover=(*run_ans>9)))
        *run_ans-=10;
    --run_ans;
    --run_a;
    while(run_a>=a->start)
    {
        *run_ans=*run_a+carryover;
        if((carryover=(*run_ans>9)))
            *run_ans-=10;
        --run_ans;
        --run_a;
    }
    ans->start=++run_ans;
    if(carryover)
        *--ans->start=1;
    return res;

}

BNERR_NUMBER BNplus_u_eq(pBigNumber a,unsigned int b)
{
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG
    register char *run_a,carryover;
    BNERR_NUMBER c,res=BN_OK;

    c=BNcheck_buffer(a,BNlen(a)+1);
    if(c!=BN_OK)return c;
    run_a=a->end;

    *run_a+=b;
    if((carryover=(*run_a>9)))
        *run_a-=10;

    --run_a;
    while(carryover && run_a>=a->start)
    {
        *run_a+=carryover;
        if((carryover=(*run_a>9)))
            *run_a-=10;

        --run_a;
    }
    if(carryover)
        *--a->start=1;
    return res;

}

BNERR_NUMBER BNminus_minus(pBigNumber n)
{
    BNERR_NUMBER res=BN_OK;
    register char *runner, carryover=0;
    runner=n->end;

    do
    {
        if( (carryover=(*runner==0)))
            *runner=9;
        else
            --(*runner);
        --runner;
    }
    while(carryover && runner>=n->start);
    if(carryover)
    {
        BNreset(n);
        return BN_NEGATIVE_NUMBER;
    }
    while(*n->start==0 && n->start<n->end)
        ++n->start;
    return res;
}
BNERR_NUMBER BNminus_eq(pBigNumber n,pBigNumber other)
{
    BNERR_NUMBER res=BN_OK;
    register char *run_a,*run_b,carryover=0;
    run_a=n->end;
    run_b=other->end;
    if(BNlen(n)<BNlen(other))
    {
        BNreset(n);
        return BN_NEGATIVE_NUMBER;
    }
    while(run_b>=other->start)
    {
        *run_a-=*run_b+carryover;
        if( (carryover=(*run_a<0)))
            *run_a+=10;
        --run_a;
        --run_b;
    }
    while(run_a>=n->start && carryover)
    {
        if( (carryover=(*run_a==0))  )
            *run_a=9;
        else
            --(*run_a);
        --run_a;
    }
    if(carryover)
    {
        BNreset(n);
        return BN_NEGATIVE_NUMBER;
    }

    while(*n->start==0 &&n->start<n->end  )
        ++n->start;

    return res;
}
BNERR_NUMBER BNminus(pBigNumber a,pBigNumber b,pBigNumber ans)
{
    BNERR_NUMBER c,res=BN_OK;
    register char *run_a,*run_b,*run_ans,carryover=0;

    BNreset(ans);
    if(BNlen(a)<BNlen(b))
    {

        return BN_NEGATIVE_NUMBER;
    }
    if((c=(BNcheck_buffer(ans,BNlen(a))))!=BN_OK)
        return c;
    run_a=a->end;
    run_b=b->end;
    run_ans=ans->end;
    while(run_b>=b->start)
    {
        *run_ans=*run_a-*run_b-carryover;
        if( (carryover=(*run_ans<0)))
            *run_ans+=10;
        --run_a;
        --run_b;
        --run_ans;
    }
    while(run_a>=a->start)
    {
        if(carryover)
        {
            *run_ans=*run_a-carryover;
            if( (carryover=(*run_ans<0)  ))
                *run_ans+=10;
        }
        else
            *run_ans=*run_a;
        --run_a;
        --run_ans;
    }
    if(carryover)
    {
        BNreset(ans);
        return BN_NEGATIVE_NUMBER;
    }
    ++run_ans;
    ans->start=run_ans;
    while(*ans->start==0 && ans->start<ans->end  )
        ++ans->start;

    return res;

}
BNERR_NUMBER BNminus_u(pBigNumber a,unsigned int b,pBigNumber ans)
{
    BNERR_NUMBER c,res=BN_OK;
    register char *run_a,*run_ans,carryover=0;
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG
    BNreset(ans);
    if((c=BNcheck_buffer(ans,BNlen(a)+1))!=BN_OK )
        return c;
    run_a=a->end;
    run_ans=ans->end;
    *run_ans=*run_a-b;
    if((carryover=(*run_ans<0) ))
        *run_ans+=10;
    while(run_a>=a->start)
    {
        --run_ans;
        --run_a;
        *run_ans=*run_a-carryover;
        if( (carryover=(*run_ans<0)))
            *run_ans+=10;
    }
    ++run_ans;
    if(carryover)
    {
        BNreset(ans);
        return BN_NEGATIVE_NUMBER;
    }
    ans->start=run_ans;
    while(*ans->start==0 && ans->start<ans->end  )
        ++ans->start;

    return res;


}
BNERR_NUMBER BNminus_u_eq(pBigNumber a,unsigned int b)
{
    BNERR_NUMBER res=BN_OK;
    register char *run_a,carryover=0;
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG

    run_a=a->end;
    *run_a-=b;
    if((carryover=(*run_a<0) ))
        *run_a+=10;
    while(carryover && run_a>=a->start)
    {
        --run_a;
        if ( (carryover=(*run_a==0) ))
            *run_a=9;
        else
            --(*run_a);
    }
    if(carryover)
    {
        BNreset(a);
        return BN_NEGATIVE_NUMBER;
    }

    while(*a->start==0 && a->start<a->end  )
        ++a->start;

    return res;
}
BNERR_NUMBER BNtimes_u(pBigNumber a,unsigned int b, pBigNumber ans)
{
    BNERR_NUMBER c, res=BN_OK;
    register char *run_a,*run_ans,carryover=0,acc;
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG
    BNreset(ans);
    if(b==0) return res;
    if((c=BNcheck_buffer(ans,BNlen(a)+1 ))!=BN_OK )
        return c;
    run_a=a->end;
    run_ans=ans->end;
    while(run_a>=a->start)
    {
        acc=*run_a*b+carryover;
        *run_ans=acc%10;
        carryover=acc/10;
        --run_a;
        --run_ans;
    }

    ans->start=++run_ans;
    if(carryover)
        *--ans->start=carryover;
    return res;
}
BNERR_NUMBER BNtimes_u_imp(pBigNumber a,unsigned int b, pBigNumber ans)
{
    BNERR_NUMBER c, res=BN_OK;
    register char *run_a,*run_ans,carryover=0,acc;
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG
    BNreset(ans);
    if(BNis_zero(a)) return res;

    if((c=BNcheck_buffer(ans,BNlen(a)+1 ))!=BN_OK )
        return c;
    run_a=a->end;
    run_ans=ans->end;
    switch(b)
    {
        case 0:
            BNreset(ans);
            break;
        case 1:
            while(run_a>=a->start)
            {
                acc=*run_a+carryover;
                *run_ans=acc%10;
                carryover=acc/10;
                --run_a;
                --run_ans;
            }
            break;
            case 2:
                while(run_a>=a->start)
            {
                acc=(*run_a<<1)+carryover;
                *run_ans=acc%10;
                carryover=acc/10;
                --run_a;
                --run_ans;
            }
            break;
            case 3:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<1)+*run_a+carryover;
                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 4:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<2)+carryover;
                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 5:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<2)+*run_a+carryover;
                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 6:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<2)+(*run_a<<1)+carryover;
                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 7:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<2)+(*run_a<<1)+*run_a+carryover;

                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 8:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<3)+carryover;

                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            case 9:
                while(run_a>=a->start)
                {
                    acc=(*run_a<<3)+*run_a+carryover;

                    *run_ans=acc%10;
                    carryover=acc/10;
                    --run_a;
                    --run_ans;
                }
                break;
            default:
            assert(0);

    }
    ans->start=++run_ans;
    if(carryover)
        *--ans->start=carryover;
    return res;
}
BNERR_NUMBER BNtimes_u_eq(pBigNumber a,unsigned int b)
{
    BNERR_NUMBER c, res=BN_OK;
    register char *run_a,carryover=0,acc;
#if BN_IS_DEBUG
    assert(b<10);
#endif // BN_IS_DEBUG

    if(b==0)
    {
        BNreset(a);
        return res;
    }
    if((c=BNcheck_buffer(a,BNlen(a)+1 ))!=BN_OK )
        return c;
    run_a=a->end;
    while(run_a>=a->start)
    {
        acc=*run_a*b+carryover;
        *run_a=acc%10;
        carryover=acc/10;
        --run_a;
    }
    if(carryover)
        *--a->start=carryover;
    return res;
}
BNERR_NUMBER BNtimes(pBigNumber a,pBigNumber b,pBigNumber ans )
{
    BNERR_NUMBER res=BN_OK;

    BNu64 n,m;
    int s,t;
    register char *run_a,*run_b;
    register int *loc_run;
    register char *run_a_stop,*run_b_stop;
    register char *run_ans ;
    int *loc_buf,*loc_buf_end;
    BNreset(ans);
    n=BNlen(a);
    m=BNlen(b);

    if(BNis_zero(a)||BNis_zero(b) )
        return res;
    if( (res= BNcheck_buffer(ans,n+m+1))!=BN_OK)
        return res;

    loc_buf=calloc(n+m,sizeof(int));
    if(!loc_buf)
        return BN_RUN_OUT_OF_MEMORY;

    loc_buf_end=&loc_buf[n+m-1];
    run_a=a->end;
    run_a_stop=a->start-1;
    run_b_stop=b->start-1;
    while(run_a>run_a_stop)
    {
        loc_run=loc_buf_end;
        run_b=b->end;
        while(run_b>run_b_stop)
        {
            *loc_run+=*run_a**run_b;
            --loc_run;
            --run_b;
        }
        --loc_buf_end;
        --run_a;
    }
    loc_run=&loc_buf[n+m-1];
    run_ans=ans->end;
    t=0;
    while(loc_run>=loc_buf)
    {
        s = t + *loc_run;
        if(s<0)
        {
            free(loc_buf);
            BNreset(ans);
            return BN_OVERFLOW;
        }
        *run_ans= s % 10;
        t = s / 10;
        --run_ans;
        --loc_run;
    }
    ans->start=++run_ans;
    while(*ans->start==0 && ans->start<ans->end)
        ++ans->start;
    free(loc_buf);
    return res;

}
//BNERR_NUMBER BNtimes(pBigNumber a,pBigNumber b,pBigNumber ans )
//{
//    BNERR_NUMBER res=BN_OK;
//
//    uBNi64 n,m;
//    int s,t;
//    register char *run_a,*run_b;
//    register int *loc_run;
//    register char *run_a_stop,*run_b_stop;
//    register char *run_ans ;
//    bn_times_buffert_t *loc_buf,*loc_buf_end;
//    BNreset(ans);
//    n=BNlen(a);
//    m=BNlen(b);
//    if(BNis_zero(a)||BNis_zero(b) )
//        return res;
//    if( (res= BNcheck_buffer(ans,n+m+1))!=BN_OK)
//        return res;
//
//    loc_buf=calloc(n+m,sizeof(bn_times_buffert_t));
//    if(!loc_buf)
//        return BN_RUN_OUT_OF_MEMORY;
//
//    loc_buf_end=&loc_buf[n+m-1];
//    run_a=a->end;
//    run_a_stop=a->start-1;
//    run_b_stop=b->start-1;
//
//
//
//
//    while(run_a>run_a_stop)
//    {
//        loc_run=loc_buf_end;
//        run_b=b->end;
//        switch(*run_a)
//        {
//        case 0:
//            break;
//        case 1:
//            while(run_b>run_b_stop)
//            {
//                *loc_run+= *run_b;
//                --loc_run;
//                --run_b;
//            }
//            break;
//        case 2:
//             while(run_b>run_b_stop)
//            {
//                *loc_run+= *run_b<<1;
//                --loc_run;
//                --run_b;
//            }
//            break;
//        case 3:
//            while(run_b>run_b_stop)
//            {
//                *loc_run+= (*run_b<<1)+*run_b;
//                --loc_run;
//                --run_b;
//            }
//            break;
//        case 4:
//            while(run_b>run_b_stop)
//                {
//                    *loc_run+= *run_b<<2;
//                    --loc_run;
//                    --run_b;
//                }
//                break;
//        case 5:
//            while(run_b>run_b_stop)
//            {
//                *loc_run+= (*run_b<<2)+*run_b;
//                --loc_run;
//                --run_b;
//            }
//            break;
//        case 6:
//            while(run_b>run_b_stop)
//                {
//                    *loc_run+= (*run_b<<2)+(*run_b<<1);
//                    --loc_run;
//                    --run_b;
//                }
//                break;
//        case 7:
//            while(run_b>run_b_stop)
//                {
//                    *loc_run+= (*run_b<<2)+(*run_b<<1) +*run_b;
//                    --loc_run;
//                    --run_b;
//                }
//                break;
//        case 8:
//            while(run_b>run_b_stop)
//            {
//                *loc_run+= (*run_b<<3);
//                --loc_run;
//                --run_b;
//            }
//            break;
//        case 9:
//            while(run_b>run_b_stop)
//            {
//                *loc_run+= (*run_b<<3)+*run_b;
//                --loc_run;
//                --run_b;
//            }
//            break;
//        default:
//            break;
//        }
//
//
//
////        if(*run_a==5)
////        {
////            //5
////
////
////        }
////        else if(*run_a<5)
////        {
////            if(*run_a==3)
////            {
////                //3
////
////            }
////            else if(*run_a<3)
////            {
////                if(*run_a==1)
////                {
////                    //1
////
////                }
////                else if (*run_a<1)
////                {
////                    //0
////
////                }
////                else
////                {
////                    //2
////
////
////                }
////
////            }
////            else
////            {
////                //4
////
////
////
////            }
////
////        }
////        else
////        {
////            if(*run_a==7)
////            {
////                //7
////
////            }
////            else if(*run_a<7 )
////            {
////                //6
////
////            }
////            else
////            {
////                if(*run_a==8)
////                {
////                    //8
////
////
////                }
////                else
////                {
////                    //9
////
////                }
////            }
////
////        }
//
//
//        --loc_buf_end;
//        --run_a;
//    }
//    loc_run=&loc_buf[n+m-1];
//    run_ans=ans->end;
//    t=0;
//    while(loc_run>=loc_buf)
//    {
//        s = t + *loc_run;
//        if(s<0)
//        {
//            free(loc_buf);
//            BNreset(ans);
//            return BN_OVERFLOW;
//        }
//        *run_ans= s % 10;
//        t = s / 10;
//        --run_ans;
//        --loc_run;
//    }
//    ans->start=++run_ans;
//    while(*ans->start==0 && ans->start<ans->end)
//        ++ans->start;
//
//    free(loc_buf);
//    return res;
//
//}


BNERR_NUMBER BNtimes_10x_eq(pBigNumber a,BNu64 x)
{
    BNERR_NUMBER res=BN_OK;
    if( (res=BNcheck_rg_buffer(a,x+2))!=BN_OK)
        return res;
    while(x)
    {
        ++a->end;
        *a->end=0;
        --x;
    }
    a->end[1]='\0';
    // --a->end;
    return res;

}
BNERR_NUMBER BNtimes_10x(pBigNumber a,BNu64 x, pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    BNreset_lf(ans);
    if( (res=BNcheck_buffer_both_sides(ans,BNlen(a)+1,x+2))!=BN_OK)
        return res;
    if( (res =BNcopy(a,ans))!=BN_OK)
    {
#if BN_IS_DEBUG
        assert(0);
#endif // BN_IS_DEBUG
        BNreset(ans);
        return res;
    }
    while(x)
    {
        ++ans->end;
        *ans->end=0;
        --x;
    }
    ans->end[1]='\0';

    return res;
}
BNERR_NUMBER BNtimes_eq(pBigNumber a,pBigNumber b)
{
    BNu64 max_size;
    pBigNumber tmp;
    BNERR_NUMBER res=BN_OK;
    max_size=BNlen(a)>BNlen(b)?BNlen(a)*2+1:BNlen(b)*2+1;
    tmp=BNnew_set(max_size,BNDEFAULTBUF);
    if(!tmp)
        return BN_RUN_OUT_OF_MEMORY;
    if( (res=BNtimes(a,b,tmp))!=BN_OK )
    {
        BNreset(a);
        BNdestroy(tmp);
        return res;
    }
    free(a->buffer);
    a->buffer=tmp->buffer;
    a->start=tmp->start;
    a->end=tmp->end;
    a->tail=tmp->tail;
    free(tmp);
    return res;
}
BNERR_NUMBER BNfactorial(pBigNumber a,pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber tmp;
    BNint_to_bn(ans,1);
    if(BNis_zero(a))
    {

        return res;
    }
    else if(BNis_one(a))
    {
        return res;
    }
    tmp=BNclone(a);
    if(!tmp)
    {
        BNreset(ans);
        return BN_RUN_OUT_OF_MEMORY;
    }
    while(!BNis_one(tmp))
    {
        if( (res=BNtimes_eq(ans,tmp))!=BN_OK)
        {
            BNdestroy(tmp);
            BNreset(ans);
            return res;
        }
        if((res=BNminus_minus(tmp))!=BN_OK )
        {
            BNdestroy(tmp);
            BNreset(ans);
            return res;
        }
    }
    return res;
}
BNERR_NUMBER BNtimes_safe(pBigNumber a,pBigNumber b,pBigNumber ans )
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber _a,_b,tmp;
    register char *run_b;
    BNu64 tmp_end_position;
    BNreset(ans);
    if( (res=BNcheck_buffer(ans,BNlen(a)+BNlen(b)+1 ))!=BN_OK)
    {

        return res;
    }

    if(BNis_zero(a)|| BNis_zero(b) )
    {

        return res;
    }
    if(BNlen(b)>BNlen(a))
    {
        _a=b;
        _b=a;
    }
    else
    {
        _a=a;
        _b=b;
    }
    tmp=BNnew_set(BNlen(_a)+1,BNlen(_b)+1);
    if(!tmp)
    {
        BNreset(ans);
        return BN_RUN_OUT_OF_MEMORY;
    }
    run_b=_b->end;
    tmp_end_position=tmp->tail-tmp->end;
    while(run_b>=_b->start)
    {
        tmp->end=tmp->tail-tmp_end_position;
        BNreset(tmp);
        if(*run_b==0)
        {
            --run_b;
            continue;
        }
        else if(*run_b==1)
        {
            if( (res=BNcopy(_a,tmp))!=BN_OK )
            {
                BNreset(ans);
                return res;
            }
        }
        else
        {
            if((res=BNtimes_u(_a,*run_b,tmp) )!=BN_OK)
            {
                BNreset(ans);
                return res;
            }
        }
        if((res=BNtimes_10x_eq(tmp,_b->end-run_b))!=BN_OK )
        {
            BNreset(ans);
            return res;
        }
        if((res=BNplus_eq(ans,tmp))!=BN_OK )
        {
            BNreset(ans);
            return res;
        }
        --run_b;
    }
    return res;
}
BNERR_NUMBER BNtimes_eq_safe(pBigNumber a,pBigNumber b)
{
    BNu64 max_size;
    pBigNumber tmp;
    BNERR_NUMBER res=BN_OK;
    max_size=BNlen(a)>BNlen(b)?BNlen(a)*2+1:BNlen(b)*2+1;
    tmp=BNnew_set(max_size,BNDEFAULTBUF);
    if(!tmp)
        return BN_RUN_OUT_OF_MEMORY;
    if( (res=BNtimes_safe(a,b,tmp))!=BN_OK )
    {
        BNreset(a);
        BNdestroy(tmp);
        return res;
    }
    free(a->buffer);
    a->buffer=tmp->buffer;
    a->start=tmp->start;
    a->end=tmp->end;
    a->tail=tmp->tail;
    free(tmp);
    return res;
}

BNERR_NUMBER BNdivide(pBigNumber a,pBigNumber b,pBigNumber ans,pBigNumber remainder )
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber loc_bn;
    int cmp;
    int first=1;
    BNu64 n,m;
    char *end_remainder,tmp_int,cc;
    if(*b->start==0 && b->start!=b->end)
        printf_bn(b);
    if(BNis_zero(b))
    {
        BNreset(ans);
        BNreset(remainder);
        return BN_DIVISION_BY_ZERO;
    }
    cmp=BNcmp(a,b);
    if(cmp<0)
    {
        BNreset(ans);
        res=BNcopy(b,remainder);

        return res;
    }
    else if(cmp==0)
    {
        BNint_to_bn(ans,1);
        BNreset(remainder);

        return res;
    }
    n=BNlen(a);
    m=BNlen(b);
    BNreset_lf(ans);
    BNreset(remainder);
    if((res=BNcheck_rg_buffer(ans,n+1))!=BN_OK )
    {
        BNreset(ans);
        BNreset(remainder);
        return res;
    }
    if((res=BNcopy(a,remainder))!=BN_OK)
    {
        BNreset(ans);
        BNreset(remainder);
        return res;
    }
    loc_bn=BNnew_set(BNlen(a)+1,BNDEFAULTBUF);
    if(!loc_bn)
    {
        BNreset(ans);
        BNreset(remainder);
        return BN_RUN_OUT_OF_MEMORY;

    }

    end_remainder=remainder->end;


    remainder->end=remainder->start+m-1;
    if(BNlt(remainder,b))
        ++remainder->end;
    while(remainder->end<=end_remainder)
    {
        if(BNlen(remainder)>BNlen(b))
            tmp_int=remainder->start[0]*10+remainder->start[1];
        else if (BNlen(remainder)==BNlen(b))
            tmp_int=*remainder->start;
        else
        {
            *++ans->end=0;
            ans->end[1]='\0';
            if(! (*remainder->start))
                ++remainder->start;
            ++remainder->end;
            continue;
        }

        cc=tmp_int/ b->start[0];
        if(cc>9)
            cc=9;
        if((res=BNtimes_u(b,cc,loc_bn))!=BN_OK )
        {
            BNreset(ans);
            BNreset(remainder);
            BNdestroy(loc_bn);
            return res;
        }
        while(BNgt(loc_bn,remainder ))
        {
            --cc;
            if((res=BNtimes_u(b,cc,loc_bn))!=BN_OK )
            {
                BNreset(ans);
                BNreset(remainder);
                BNdestroy(loc_bn);
                return res;
            }
        }

        if(first)
        {
            *ans->end=cc;
            first=0;
        }
        else
        {
            *++ans->end=cc;
            ans->end[1]='\0';
        }

        if((res=BNminus_eq(remainder,loc_bn))!=BN_OK )
        {
            BNreset(ans);
            BNreset(remainder);
            BNdestroy(loc_bn);
            return res;

        }
        ++remainder->end;

    }
    --remainder->end;
//    while(*remainder->start==0 &&remainder->start<remainder->end)
//        ++remainder->start;
    BNdestroy(loc_bn);

    return res;
}
BNERR_NUMBER BNdivide_by_2(pBigNumber a,pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    int carryover=0;
    register char *run_a,*run_ans;
    if((res=BNcheck_buffer(ans,BNlen(a)))!=BN_OK )
    {
        BNreset(ans);
        return res;
    }
    run_a =a->start;
    run_ans=ans->start=ans->end-BNlen(a)-1;
    while(run_a<=a->end)
    {
        *run_ans=((*run_a)>>1)+carryover;
        carryover=((*run_a) &1)*5;
        ++run_a;
        ++run_ans;
    }
    *run_ans='\0';
    ans->end=--run_ans;
    while(*ans->start==0 && ans->start<ans->end)
        ++ans->start;

    return BN_OK;
}
BNERR_NUMBER BNdivide_by_2_eq(pBigNumber a)
{
    pBigNumber tmp;
    BNERR_NUMBER res=BN_OK;
    tmp=BNnew_set(BNlen(a)+1,BNRIGHTBUF);
    if((res=BNdivide_by_2(a,tmp))!=BN_OK )
        return res;
    free(a->buffer);
    a->buffer=tmp->buffer;
    a->start=tmp->start;
    a->end=tmp->end;
    a->tail=tmp->tail;
    free(tmp);
    return res;
}
BNERR_NUMBER BNsqrt(pBigNumber a,pBigNumber ans)
{
    pBigNumber lf,rg,mid,prod;
    BNERR_NUMBER res=BN_OK;
    if( (res=BNcheck_buffer(ans,3+ (BNlen(a)>>1) ))!=BN_OK)
        goto ERR1;
    BNreset(ans);
    if(BNlen(a)>5)
    {
        BNu64 len_n,len_buf;
        len_n=BNlen(a);
        if(len_n&1)
        { /// the smallest odd digits len is 10000, the sqr 100

            len_buf=(len_n>>1);
            if(!(lf=BNnew_from_int(1))) {res=BN_RUN_OUT_OF_MEMORY;goto ERR1;};
            if((res=BNtimes_10x_eq(lf,len_buf)) !=BN_OK) goto ERR2;
//            printf("lf:");
//            printf_bn(lf);
        }
        else
        { /// the smallest even digits len is 100.000, the sqr 316

            len_buf=(len_n>>1)-1;
            if(!(lf=BNnew_from_int(3))) {res=BN_RUN_OUT_OF_MEMORY;goto ERR1;};
            if((res=BNtimes_10x_eq(lf,len_buf)) !=BN_OK) goto ERR2;
//            printf("lf:");
//            printf_bn(lf);
        }

    }
    else
    {
        lf=BNnew_from_int(1);
        if(!lf)
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR1;
        };
    }

    if( BNlen(a) >5)
    {
        rg=BNnew_default();
        if(!rg)
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR2;
        }
        if((res=BNrough_sqrt(a,rg))!=BN_OK) goto ERR3;
//        printf("rg:");
//        printf_bn(rg);
    }
    else
    {
        rg=BNnew_from_bn(a);
        if(!rg)
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR2;
        }
    }


    mid=BNnew_set(BNlen(a),BNDEFAULTBUF );
    if(!mid)
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR3;
    }

    prod=BNnew_set(BNlen(a),BNDEFAULTBUF);
    if(!prod)
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR4;
    }

    //if( (res=BNdivide_by_2_eq(rg))!=BN_OK) goto ERR5;
    BNreset(mid);
    while(BNlt_eq(lf,rg))
    {
        if((res=BNplus_eq(mid,lf))!=BN_OK) goto ERR5;
        if((res=BNplus_eq(mid,rg))!=BN_OK) goto ERR5;
        if((res=BNdivide_by_2_eq(mid))!=BN_OK) goto ERR5;
        if((res=BNtimes(mid,mid,prod))!=BN_OK) goto ERR5;
        if(BNlt_eq(prod,a))
        {
            if((res=BNcopy(mid,ans))!=BN_OK )goto ERR5;
            if((res=BNplus_plus(mid))!=BN_OK ) goto ERR5;
            if((res=BNcopy(mid,lf))!=BN_OK ) goto ERR5;
        }
        else
        {
            if((res=BNminus_minus(mid))!=BN_OK ) goto ERR5;
            if((res=BNcopy(mid,rg))!=BN_OK ) goto ERR5;
        }
        BNreset(mid);
    }
ERR5:
    BNdestroy(prod);
ERR4:
    BNdestroy(mid);
ERR3:
    BNdestroy(rg);
ERR2:
    BNdestroy(lf);
ERR1:
    return res;
}
BNERR_NUMBER BNsqrt_remainder(pBigNumber a,pBigNumber ans,pBigNumber remainder)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber tmp=BNnew_default();
    if(!tmp) return BN_RUN_OUT_OF_MEMORY;
    if((res=BNsqrt(a,ans))!=BN_OK) goto ERR1;
    if((res=BNtimes(ans,ans,tmp))!=BN_OK ) goto ERR1;
    res=BNminus(a,tmp,remainder);

ERR1:
    BNdestroy(tmp);
    return res;
}

BNERR_NUMBER BNrough_sqrt(pBigNumber a,pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    BNu64 n_len,buf_len;

    n_len=BNlen(a);
    if(n_len & 1)
    {
        /// The biggest number with odd len is 99999
        /// the sqrt is 316 so half len + 1
        buf_len=(n_len>>1);
        BNreset_lf(ans);
        if((res=BNint_to_bn(ans,4))!=BN_OK) return res;
        if((res=BNtimes_10x_eq(ans,buf_len))!=BN_OK) return res;

    }else
    {
        /// The biggest number with even len is for  9999
        /// the sqrt is 99.9 rounded to 100 so half len + 1
        buf_len=(n_len>>1);
        BNreset_lf(ans);
        if((res=BNint_to_bn(ans,1))!=BN_OK) return res;
        if((res=BNtimes_10x_eq(ans,buf_len))!=BN_OK) return res;

    }
    return res;

}
BNERR_NUMBER BNpow(pBigNumber a, BNu64 pow, pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber base;

    base=BNnew_from_bn(a);
    if(!base)
    {
        return BN_RUN_OUT_OF_MEMORY;
    }
    if( (res=BNint_to_bn(ans,1))!=BN_OK )
    {
        BNreset(ans);
        goto ERR1;
    }
    while(pow)
    {
        if(pow&1)
        {
            if((res=BNtimes_eq(ans,base))!=BN_OK)
            {
                BNreset(ans);
                goto ERR1;
            }
            if(pow==1) break;

        }
        if((res=BNtimes_eq(base,base))!=BN_OK )
        {
            BNreset (ans);
            goto ERR1;
        }

        pow>>=1;
    }

ERR1:
    BNdestroy(base);
    return res;
}

BNERR_NUMBER BNmod_u(pBigNumber n, BNu64 u, BNu64 *ans )
{
    BNERR_NUMBER res=BN_OK;
    register char *run_n;
    if(u==0) return BN_DIVISION_BY_ZERO;
    run_n=n->start;
    *ans=0;
    while( run_n<=n->end )
    {
        *ans=( (*ans<<3)+(*ans<<1)+*run_n)%u;
        //*ans=( (*ans*10)+*run_n)%u;
        ++run_n;
    }

    return res;

}
BNERR_NUMBER BNgcd(pBigNumber a,pBigNumber b,pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber la,lb,lans,lremainder;
    BNreset(ans);
    if(BNis_zero(a))
    {
        res=BNcopy(b,ans);
        return res;
    }
    if(BNis_zero(b))
    {
        res=BNcopy(a,ans);
        return res;
    }
    if(BNgt(a,b))
    {
        if(!(la=BNnew_from_bn(a)))
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR1;
        };
        if(!(lb=BNnew_from_bn(b)))
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR2;
        };
    }
    else
    {
        if(!(la=BNnew_from_bn(b)))
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR1;
        };
        if(!(lb=BNnew_from_bn(a)))
        {
            res=BN_RUN_OUT_OF_MEMORY;
            goto ERR2;
        };
    }

    if (!(lans=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR3;
    };
    if (!(lremainder=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR4;
    };
    while(!BNis_zero(lb))
    {

        if((res=BNdivide(la,lb,lans,lremainder))!=BN_OK  ) goto ERR5;

        if((res=BNcopy(lb,la))!=BN_OK) goto ERR5;

        if((res=BNcopy(lremainder,lb))!=BN_OK) goto ERR5;


    }
    if((res=BNcopy(la,ans))!=BN_OK) goto ERR5;
ERR5:
    BNdestroy(lremainder);
ERR4:
    BNdestroy(lans);
ERR3:
    BNdestroy(lb);
ERR2:
    BNdestroy(la);
ERR1:

    return res;
}
BNERR_NUMBER BNlcm(pBigNumber a,pBigNumber b, pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber gcd, tms,rem;
    if((gcd=BNnew_default())==NULL) goto ERR1;
    if((tms=BNnew_default())==NULL) goto ERR2;
    if((res=BNtimes(a,b,tms))!=BN_OK ) goto ERR3;
    if((res=BNgcd(a,b,gcd))!=BN_OK ) goto ERR3;
    printf_bn(gcd);
    if((rem=BNnew_default())==NULL ) goto ERR3;
    res=BNdivide(tms,gcd,ans,rem);

    BNdestroy(rem);
ERR3:
    BNdestroy(tms);
ERR2:
    BNdestroy(gcd);
ERR1:
    return res;
}
BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber tmp,loc_a;
    BNu64 u_mod,tmp_mod;
    if(BNis_zero(mod))
    {
        BNreset(ans);
        return BN_DIVISION_BY_ZERO;
    }
    if((res=BNint_to_bn(ans,1))!=BN_OK)return res;
    if(!(tmp=BNnew_default()))
    {
        return BN_RUN_OUT_OF_MEMORY;
    }
    if(!(loc_a=BNnew_from_bn(a)))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR1;
    }
    if(BNlt_eq(mod,max_ull_size))
    {
        BNto_u(mod,&u_mod);
        while(1)
        {
            if(pow&1)
            {
                if( (res=BNtimes(ans,loc_a,tmp))!=BN_OK) goto ERR2;

                if(BNis_zero(tmp))
                    BNreset(ans);
                else
                {
                    BNmod_u(tmp,u_mod,&tmp_mod);
                    BNint_to_bn(ans,tmp_mod);
                }
                if(pow==1) break;
            }
            pow>>=1;
            if((res=BNpow(loc_a,2,tmp))!=BN_OK) goto ERR2;

            if(BNis_zero(tmp))
                BNreset(loc_a);
            else
            {
                BNmod_u(tmp,u_mod,&tmp_mod);
                BNint_to_bn(loc_a,tmp_mod);
            }
        }

    }
    else
    {
        if((res=BNpow(a,pow,tmp))!=BN_OK ) goto ERR2;
        res=BNdivide(tmp,mod,loc_a,ans);

    }

ERR2:
    BNdestroy(loc_a);
ERR1:
    BNdestroy(tmp);
    return res;
}


BNERR_NUMBER BNfactorize_rho(pBigNumber n, pBigNumber ans)
{
    BNERR_NUMBER res=BN_OK;
    pBigNumber x,y,tmp,m,h,c;
    BNu64 len_rand_bn;
    int i=0,z=0;
    clock_t t1,t2;

    if(BNis_one(n))
    {
        res=BNcopy(n,ans);
        return res;
    }
    if(BNis_even(n))
    {
        res=BNint_to_bn(ans,2);
        return res;
    }
    if(!(tmp=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR1;
    }
    if(!(m=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR2;
    }
    if(!(h=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR3;
    }
    len_rand_bn=BNlen(n)>1?BNlen(n)-1:1;
    if(!(x=BNnew_rand(len_rand_bn,BNTRUE)))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR4;
    }
    if(BNgt(x,n))
    {
        if((res=BNminus(x,n,m))!=BN_OK) goto ERR5;
        if((res=BNminus_eq(x,m))!=BN_OK) goto ERR5;
    }
    if(!(y=BNnew_from_bn(x)))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR5;
    }
    if(!(c=BNnew_rand(len_rand_bn,BNFALSE)))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR6;
    }


    if(BNgt(c,n))
    {
        if((res=BNminus(c,n,m))!=BN_OK) goto ERR6;
        if((res=BNminus_eq(c,m))!=BN_OK) goto ERR6;
    }
    BNint_to_bn(ans,1);
    t1=clock();
    while (BNis_one(ans))
    {
        i++;

        if(i>=100000)
        {
            z++;
            i=0;
            t2=clock();
            printf("%d 100k %f\n",z, (float)(t2-t1)/CLOCKS_PER_SEC);
            t1=clock();
        }
        if( (res=BNmodular_pow(x,2,n,tmp))!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,c) )!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,n) )!=BN_OK) goto ERR8;
        if( (res=BNdivide(tmp,n,h,x) )!=BN_OK) goto ERR8;

        if( (res=BNmodular_pow(y,2,n,tmp))!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,c) )!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,n) )!=BN_OK) goto ERR8;
        if( (res=BNdivide(tmp,n,h,y) )!=BN_OK) goto ERR8;

        if( (res=BNmodular_pow(y,2,n,tmp))!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,c) )!=BN_OK) goto ERR8;
        if( (res=BNplus_eq(tmp,n) )!=BN_OK) goto ERR8;
        if( (res=BNdivide(tmp,n,h,y) )!=BN_OK) goto ERR8;
        if( BNgt(x,y))
        {
            if((res=BNminus(x,y,tmp))!=BN_OK ) goto ERR8;
        }
        else
        {
            if((res=BNminus(y,x,tmp))!=BN_OK ) goto ERR8;
        }

        if((res=BNgcd(tmp,n,ans))!=BN_OK) goto ERR8;

        if(BNeq(n,ans))
        {
            break;
            if( (res=BNfactorize_rho(n,ans))!=BN_OK)
                goto ERR8;

        }

    }
ERR8:

//ERR7:
    BNdestroy(c);
ERR6:
    BNdestroy(y);
ERR5:
    BNdestroy(x);
ERR4:
    BNdestroy(h);
ERR3:
    BNdestroy(m);
ERR2:
    BNdestroy(tmp);
ERR1:
    return res;
}


BNERR_NUMBER BNreverse(pBigNumber a,pBigNumber b)
{
    BNERR_NUMBER res=BN_OK;
    register char *run_a,*run_b;
    char *run_a_stop;
    if((res=BNcheck_buffer(b,BNlen(a)+1))!=BN_OK  )
        return res;
    run_a=a->start;
    run_b=b->end;
    run_a_stop=a->end+1;
    while(run_a<run_a_stop)
    {
        *run_b=*run_a;
        ++run_a;
        --run_b;
    }
    b->start=++run_b;
    return res;


}
int BNis_palindrome(pBigNumber a)
{
    register char *run_end,*run_start;
    run_end=a->end;
    run_start=a->start;
    while(*run_end==*run_start && run_start<run_end )
    {
        ++run_start;
        --run_end;
    }
    return run_start>=run_end;
}


pVectFact BNnew_vect_fact()
{
    pVectFact res =malloc(sizeof(VectFact));
    if(!res) return NULL;
    res->pos=0;
    res->sz=BNSTARTVECTFACTLEN;
    res->p=calloc(res->sz,sizeof(Factorize) );
    if(!res->p)
    {
        free(res);
        return NULL;
    }
    return res;
}

BNERR_NUMBER BNpush_n(pVectFact vf,pBigNumber n )
{
    BNERR_NUMBER res=BN_OK;
    if(vf->pos>=vf->sz)
    {
        size_t new_size=vf->sz*2;
        pFactorize tmp=realloc(vf->p,sizeof(Factorize)*new_size);
        if(!tmp) return BN_RUN_OUT_OF_MEMORY;
        vf->sz=new_size;
        vf->p=tmp;
    }
    vf->p[vf->pos].bn=BNclone(n);
    if(!vf->p[vf->pos].bn) return BN_RUN_OUT_OF_MEMORY;
    vf->p[vf->pos++].cnt=1;
    return res;

}
int incr_counter(pVectFact vf)
{
    ;
    return 1;
}

void BNdestroy_vect_fact(pVectFact vf )
{
    while(vf->pos)
        BNdestroy(vf->p[--vf->pos].bn);
    free(vf->p);
    free(vf);
}


static BNERR_NUMBER BNfactorize_start_from(pBigNumber n, pBigNumber start, pVectFact ans)
{
    pBigNumber sqr,remainder,i,tmp_ans;
    BNERR_NUMBER res=BN_OK;
    BNu64 u_mod,u_sqrt,u_i;
    size_t cur_pos;
    int first;
    if ( !(sqr=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR1;
    };
    if(!(remainder=BNnew_default()))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR2;
    };
    if(!(i=BNnew_default()))
    {

        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR3;
    };
    if(!(tmp_ans=BNnew_default() ))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR4;
    }
    first=1;
    BNto_u(start,&u_i);

    if(u_i==2 && BNis_even(n))
    {
        BNint_to_bn(i,2);
        while(BNis_even(n))
        {
            if(first)
            {
                if ((res=BNpush_n(ans,i))!=BN_OK) goto ERR5;;
                first=0;
            }
            else
            {
                BNincr_counter(ans);

            }
            BNdivide_by_2_eq(n);
        }

      //  printf("2^%d\n",ans->p[0].cnt);
    }
    if(u_i==2)
        BNint_to_bn(i,3);
    else
        BNint_to_bn(i,u_i);
    if(BNlen(n)>1000  )
    {
        if ((res=BNrough_sqrt(n,sqr))!=BN_OK) goto ERR5;
    }
    else
    {
        pBigNumber rem,ind;
        if ( !(rem=BNnew_default())) goto ERR5;
        if ((res=BNsqrt_remainder(n,sqr,rem))!=BN_OK)
        {
            BNdestroy(rem);
            goto ERR5;
        };
        if(BNis_zero(rem))
        {
            size_t l_i;
            cur_pos=ans->pos;
            if(!(ind=BNnew_from_int(3)))
            {
                BNdestroy(rem);
                res=BN_RUN_OUT_OF_MEMORY;
                goto ERR5;
            }
            if ( (res=BNfactorize_start_from(sqr,ind,ans)!= BN_OK))
            {
                BNdestroy(rem);
                BNdestroy(ind);
                goto ERR5;
            }
            BNint_to_bn(n,1);
            for(l_i=cur_pos; l_i<ans->pos; l_i++)
                ans->p[l_i].cnt*=2;

            BNdestroy(ind);
        }
        BNdestroy(rem);
    }


    BNto_u(sqr,&u_sqrt);

   if(u_i==2)
        u_i=3;
    while(u_i<=u_sqrt && u_i<=0xFFFFFFFFFFFFFF00 )
    {
        if((res=BNmod_u(n,u_i,&u_mod))!=BN_OK) goto ERR5 ;
        if(u_mod==0)
        {
            if( (res=BNint_to_bn(i,u_i))!=BN_OK)goto ERR5;
            if ((res=BNpush_n(ans,i)!=BN_OK)) goto ERR5;
            if((res=BNdivide(n,i,tmp_ans,remainder)!=BN_OK)) goto ERR5;
            do
            {

                if((res=BNcopy(tmp_ans,n))!=BN_OK) goto ERR5;
                if((res=BNdivide(n,i,tmp_ans,remainder))!=BN_OK) goto ERR5;
                if(BNis_zero(remainder))
                    BNincr_counter(ans);
                else
                    break;
            }
            while(1);
//                printf("%s^%d\n",BNto_string_inplace(i),vf->p[vf->pos-1].cnt);
//                BNto_number_inplace(i);
            if(BNlen(n)>1000)
            {
                if((res=BNrough_sqrt(n,sqr))!=BN_OK) goto ERR5 ;
            }
            else
            {
                pBigNumber rem,ind;
                if ( !(rem=BNnew_default())) goto ERR5;
                if ((res=BNsqrt_remainder(n,sqr,rem))!=BN_OK)
                {
                    BNdestroy(rem);
                    goto ERR5;
                };
                if(BNis_zero(rem))
                {
                    size_t l_i;
                    cur_pos=ans->pos;
                    if(!(ind=BNnew_from_int(u_i)))
                    {
                        BNdestroy(rem);
                        res=BN_RUN_OUT_OF_MEMORY;
                        goto ERR5;
                    }
                    if ( (res=BNfactorize_start_from(sqr,ind,ans)!= BN_OK))
                    {
                        BNdestroy(rem);
                        BNdestroy(ind);
                        goto ERR5;
                    }
                    BNint_to_bn(n,1);
                    for(l_i=cur_pos; l_i<ans->pos; l_i++)
                        ans->p[l_i].cnt*=2;
                    BNdestroy(ind);
                }
                BNdestroy(rem);
            }
            BNto_u(sqr,&u_sqrt);
        }
        u_i+=2;
    }

    BNint_to_bn(i,u_i);

    if(BNlt(i,sqr) && BNlen(n)>1000)
    {
         if( (res=BNrough_sqrt(n,sqr))!=BN_OK) goto ERR5;
    }
    else if(BNlt(i,sqr))
    {
        pBigNumber rem,ind;
        if ( !(rem=BNnew_default())) goto ERR5;
        if ((res=BNsqrt_remainder(n,sqr,rem))!=BN_OK)
        {
            BNdestroy(rem);
            goto ERR5;
        };
        if(BNis_zero(rem))
        {
            size_t l_i;
            cur_pos=ans->pos;
            if(!(ind=BNnew_from_int(u_i)))
            {
                BNdestroy(rem);
                res=BN_RUN_OUT_OF_MEMORY;
               goto ERR5;
            }
            if ( (res=BNfactorize_start_from(sqr,ind,ans)!= BN_OK))
            {
                BNdestroy(rem);
                BNdestroy(ind);
                goto ERR5;
            }
            for(l_i=cur_pos; l_i<ans->pos; l_i++)
                ans->p[l_i].cnt*=2;

            BNdestroy(ind);
        }
        BNdestroy(rem);
    }

    while(BNlt_eq(i,sqr))
    {
//            ++n_loops;
//            if(n_loops==1000000)
//            {
//                t4=clock();
//                printf("1M %f\n",(float)(t4-t3)/CLOCKS_PER_SEC);
//                t3=clock();
//                n_loops=0;
//            }
        if ((res=BNdivide(n,i,tmp_ans,remainder))!=BN_OK) goto ERR5;
        if(BNis_zero(remainder)  )
        {
            if((res=BNpush_n(ans,i))) goto ERR5;
            do
            {
                if((res=BNcopy(tmp_ans,n))!=BN_OK) goto ERR5;
                if((res=BNdivide(n,i,tmp_ans,remainder))!=BN_OK) goto ERR5;
                if(BNis_zero(remainder))
                    BNincr_counter(ans);
                else
                    break;
            }
            while(1);
            if(BNlen(n)>1000)
                {if ((res=BNrough_sqrt(n,sqr))!=BN_OK) goto ERR5;}
            else
            {
                pBigNumber rem,ind;
                if ( !(rem=BNnew_default())) goto ERR5;
                if ((res=BNsqrt_remainder(n,sqr,rem))!=BN_OK)
                {
                    BNdestroy(rem);
                    goto ERR5;
                };
                if(BNis_zero(rem))
                {
                    size_t l_i;
                    cur_pos=ans->pos;
                    if(!(ind=BNnew_from_bn(i)))
                    {
                        BNdestroy(rem);
                        res=BN_RUN_OUT_OF_MEMORY;
                        goto ERR5;
                    }
                    if ( (res=BNfactorize_start_from(sqr,ind,ans)!= BN_OK))
                    {
                        BNdestroy(rem);
                        BNdestroy(ind);
                        goto ERR5;
                    }
                    BNint_to_bn(n,1);
                    for(l_i=cur_pos; l_i<ans->pos; l_i++)
                        ans->p[l_i].cnt*=2;

                    BNdestroy(ind);
                }
                BNdestroy(rem);
            }
        }
        BNplus_u_eq(i,2);
    }
    if(!BNis_one(n))
        BNpush_n(ans,n);
    BNint_to_bn(n,1);
ERR5:
    BNdestroy(tmp_ans);
ERR4:
    BNdestroy(i);
ERR3:
    BNdestroy(remainder);
ERR2:
    BNdestroy(sqr);
ERR1:
    return res;
}

BNERR_NUMBER BNfactorize(pBigNumber n, pVectFact ans)
{
    pBigNumber tmp,ex_n;
    BNERR_NUMBER res=BN_OK;

    tmp=BNnew_from_int(2);
    if(!tmp) return BN_RUN_OUT_OF_MEMORY;
    if( !(ex_n=BNnew_from_bn(n)))
    {
        res=BN_RUN_OUT_OF_MEMORY;
        goto ERR1;
    };

    res=BNfactorize_start_from(ex_n,tmp,ans);
    BNdestroy(ex_n);
ERR1:
    BNdestroy(tmp);
    return res;
}
