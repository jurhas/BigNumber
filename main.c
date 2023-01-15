#include <stdio.h>
#include <stdlib.h>
#include "bignumbers.h"
#include "time.h"
#define reset_string(a) ((a)->pos=0,*(a)->buf='\0')


pBigNumber get_bn_from_console();
char choose();

typedef struct
{
    char *buf;
    size_t buf_size;
    size_t pos;
} String,*pString;


pString new_string(size_t size)
{
    pString res=malloc(sizeof(String));
    if(!res) return NULL;
    res->buf_size=size+1;
    res->buf=malloc(res->buf_size);
    if(!res->buf)
    {
        free(res);
        return NULL;
    }
    res->pos=0;
    *res->buf='\0';
    return res;
}
char* concatc(pString st, int c)
{
    if(st->buf_size-st->pos<=1)
    {
        char *tmp;
        size_t tmp_buf_sz=st->buf_size*2;
        tmp=realloc(st->buf,tmp_buf_sz);
        if(!tmp)
            return NULL;
        st->buf=tmp;
        st->buf_size=tmp_buf_sz;
    }
    st->buf[st->pos]=c;
    st->buf[++st->pos]='\0';
    return st->buf;
}
void destroy_string(pString st)
{
    free(st->buf);
    free(st);
}
BNERR_NUMBER get_string(pString st,char * msg)
{
    int c;
    printf("%s\n",msg);
    while ((c = getchar()) == '\n')
        ;
    do{
        if(!concatc(st,c))
            return BN_RUN_OUT_OF_MEMORY;

    } while ((c = getchar()) != '\n');

    return BN_OK;
}
int printf_vf(pVectFact vf)
{
    size_t i;
    for (i=0; i<vf->pos; i++)
    {
        printf("%s ^ %d\n",BNto_string_inplace(vf->p[i].bn),vf->p[i].cnt);
        BNto_number_inplace(vf->p[i].bn);
    }
    return 1;
}
int factorize(pBigNumber n, int save_to_file )
{

//        if(save_to_file)
//        {
//            FILE*f;
//            size_t i;
//            char tmp[100];
//            f=fopen("fact.txt","wb");
//            for(i=0;i<vf->pos;i++)
//            {
//                fputs( BNto_string_inplace( vf->p[i].bn),f );
//                fputs(" ^ ",f);
//                sprintf(tmp,"%d\n",vf->p[i].cnt);
//                fputs(tmp,f);
//            }
//            fclose(f);
//        }
//        BNdestroy(sqr);
//        BNdestroy(remainder);
//        BNdestroy(ans);
//        while(vf->pos)
//        {
//            BNdestroy(vf->p[--vf->pos].bn);
//        }
//        free(vf->p);
return 1;
}

pBigNumber get_bn_from_console()
{
    pString st=new_string(100);
    pBigNumber res=NULL;
    BNERR_NUMBER c;
    do
    {
        reset_string(st);
        c=get_string(st,"Get Number:");
        if(c==BN_RUN_OUT_OF_MEMORY)
        {
            printf("Not enough memory");
            continue;
        }
        res=BNnew_from_string(st->buf,&c);
        switch(c)
        {
        case BN_OK :
            break;
        case BN_RUN_OUT_OF_MEMORY:
            printf("Not Enough Memory\n");
            break;
        case BN_NUMBER_NOT_VALID:
            printf("Number Not Valid\n");
            break;
        default:
            assert(0);
        }
    }
    while(c!=BN_OK);

    return res;
}

