/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"


static void PrintEnv(char *label, char **envp)
{
	printf("%s:<br>\n<pre>\n", label);
	for ( ; *envp != NULL; envp++) {
		printf("%s\n", *envp);
	}
	printf("</pre><p>\n");
}

char* memstr(char* full_data, int full_data_len, char* substr) 
{ 
    if (full_data == NULL || full_data_len <= 0 || substr == NULL) { 
        return NULL; 
    } 

    if (*substr == '\0') { 
        return NULL; 
    } 

    int sublen = strlen(substr); 

    int i; 
    char* cur = full_data; 
    int last_possible = full_data_len - sublen + 1; 
    for (i = 0; i < last_possible; i++) { 
        if (*cur == *substr) { 
            //assert(full_data_len - i >= sublen);  
            if (memcmp(cur, substr, sublen) == 0) { 
                //found  
                return cur; 
            } 
        } 
        cur++; 
    } 

    return NULL; 
} 


int main ()
{
	char **initialEnv = environ;//当前的环境变量 备份了到initialEnv
	int count = 0;

	while (FCGI_Accept() >= 0) {
		//数据---post数据 http://192.168.23.250:/demo?cmd=123&name=zhang3  [post数据]
		char *contentLength = getenv("CONTENT_LENGTH");
		//表示post数据的长度
		int len;

		printf("Content-type: text/html\r\n"
					"\r\n"
					"<title>FastCGI echo</title>"
					"<h1>FastCGI echo</h1>\n"
					"Request number %d,  Process ID: %d<p>\n", ++count, getpid());

		if (contentLength != NULL) {
			len = strtol(contentLength, NULL, 10);
		}
		else {
			len = 0;
		}

		if (len <= 0) {
			printf("No data from standard input.<p>\n");
		}
		else {
			//回显示 用户的post数据
			char *buf = malloc(len);
			char *p = buf;
			int	start,end;
			char name[80] = {0};
			char buftmp[80] = {0};
			char* tmp = NULL;
			char *head = NULL;
			int i, ch;
			int fstart,fend;
			//char* folder = "./resource";       

			printf("Standard input:<br>\n<pre>\n");
			for (i = 0; i < len; i++) 
			{
				//就相当于从浏览器客户端cfd, 读一个字节到程序中
				if ((ch = getchar()) < 0) {
					printf("Error: Not enough bytes received on standard input<p>\n");
					break;
				}
				putchar(ch);
				*p =ch; 
				p++;
			}           

			tmp = strstr(buf,"filename");
			tmp = strstr(tmp+strlen("filename"),"\"");
			fstart = tmp-buf+1;
			char* tmp1 = strstr(tmp+strlen("\""),"\"");


			//将文件名赋给filename
			strncpy(name,buf+fstart,tmp1-tmp-1);
			printf("len:%d",tmp-buf);

			//连续找到两个/r/n
			for(i = 0; i < len; i++)
			{
				if(buf[i] == '\r' && buf[i+2] == '\r')
				{
					start = i+4;	//内容的开始
					printf("i:%d\n",i);
					break;
				}	
			}
			char* fst = strstr(buf,"\r"); //第一行内容
			//存放第一行内容
			strncpy(buftmp,buf,fst-buf);

			//找到文件的结尾
			/*
			head = strstr(buf,buftmp);   
			head = strstr(head+strlen(buftmp),buftmp);
			*/
			
			head = memstr(buf, len, buftmp);
			head = memstr(buf + strlen(buftmp),len-strlen(buftmp),buftmp);
			
			//比较两个头的地址就可以找到终止的地方
			end = head - buf;
			//将数据写入到文件中
			//将字符串拼接后输出到文件夹中
			//strcat(name,folder);
			int fd = open(name,O_CREAT | O_RDWR |O_TRUNC,0664);
			if(fd == -1)
			{
				printf("open error!\n");		
			}

			write(fd,buf+start,end-start);
			printf("start:%d,end:%d\n",start,end);
			printf("name:%s\n",name);
			printf("Standard input end:<br>\n<pre>\n");
			printf("\n</pre><p>\n");
			free(buf);
			close(fd);
		}

		PrintEnv("Request environment", environ);//environ 应该是当前cgi程序的环境变量
		PrintEnv("Initial environment", initialEnv);//initialEnv应该是 之前程序的默认环境变量
	} /* while */

	return 0;
}
