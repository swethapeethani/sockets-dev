#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"resp.h"

int readinteger(char strarr[],int* index)
{
    int num =0;
    int i=0;
    //printf("val at strarr[i] is %c and index is %d \n",strarr[*index], *index);
    while(strarr[*index] != '\r')
    {
        //printf("strarr[i] is %c  %d num is %d \t",strarr[i],(strarr[i] - '0'),num);
        num = (num*10) + (strarr[*index] - '0');
        (*index)++; 
    }
    *index = *index +2;
    //printf("READ INT: number is %d and index is %d\n",num,*index);
    return num;

}

char * readsimstr(char simstr[],int *index)
{
    printf("READ SIMPLE STRING\n");

    int i=1;
    int strsize = 0;
    //(*index)++;
    while(simstr[(*index)+i] != '\r') 
    {
        strsize++;
        i++;
    }
    //*index = *index + i+1;
    
    char *str = (char *)malloc(strsize +1);
    bzero(str,strsize +1);
    int j;
    for(j =0; j<=strsize ;j++ )
    {
        str[j] = simstr[*index];
        (*index)++;
    }
    str[j] = '\0';
    *index = *index+2;
    return str;
}

char * readerror(char strarr[],int *index)
{
    printf("READ ERROR \n");

    int i=1;
    int strsize = 0;
    while(strarr[(*index)+i] != '\r') 
    {
        strsize++;
        i++;
    }
    char *errstr = (char *)malloc(strsize +1);
    bzero(errstr,strsize +1);
    int j;
    for(j =0; j<=strsize ;j++ )
    {
        errstr[j] = strarr[*index];
        (*index)++;
    }
    errstr[j] = '\0';
    *index = *index+2;
    return errstr;

}


char * parsebulkstr(char strarr[],int *index)
{
    int len=0;
    len = readinteger(strarr,index);
    char *buff = NULL;
    if(len == -1)
    {
        printf("Its a NULL BULK string");
        return buff;

    }
    else
    {
        buff = (char *)malloc(len+1);
        bzero(buff,len+1);
        int i=0;
        int j;
        for(j=0;  j<len && strarr[*index]!='\r' ; j++)
        {
            buff[j] = strarr[*index];
            (*index)++;   
        }
        buff[j] = '\0';
        *index = *index +2;
        //printf("input is a bulk string with value %s  and size is %d and index is %d\n", buff, len,*index);
    }
    return buff;
}

void parsearray(char strarr[],int *index)
{
    printf("PARSE ARRAY\n");
     int elecount =0;
    //printf("index is %d and val is %c\n",*index,strarr[*index]);
    elecount = readinteger(strarr,index);
    int j=1;
    
    while(j<=elecount)
    {
        switch(strarr[*index])
        {
            case ':':
            int num =0;
            (*index)++;
            //printf("ind is %d val is %c\n",*index,strarr[*index]);
            num = readinteger(strarr,index);
            printf("Parsed number is %d\n",num);
            break;
       
            case '$' :
                (*index)++;
                printf(" its an array of strings\n");
                if(atoi(&strarr[*index]) == -1)
                {
                    printf("Its a NULL BULK string\n");
                }
                else
                {
                    printf(" calling Bulk string parse\n");
                    char *bulkstr = parsebulkstr(strarr,index);
                    if(bulkstr != NULL)
                    {
                        printf("bulk string is %s\n",bulkstr);
                    }
                    else
                        printf("Its a NULL string\n");
                }
            break;

            case '+' :
            char *str;
            (*index)++;
            str = readsimstr(strarr,index);
            printf("The simple string is %s and index is %d\n", str,*index);
            break;
        
        case '-' :
            char *errstr;
            (*index)++;
            errstr = readerror(strarr,index);
            printf("The error string is %s and index is %d\n", errstr,*index);
        break;

        case '*':
        
            (*index)++;
            //printf("index is %d and val is %c\n",*index,strarr[*index]);
            parsearray(strarr,index);
        break;

        }
        j++;
    }
    printf("LAST index is %d\n",*index);
}

void decoderequest(struct resprequest *req,int setindex)
{
    char *strarr = req->buff;
    static int index = 0;
    if(setindex)
    {
        index =0;
    }
    index++;
    switch(*strarr)
    {
        case '$':
        if(atoi(&strarr[index]) == -1)
        {
            printf("Its a NULL BULK string\n");
        }
        else
        {
            char *bulkstr = parsebulkstr(strarr,&index); 
            printf("bulk string is %s and index is %d\n",bulkstr, index);
        }
        break;

        case '+':
        char *str;
        str = readsimstr(strarr,&index);
        printf("The simple string is %s\n", str);
        break;

        case '-' :
        char *errstr;
        errstr = readerror(strarr,&index);
        printf("The error string is %s and index is %d\n", errstr,index);
        break;

        case ':':
        int parsednum = 0;
        parsednum = readinteger(strarr,&index);
        printf("the parsed number is %d  and index is %d\n", parsednum,index); 
        break;

        case '*':
        printf("ind is %d\n",index);
        parsearray(strarr,&index);
        break;

    }


}