pBigNumber get_bn_from_int()
{
    BNu64 i;
    pBigNumber res;
    printf("Get Number:\n");
    while(scanf(BNULONGLONGPATTERN,&i)==0)
    {
        printf("Number Not Valid\nGetNumber:\n");
        while(getchar()!='\n')
            ;
    }
    while (getchar() != '\n')
        ;

    res=BNnew_from_int(i);
    if(!res)
    {
        printf("Not enough memory\n");
    }
    return res;
}
BNERR_NUMBER test_comp()
{
    pBigNumber a,b;
    //BNu64 i=0;
    BNERR_NUMBER c;
    clock_t t1,t2;
    a=BNnew_from_string("563241211",&c);
    if(c!=BN_OK) return c;
    b=BNnew_from_string("5641211",&c);
    if(c!=BN_OK)
    {
        BNdestroy(a);
        return c;
    }
    t1=clock();
    while(BNlt(b,a))
    {
        BNplus_plus(b);
        //i++;
    }
    t2=clock();
    BNto_string_inplace(a);
    BNto_string_inplace(b);
    printf("%s %s  %f\n",a->start,b->start,((float)t2-t1)/CLOCKS_PER_SEC);
    system("pause");
    BNdestroy(a);
    BNdestroy(b);
    return c;
}
void printf_err(BNERR_NUMBER c)
{
    switch(c)
    {
    case BN_RUN_OUT_OF_MEMORY:
        printf("Not enough memory\n");
        break;
    case BN_DIVISION_BY_ZERO :
        printf("Division by zero\n");
        break;
    case BN_NUMBER_NOT_VALID:
        printf("Number not valid\n");
        break;
    case BN_NEGATIVE_NUMBER:
        printf("Negative Number\n");
        break;
    case BN_CANNOT_FIT_IN_A_VARIABLE:
        printf("Number does not fit in a variable\n");
        break;
    case BN_CANNOT_OPEN_FILE:
        printf("Cannot open file\n");
        break;
    case BN_OVERFLOW:
        printf("Overflow\n");
        break;
    case BN_EMPTY_FILE:
        printf("Empty File\n");
        break;
    case BN_OK:
        printf("Ok\n");
        break;
    }

}
int test_menu()
{
    int i;
    do
    {
        printf("Which function want test\n");
        printf("1)BNplus_plus\n");
        printf("2)BNplus_eq\n");
        printf("3)BNplus\n");
        printf("4)BNplus_u\n");
        printf("5)BNplus_u_eq\n");
        printf("6)BNminus_minus\n");
        printf("7)BNminus_eq\n");
        printf("8)BNminus\n");
        printf("9)BNminus_u\n");
        printf("10)BNminus_u_eq\n");
        printf("11)BNtimes_u\n");
        printf("12)BNtimes_u_eq\n");
        printf("13)BNtimes_10x_eq\n");
        printf("14)BNtimes_10x\n");
        printf("15)BNcopy\n");
        printf("16)BNclone\n");
        printf("17)BNtimes\n");
        printf("18)BNtimes_eq\n");
        printf("19)BNfactorial\n");
        printf("20)BNtimes_safe\n");
        printf("21)BNtimes_eq_safe\n");
        printf("22)BNdivide\n");
        printf("23)BNdivide_by_2\n");
        printf("24)BNdivide_by_2_eq\n");
        printf("25)BNsqrt\n");
        printf("26)BNpow\n");
        printf("27)BNmod_u\n");
        printf("28)BNcmp\n");
        printf("29)BNreverse\n");
        printf("30)BNis_palindrome\n");
        printf("31)BNnew_rand\n");
        printf("32)BNrough_sqrt\n");
        printf("33)BNgcd\n");
        printf("34)BNmodular_pow\n");
        printf("35)BNfactorize_rho\n");
        printf("36)BNfactorize\n");
        printf("37)BNlcm\n");
        printf("-1)exit\n");
        if (scanf("%d",&i)==0 || i>37)
        {
            while (getchar() != '\n')
                ;
            printf("Wrong choose\n");
        }
        else
        {
            while (getchar() != '\n')
                ;
            return i;

        }

    }
    while(1);

}
void reverse_and_sum()
{
    char c;
    int res,nloops;
    clock_t t1,t2;
    BNu64 tot_loops;

    do
    {
        printf("***********\n");
        printf("1)Attack 196\n");
        printf("2)Get a number\n");
        printf("q)Quit\n");
        printf("***********\n");
        c=getchar();
        while (getchar() != '\n')
            ;
        if(c=='1')
        {
            pBigNumber a,b;
            a=BNnew_from_int(196);
            b=BNnew_default();
            nloops=0;
            tot_loops=0;
            t1=clock();
            while((res=BNis_palindrome(a))==0 )
            {
                BNreverse(a,b);
                BNplus_eq(a,b);
                ++nloops;
                ++tot_loops;
                if(nloops==100000)
                {
                    t2=clock();
                    printf("len:" BNULONGLONGPATTERN " tot_loops:"BNULONGLONGPATTERN " t:%f\n",(BNu64)BNlen(a),tot_loops,(float)(t2-t1)/CLOCKS_PER_SEC);
                    t1=clock();
                    nloops=0;
                }
            }
        }
        else if (c=='2')
        {
            pBigNumber n,a,rev;
            BNi64 n_loops_tot;
            clock_t t1,t2;
            int  n_pal,n_iter_limit,cur_max,n_iter_executed;

            n=get_bn_from_console();
            a=BNnew_set(BNlen(n),BNDEFAULTBUF );
            rev=BNnew_set(BNlen(n),BNDEFAULTBUF );
            printf("Get iteration limit:\n");
            scanf("%d",&n_iter_limit);
            while (getchar() != '\n')
                ;
            nloops=0;
            cur_max=0;
            n_pal=0;
            n_loops_tot=0;
            n_iter_executed=0;
            BNcopy(n,a);
            t1=clock();
            while(1)
            {
                ++nloops;
                ++n_loops_tot;
                ++n_iter_executed;
                BNreverse(a,rev);
                BNplus_eq(a,rev);
                if(BNis_palindrome(a))
                {
                    ++n_pal;
                    if(n_iter_executed>=cur_max)
                    {
                        cur_max=n_iter_executed;

                        if(BNlen(a)<100 )
                            printf("n=%s n_iter=%d pal=%s pal_len=" BNULONGLONGPATTERN "\n",
                                   BNto_string_inplace(n),
                                   n_iter_executed,
                                   BNto_string_inplace(a),
                                   (BNu64)BNlen(a));
                        else
                            printf("n=%s n_iter=%d pal_len="BNULONGLONGPATTERN "\n",
                                   BNto_string_inplace(n),
                                   n_iter_executed,
                                   (BNu64) BNlen(a));
                        BNto_number_inplace(n);
                    }
                    n_iter_executed=0;
                    BNplus_plus(n);
                    BNcopy(n,a);
                }
                if(n_iter_executed>n_iter_limit)
                {
                    n_iter_executed=0;
                    BNplus_plus(n);
                    BNcopy(n,a);

                }
                if(nloops==10000000)
                {
                    nloops=0;
                    t2=clock();
                    printf("tot_loops:" BNULONGLONGPATTERN " n:%s  n_pal_found %d,t=%f\n",
                           n_loops_tot,BNto_string_inplace(n), n_pal,(float)(t2-t1)/CLOCKS_PER_SEC);
                    BNto_number_inplace(n);
                    n_pal=0;
                    t1=clock();
                }
            }


        }
        else if(c=='q')
        {

        }
        else
        {
            printf("Wrong choose\n");
        }
        while (getchar() != '\n')
            ;


    }
    while(c!='q');

}
char choose()
{
    char res;
  //  while (getchar() != '\n')
 //       ;
    printf("q to quit\n");
    res=getchar();
    while (getchar() != '\n')
        ;

    return res;
}
BNERR_NUMBER test_sum()
{
    pBigNumber a,b,ans;

    BNERR_NUMBER c;
    unsigned int u;
    int menu;
    clock_t t1,t2;
    do
    {
        menu=test_menu();

        switch(menu)
        {
        case 1: //
        {
            do
            {
                a=get_bn_from_console();
                c=BNplus_plus(a);
                if(c!=BN_OK)
                    printf_err(c);

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 2:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                c=BNplus_eq(a,b);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
                BNdestroy(b);
            }
            while(choose()!='q');

        }
        break;
        case 3:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                ans=BNnew_default();
                c=BNplus(a,b,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 4:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNnew_default();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);

                c=BNplus_u(a,u,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 5:
        {
            do
            {
                a=get_bn_from_console();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);
                c=BNplus_u_eq(a,u);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 6:
        {
            do
            {
                a=get_bn_from_console();
                c=BNminus_minus(a);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 7:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                c=BNminus_eq(a,b);
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
                BNdestroy(b);
            }
            while(choose()!='q');

        }
        break;
        case 8:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                ans=BNnew_default();
                c=BNminus(a,b,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 9:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNnew_default();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);
                c=BNminus_u(a,u,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 10:
        {
            do
            {
                a=get_bn_from_console();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);
                c=BNminus_u_eq(a,u);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 11:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNnew_default();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);
                c=BNtimes_u(a,u,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 12:
        {
            do
            {
                a=get_bn_from_console();
                printf("Get integer >=0 AND <10\n");
                scanf("%u",&u);
                c=BNtimes_u_eq(a,u);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 13:
        {
            do
            {
                a=get_bn_from_console();
                printf("Get integer:\n");
                scanf("%u",&u);
                c=BNtimes_10x_eq(a,u);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(a));
                BNto_number_inplace(a);
                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 14:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNnew_default();
                printf("Get integer:\n");
                scanf("%u",&u);
                c=BNtimes_10x(a,u,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 15:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNnew_default();
                c=BNcopy(a,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 16:
        {
            do
            {
                a=get_bn_from_console();
                ans=BNclone(a);
                if(!ans)
                {
                    printf_err(BN_RUN_OUT_OF_MEMORY);
                    BNdestroy(a);
                    break;
                }

                printf("%s\n",BNto_string_inplace(ans));
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 17:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNtimes(a,b,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 18:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                //ans=BNnew_default();
                t1=clock();
                c=BNtimes_eq(a,b);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %f\n",BNto_string_inplace(a),(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNto_number_inplace(a);
                BNdestroy(a);
                BNdestroy(b);

            }
            while(choose()!='q');

        }
        break;
        case 19:
        {
            do
            {
                a=get_bn_from_console();

                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNfactorial(a,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                    printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                //BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(ans);

            }
            while(choose()!='q');

        }
        break;
        case 20:
        {
            do
            {
                a=get_bn_from_console();
                b=get_bn_from_console();
                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNtimes_safe(a,b,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNto_number_inplace(ans);
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 22:
        {
            do
            {
                pBigNumber remainder=BNnew_default();
                a=get_bn_from_console();
                b=get_bn_from_console();

                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNdivide(a,b,ans,remainder);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %s %f\n",BNto_string_inplace(ans),BNto_string_inplace(remainder), (float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
                BNdestroy(remainder);
            }
            while(choose()!='q');

        }
        break;
        case 23:
        {
            do
            {
                a=get_bn_from_console();
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNdivide_by_2(a,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %f\n",BNto_string_inplace(ans), (float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(ans);

            }
            while(choose()!='q');

        }
        break;
        case 24:
        {
            do
            {
                a=get_bn_from_console();
                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                c=BNdivide_by_2_eq(a);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %f\n",BNto_string_inplace(a), (float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
            }
            while(choose()!='q');

        }
        break;
        case 25:
        {
            do
            {
                pBigNumber remainder=BNnew_default();
                a=get_bn_from_console();
                ans=BNnew_default();

                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                c=BNsqrt(a,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }

                printf("%s %s %f\n",BNto_string_inplace(ans),BNto_string_inplace(remainder), (float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(ans);
                BNdestroy(remainder);
            }
            while(choose()!='q');

        }
        break;
        case 26:
        {
            do
            {
                BNu64 pow;
                a=get_bn_from_console();
                printf("Get pow:\n");
                scanf(BNULONGLONGPATTERN,&pow);
                ans=BNnew_default();

                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                c=BNpow(a,pow,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                    printf("%s %f\n",BNto_string_inplace(ans), (float)(t2-t1)/CLOCKS_PER_SEC  );
                else
                    printf( BNULONGLONGPATTERN " %f\n",(BNu64)BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNdestroy(a);
                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 27:
        {
            do
            {
                BNu64 divisor,modulo;
                a=get_bn_from_console();
                printf("Get divisor:\n");
                scanf(BNULONGLONGPATTERN,&divisor);


                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                c=BNmod_u(a,divisor,&modulo);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                printf( BNULONGLONGPATTERN " %f\n",modulo,(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNdestroy(a);

            }
            while(choose()!='q');

        }
        break;
        case 28:
        {
            do
            {
                int cmp;
                a=get_bn_from_console();
                b=get_bn_from_console();

                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                cmp=BNcmp(a,b);
                t2=clock();
                /*if(c!=BN_OK)
                {
                    printf_err(c);

                }*/
                printf( "%d %f\n",cmp,(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNdestroy(a);
                BNdestroy(b);

            }
            while(choose()!='q');

        }
        break;
        case 29:
        {
            do
            {

                a=get_bn_from_console();
                b=BNnew_default();

                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                c=BNreverse(a,b);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                printf( "%s %f\n",BNto_string_inplace(b),(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNdestroy(a);
                BNdestroy(b);

            }
            while(choose()!='q');

        }
        break;
        case 30:
        {
            do
            {
                int res;
                a=get_bn_from_console();


                //b=BNnew_from_string(BIGNUMB,&c);
                t1=clock();
                res=BNis_palindrome(a);
                t2=clock();
                /*if(c!=BN_OK)
                {
                    printf_err(c);

                }*/
                printf( "%d %f\n",res,(float)(t2-t1)/CLOCKS_PER_SEC  );
                BNdestroy(a);


            }
            while(choose()!='q');

        }
        break;
        case 31:
        {
            do
            {
                pBigNumber a;

                BNu64 len_a;
                printf("Get rnd length:\n");
                scanf(BNULONGLONGPATTERN,&len_a);
                a=BNnew_rand(len_a,BNTRUE);
                printf( "%s\n",BNto_string_inplace(a));
                BNdestroy(a);


            }
            while(choose()!='q');

        }
        break;
        case 32:
        {
            do
            {

                pBigNumber a,ans;
                a=get_bn_from_console();
                ans=BNnew_default();
                c=BNrough_sqrt(a,ans);
                if(c!=BN_OK)
                {
                    printf_err(c);
                    break;
                }
                printf( "%s\n",BNto_string_inplace(ans));
                BNdestroy(a);
                BNdestroy(ans);


            }
            while(choose()!='q');

        }
        break;
        case 33:
        {
            do
            {
                pBigNumber a,b,ans;
                a=get_bn_from_console();
                b=get_bn_from_console();
                ans=BNnew_default();
                c=BNgcd(a,b,ans);
                if (c != BN_OK)
                {
                    printf_err(c);
                    break;
                }
                printf( "%s\n",BNto_string_inplace(ans));
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);


            }
            while(choose()!='q');

        }
        break;
        case 34:
        {
            do
            {

                BNu64 pow;
                pBigNumber a,b,ans;
                a=get_bn_from_console();
                printf("Get power:\n");
                scanf(BNULONGLONGPATTERN,&pow);
                b=get_bn_from_console();
                ans=BNnew_default();
//BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans)
                c=BNmodular_pow(a,pow,b,ans);
                if (c != BN_OK)
                {
                    printf_err(c);
                    break;
                }
                printf( "%s\n",BNto_string_inplace(ans));
                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);


            }
            while(choose()!='q');

        }
        break;
        case 35:
        {
            do
            {


                pBigNumber a,ans;
                char s;
                printf("*******\n");
                printf("1)Get Number\n");
                printf("2)Load From File\n");
                printf("*******\n");
                s=getchar();
                while (getchar() != '\n')
                    ;

                if(s=='1')
                    a=get_bn_from_console();
                else
                {
                    pString  st=new_string(100);
                    get_string(st,"Get File Name:");
                    a=BNnew_from_file(st->buf,&c);
                    destroy_string(st);
                    if(c!=BN_OK)
                    {
                        printf_err(c);
                        continue;
                    }
#if BN_IS_DEBUG
                    printf_bn(a);
#endif // BN_IS_DEBUG

                }
                ans=BNnew_default();
//BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans)
                t1=clock();
                c=BNfactorize_rho(a,ans);
                t2=clock();
                if (c != BN_OK)
                {
                    printf_err(c);
                    break;
                }
                printf( "%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC );
                BNdestroy(a);

                BNdestroy(ans);
            }
            while(choose()!='q');

        }
        break;
        case 36:
        {
            do
            {


                pBigNumber a;
                pVectFact vf;

                a=get_bn_from_console();
                vf=BNnew_vect_fact();

//BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans)
                c=BNfactorize(a,vf);
                if (c != BN_OK)
                {
                    printf_err(c);
                    break;
                }
                printf_vf(vf);
                BNdestroy(a);
                BNdestroy_vect_fact(vf);


            }
            while(choose()!='q');

        }
        break;
        case 37:
        {
            do
            {


                pBigNumber a,b,ans;
                a=get_bn_from_console();
                b=get_bn_from_console();
                ans=BNnew_default();
//BNERR_NUMBER BNmodular_pow(pBigNumber a, BNu64 pow,pBigNumber mod, pBigNumber ans)
                c=BNlcm(a,b,ans);
                printf("%s\n",BNto_string_inplace(ans));
                if (c != BN_OK)
                {
                    printf_err(c);
                    break;
                }

                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);

            }
            while(choose()!='q');

        }
        break;
        }
        printf("q to quite any other key to continue\n");
    }
    while(getchar()!='q');

    return 1;
}
int bench_test()
{
    char val;
    clock_t t1,t2;
    do
    {
        while (getchar() != '\n')
            ;
        printf("*********\n");
        printf("1)Factorize\n");
        printf("2)Reverse And Sum\n");
        printf("3)Factorial\n");
        printf("4)Pow\n");
        printf("5)Multiplication\n");
        printf("6)Division\n");
        printf("7)Sqrt\n");
        printf("8)Factorial + Factorize\n");
        printf("9)(Factorial+1) + Factorize Rho\n");
        printf("q)quite\n");
        printf("*********\n");
        val=getchar();

        switch (val)
        {
        case '1':
            {
                pBigNumber n;
                pVectFact vf;
                BNERR_NUMBER c;
                do
                {
                    n=get_bn_from_console();
                    vf=BNnew_vect_fact();
                    t1=clock();
                    c=BNfactorize(n,vf);
                    t2=clock();
                    if(c!=BN_OK)
                    {
                        printf_err(c);

                    }

                    printf_vf(vf);
                    printf("t:%f\n",(float)(t2-t1)/CLOCKS_PER_SEC );
                    BNdestroy_vect_fact(vf);
                    BNdestroy(n);
                }while(choose()!='q');

            }

            break;
        case '2':
            reverse_and_sum();
            break;
        case '3':
            do
            {
                pBigNumber a,ans;
                BNERR_NUMBER c;

                a=get_bn_from_console();

                //a=BNnew_from_string(BIGNUMA,&c);
                //b=BNnew_from_string(BIGNUMB,&c);
                ans=BNnew_default();
                t1=clock();
                c=BNfactorial(a,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                    printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(ans);

            }
            while(choose() !='q');
            break;
        case '4':
            do
            {
                pBigNumber a,ans;

                BNERR_NUMBER c;
                BNu64 pow;

                a=get_bn_from_console();
                printf("Get pow:\n");
                scanf(BNULONGLONGPATTERN,&pow);
                ans=BNnew_default();
                t1=clock();
                c=BNpow(a, pow,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                    printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(ans);

            }
            while(choose() !='q');
            break;
        case '5':
            do
            {
                BNu64 len_a,len_b;
                pBigNumber a,b,ans;
                BNERR_NUMBER c;
                printf("Get rand length a:\n");
                scanf(BNULONGLONGPATTERN,&len_a);
                while (getchar() != '\n')
                    ;
                printf("Get rand length b:\n");
                scanf(BNULONGLONGPATTERN,&len_b);

                a=BNnew_rand(len_a,BNTRUE);
                if(!a)
                {
                    printf("Not enough memory\n");
                    break;
                }
                b=BNnew_rand(len_b,BNFALSE);
                if(!b)
                {
                    printf("Not enough memory\n");
                    BNdestroy(a);
                    break;
                }
                ans=BNnew_default();
                t1=clock();
                c=BNtimes(a, b,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                    printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );

                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);

            }
            while(choose() !='q');
            break;
        case '6':
            do
            {
                BNu64 len_a,len_b;
                pBigNumber a,b,ans,remainder;
                BNERR_NUMBER c;
                int cmp;
                printf("Get rand length a:\n");
                scanf(BNULONGLONGPATTERN,&len_a);
                while (getchar() != '\n')
                    ;
                printf("Get rand length b:\n");
                scanf(BNULONGLONGPATTERN,&len_b);

                a=BNnew_rand(len_a,BNTRUE);
                if(!a)
                {
                    printf("Not enough memory\n");
                    break;
                }
                b=BNnew_rand(len_b,BNTRUE);
                if(!b)
                {
                    printf("Not enough memory\n");
                    BNdestroy(a);
                    break;
                }
                ans=BNnew_default();
                remainder=BNnew_default();
                t1=clock();
                c=BNdivide(a, b,ans,remainder);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                {
                    printf("%s %f\n",BNto_string_inplace(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );
                    BNto_number_inplace(ans);
                }
                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );


                t1=clock();

                BNtimes_eq(ans,b);
                BNplus_eq(ans,remainder);
                cmp=BNcmp(a,ans);
                t2=clock();
                printf("%s %f\n",cmp==0?"OK":"Not Ok",(float)(t2-t1)/CLOCKS_PER_SEC  );



                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
                BNdestroy(remainder);

            }
            while(choose() !='q');
            break;
        case '7':
            do
            {
                BNu64 len_a;
                pBigNumber a,b,ans,remainder;
                BNERR_NUMBER c;
                int cmp;
                remainder=BNnew_default();
                printf("Get rand length a:\n");
                scanf(BNULONGLONGPATTERN,&len_a);
                a=BNnew_rand(len_a,BNTRUE);
                if(!a)
                {
                    printf("Not enough memory\n");
                    break;
                }

                ans=BNnew_default();
                b=BNnew_default();
                t1=clock();
                c=BNsqrt(a,ans);
                t2=clock();
                if(c!=BN_OK)
                {
                    printf_err(c);

                }
                if(BNlen(ans)<100)
                {
                    printf("%s %s %s %f\n",BNto_string_inplace(a),BNto_string_inplace(ans),BNto_string_inplace(remainder), (float)(t2-t1)/CLOCKS_PER_SEC  );
                    BNto_number_inplace(a);
                    BNto_number_inplace(ans);
                }

                else
                    printf("len:" BNULONGLONGPATTERN " %f\n",BNlen(ans),(float)(t2-t1)/CLOCKS_PER_SEC  );


                t1=clock();
                BNtimes(ans,ans,b);
                BNminus(a,b,ans);
                cmp=BNcmp(ans,a);
                t2=clock();
                printf("%s %f\n",cmp<0?"OK":"Not Ok",(float)(t2-t1)/CLOCKS_PER_SEC  );



                BNdestroy(a);
                BNdestroy(b);
                BNdestroy(ans);
                BNdestroy(remainder);


            }
            while(choose() !='q');
            break;
        case '8':
            do
            {

                pBigNumber a,ans;
                BNERR_NUMBER c;
                pVectFact vf;
                a=get_bn_from_console();
                ans=BNnew_default();
                vf=BNnew_vect_fact();
                t1=clock();
                c=BNfactorial(a,ans);
                t2=clock();
                if (c != BN_OK)
                    printf_err(c);

                printf("len:" BNULONGLONGPATTERN " t:%f\n",(BNu64) BNlen(ans), (float)(t2-t1)/CLOCKS_PER_SEC);
                t1=clock();
                    c=BNfactorize(ans,vf);
                t2=clock();
                if (c != BN_OK)
                    printf_err(c);
                 printf("t:%f\n",(float)(t2-t1)/CLOCKS_PER_SEC);
                 printf_vf(vf);

                BNdestroy(a);
                BNdestroy(ans);
                printf("Do you want save the factor? (y/n)\n");
                if(getchar()=='y')
                {

                    pString st=new_string(100);
                    get_string(st,"Get File name\n");
                    FILE*f;
                    size_t i;
                    char tmp[100];
                    f=fopen("fact.txt","wb");
                    for(i=0;i<vf->pos;i++)
                    {
                        fputs( BNto_string_inplace( vf->p[i].bn),f );
                        fputs(" ^ ",f);
                        sprintf(tmp,"%d\n",vf->p[i].cnt);
                        fputs(tmp,f);
                    }
                    fclose(f);
                    destroy_string(st);
                }
                BNdestroy_vect_fact(vf);
                while(getchar()!='\n')
                    ;

            }
            while(choose() !='q');
            break;
        case '9':
            do
            {

                pBigNumber a,ans,ans_rho;
                BNERR_NUMBER c;

                a=get_bn_from_console();
                ans=BNnew_default();
                ans_rho=BNnew_default();
                t1=clock();
                c=BNfactorial(a,ans);
                t2=clock();
                if (c != BN_OK)
                    printf_err(c);
                if(BNlen(ans)<120)
                   printf("%s t:%f\n",BNto_string_inplace(ans), (float)(t2-t1)/CLOCKS_PER_SEC);
                else
                    printf("len:" BNULONGLONGPATTERN " t:%f\n",(BNu64) BNlen(ans), (float)(t2-t1)/CLOCKS_PER_SEC);
                BNto_number_inplace(ans);
                BNplus_plus(ans);
                if(BNlen(ans)<120)
                   printf("%s t:%f\n",BNto_string_inplace(ans), (float)(t2-t1)/CLOCKS_PER_SEC);
                else
                    printf("len:" BNULONGLONGPATTERN " t:%f\n",(BNu64) BNlen(ans), (float)(t2-t1)/CLOCKS_PER_SEC);
                BNto_number_inplace(ans);
                t1=clock();
                    c=BNfactorize_rho(ans,ans_rho);
                t2=clock();
                if (c != BN_OK)
                    printf_err(c);
                 printf("%s t:%f\n",BNto_string_inplace(ans_rho),(float)(t2-t1)/CLOCKS_PER_SEC);


                BNdestroy(a);
                BNdestroy(ans);
                BNdestroy(ans_rho);


            }
            while(choose() !='q');
            break;

        }

    }
    while(val!='q');


    return 1;
}
int main()
{
    pBigNumber ndef,n;
    char *tmp;
    char val=1;
    do
    {
        printf("**********\n");
        printf("1)Test functions\n");
        printf("2)Benchmark\n");
        printf("q)quite\n");
        printf("**********\n");
        val=getchar();


        switch (val)
        {
        case '1':
            test_sum();
            break;
        case '2':
            bench_test();
            break;
        case 'q':
            break;
        default :
            printf("Wrong choose\n");
            break;
        }
        while (getchar() != '\n')
            ;
    }
    while(val!='q');


    ///test_comp();
//    printf("%s\n" ,BNto_string_inplace(max_ull_size));
//    BNto_number_inplace(max_ull_size);
//    system("pause");
    test_sum();
    ndef=BNnew_default();
    if(!ndef)
        printf("Not enough memory\n");
    BNdestroy(ndef);
    ndef=NULL;

    n=BNnew_set(1000,100);
    if(!n)
        printf("Not enough memory\n");
    BNdestroy(n);
    n=NULL;

    n=get_bn_from_console();

    tmp=BNto_string_inplace(n);
    printf("%s\n",tmp );
    BNto_number_inplace(n);
    tmp=NULL;

    tmp=BNto_string_dup(n);
    printf("%s\n",tmp);
    free(tmp);
    tmp=NULL;
    BNdestroy(n);

    n=get_bn_from_int();
    if(n)
    {
        printf("%s\n",BNto_string_inplace(n));
        BNto_number_inplace(n);
    }
    BNdestroy(n);
    n=NULL;
    system("pause");
    return 0;
}
