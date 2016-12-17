#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"make_log.h"
#include<unistd.h>
#include<sys/wait.h>

int main(int argc,char *argv[])
{
	pid_t pid = 0;
	int fd[2];
	char buf[256] = {0};
	int ret = pipe(fd);
	if(argc < 2)
	{
		printf("a.out param\n");
		return;
	}

	if(-10 == ret)
	{
		perror("pipe");
		exit(1);
	}
	//重定向输出
	dup2(fd[1],STDOUT_FILENO);
	pid = fork();
	if(pid < 0)
	{
		perror("fork");
		exit(1);
	}
	
	if(0 == pid)
	{
		close(fd[0]);
		execlp("fdfs_upload_file","fdfs_upload_file","/etc/fdfs/client.conf" ,argv[1],NULL);
		perror("execlp");
		exit(1);
	}

	if(pid > 0)
	{
		close(fd[1]);
		read(fd[0],buf,256);
		LOG("upload","upload",buf);
		wait(NULL);
	}

	return 0;
}
