#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>

void setpath(char A[100][100],char path[]){
	if(strcmp(A[0],"sort")==0 || strcmp(A[0],"uniq")==0 || strcmp(A[0],"wc")==0 || strcmp(A[0],"man")==0 || strcmp(A[0],"du")==0){
			strcpy(path,"/usr/bin/");
			//strcpy(path,"/usr");
		}
		else{
			strcpy(path,"/bin/");
			//strcpy(path,"");
		}
}

void ex_simple(char A[100][100], char* In1){

	char* token = strtok(In1, " ");
	int i=0;
	while(token!=NULL){
		strcpy(A[i], token);
		token = strtok(NULL, " ");
		i++;
	}

	if(strcmp(A[0],"cd")==0){
		chdir(A[1]);
	}

	else{
		char path[100]="";

		//setpath(A,path);

		strcat(path,A[0]);

		char *args[i+1];
		args[0]=path;
		for(int k=1;k<i;k++){
			args[k]=A[k];
		}
		args[i]=NULL;

 	   	pid_t pid = fork();  
 	 
   		if (pid == -1) { 
   	    	printf("Failed forking child\n"); 
   	    	return; 
    	} else if (pid == 0) { 
       		if(execvp(args[0],args)<0){
       			printf("Could not execute command\n");
       			exit(0);
       		}
    	} else { 
        	wait(NULL);  
    	}
    }
}

static void redirect(int oldfd, int newfd) {
  if (oldfd != newfd) {
  	dup2(oldfd, newfd);
  	close(oldfd);
  }
}

void exec_pipeline(char* const* cmds[], size_t pos, int in_fd) {
  if (cmds[pos + 1] == NULL) {
    	redirect(in_fd, 0); 
    	execvp(cmds[pos][0], cmds[pos]);
    	printf("Error at last command\n");
    	exit(0);
  }
  else { 
    int fd[2]; 
    if (pipe(fd) == -1)
      printf("pipe error\n");

  int id = fork();
    if(id<0) {
      printf("fork error\n");
  	}
    else if(id==0){ 
      //child = 1;
      close(fd[0]);
      redirect(in_fd, 0); 
      redirect(fd[1], 1); 
      //printf("yoooo\n");
      execvp(cmds[pos][0], cmds[pos]);
      printf("unable to execute command\n");
      exit(0);
  	}
    else {
      close(fd[1]); 
      close(in_fd); 
      exec_pipeline(cmds, pos + 1, fd[0]); 
    }
	}
}

void main(){
	char pwd[1024]; 
	char In[100]= "";
	char A[100][100];
	memset(A,0,sizeof(A));
	while(1){
		memset(In,0,sizeof(In));
		getcwd(pwd, 1024); 
		printf("%s$ ", pwd);
		fgets(In, 100, stdin);
		if(strcmp(In,"\n")==0) continue;
		char* In1 = strtok(In, "\n");

		if(strcmp(In1,"exit")==0 || strcmp(In1,"exit\n")==0){
			break;
		}

		if(strchr(In1,'|')!=NULL){

			char* p = strtok(In1,"|");
			char pipe[50][50];
			int n=0;
			while(p!=NULL){
				strcpy(pipe[n],p);
				p=strtok(NULL,"|");
				n++;
			}
			char *pipecmd[n];
			for(int k=0;k<n;k++){
				pipecmd[k]=pipe[k];
			}

			char* const* pargs[n+1];
			char* ar[n+1][50];
			for(int j=0;j<n;j++){
				if(strstr(pipecmd[j],">>")==NULL && strchr(pipecmd[j],'>')==NULL && strchr(pipecmd[j],'<')==NULL){
					char* t = strtok(pipecmd[j], " ");
					int a=0;
					while(t!=NULL){
						ar[j][a] = t;
						t = strtok(NULL, " ");
						a++;
					}
					ar[j][a]=NULL;
				}
				else{
					printf("redirection in pipe\n");
					ar[j][0]=pipecmd[j];
				}
				pargs[j]=ar[j];
			}
			pargs[n]=NULL;
			int PID=fork();
			if(PID==0)
				exec_pipeline(pargs,0,0);
			else
				wait(NULL);
		}

		else if(strstr(In1,">>")!=NULL){
			//printf("appending\n");
			char* outcmd = strtok(In1,">>");
			char* outfile = strtok(NULL,">>");
			outfile = strtok(outfile," ");

			char* ar[100];
			char* t = strtok(outcmd," ");
			int a=0;
			while(t!=NULL){
				ar[a]=t;
				t = strtok(NULL," ");
				a++;
			}

			int pid = fork();
			if(pid<0) printf("Fork error\n");
			else if(pid==0){
				close(1);
				open(outfile,O_CREAT | O_APPEND | O_WRONLY, 0777);
							
				execvp(ar[0],ar);
				printf("Unable to execute command\n");
				exit(0);
			}
			else{
				wait(NULL);
			}
		}

		else if(strchr(In1,'>')!=NULL){
			char* outcmd = strtok(In1,">");
			char* outfile = strtok(NULL,">");
			outfile = strtok(outfile," ");
			char* rest = strtok(NULL,">");
			char* f = strtok(NULL,">");
			
			int n=0;
			if(rest){
				if(strcmp(rest,"1")==0) n=1;
				else if(strcmp(rest,"2")==0) n=2;

			}

			int numflag[2]={0,0};
			int andflag[2]={0,0};
			char* ar[100];
			char* t = strtok(outcmd," ");
			int a=0;
			while(t!=NULL){
				ar[a]=t;
				t = strtok(NULL," ");
				a++;
			}
		
			if(strcmp(ar[a-1],"1")==0){
				ar[a-1]=NULL;
				numflag[0]=1;
				numflag[1]=1;
			}
			else if(strcmp(ar[a-1],"2")==0){
				ar[a-1]=NULL;
				numflag[0]=1;
				numflag[1]=2;
			}
			if(strchr(outfile,'&')!=NULL){
				andflag[0]=1;
				if(strcmp(outfile,"&1")==0){
					andflag[1]=1;
				}
				else if(strcmp(outfile,"&2")==0){
					andflag[1]=2;
				}
			}
			int pid = fork();
			if(pid<0) printf("Fork error\n");
			else if(pid==0){
				if(numflag[0]==0 && andflag[0]==0){
					close(1);
					creat(outfile,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				}
				else if(numflag[0]==1){
					if(andflag[0]==0){
						close(numflag[1]);
						creat(outfile,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					}
					else{
						close(numflag[1]);
						dup(andflag[1]);
					}
				}
				execvp(ar[0],ar);
				printf("Unable to execute command\n");
				exit(0);
			}
			else{
				wait(NULL);
			}
			memset(ar,0,sizeof(ar));
		}

		else if(strchr(In1,'<')!=NULL){
			char* incmd = strtok(In1,"<");
			char* infile = strtok(NULL,"<");
			infile = strtok(infile," ");

			char* ar[100];
			char* t = strtok(incmd," ");
			int a=0;
			while(t!=NULL){
				ar[a]=t;
				t = strtok(NULL," ");
				a++;
			}
			int pid = fork();
			if(pid<0) printf("Fork error\n");
			else if(pid==0){
				close(0);
				open(infile,O_RDONLY);
				execvp(ar[0],ar);
				printf("Unable to execute command\n");
				exit(0);
			}
			else{
				wait(NULL);
			}
			memset(ar,0,sizeof(ar));
		}
		
		else ex_simple(A,In1);

    	memset(A,0,sizeof(A));
	}
}
