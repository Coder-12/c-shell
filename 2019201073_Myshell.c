#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <algorithm>
#include <readline/history.h>
#define cmd_size 10000
#define cmd_cardinality 100
int tokenslen=0,tokenlistlen=0;
int builtin_cmd_processor(char **parsedstr);
void seperator(char *str,char **parsedstr){
	int i=0;
	char *token=strtok(str,"=");
	//printf("%s",token);
	while(token){
		parsedstr[i]=token;
		token=strtok(NULL,"=");
		i++;
	}
	parsedstr[i]=NULL;
}
void initialize(){
	FILE *fp;
	fp=fopen(".myrc","r");
	char *line=NULL,*parsedstr[2];
	size_t len=0;
	ssize_t r;
	//unordered_map<char*,char*> hash;
	if(!fp){
		printf("Cannot open file\n");
		_exit(EXIT_FAILURE);
	}
	while((r=getline(&line,&len,fp))!=-1){
		//parsed[0]=(char*)malloc(MAXCOM*sizeof(char));
		seperator(line,parsedstr);
	//	printf("%s",line);
		//printf("%s\n%s\n",parsed[0],parsed[1]);
		//hash[parsed[0]]=parsed[1];
	}
	fclose(fp);
	if(line) free(line);
	char *user=getenv("USER");
	printf("\nUSER is: @%s",user);
	sleep(1);
	printf("\033[H\033[J");
}
void p_Dir(){
	char cwd[1024];
        getcwd(cwd,sizeof(cwd));
	printf("\nDir: %s",cwd);
}
int read_input(char *str){
	char *buf;
	buf=readline("\n>>> ");
	if(strlen(buf)!=0){
		add_history(buf);
		strcpy(str,buf);
		return 0;
	}
	else return 1;
}
void bg(char **parsed){
    pid_t pid=fork();
    if(pid==-1){
        printf("\nFailed creating child!!!");
        return;
    }
    else if(pid==0){
        if(execvp(parsed[0],parsed)<0)
            printf("\ncouldn't execute command");
        exit(0);
    }
    else{
        printf("pid : %d\n",pid);
        return;
    }
    //return (int)pid;
}
void fg(char **parsed){
    pid_t pid=fork();
    int status;
    if(pid==-1){
        printf("\nFailed creating child!!!");
        return;
    }
    else if(pid==0){
        if(execvp(parsed[0],parsed)<0)
            printf("\ncouldn't execute command");
        exit(0);
    }
    else{
	  wait(NULL);
	  return;
        //while(wait(&status)!=pid);
    }
}

void exec_pipe(char ***cmd){
	char input[1024],output[1024];
        pid_t p;
        p=fork();
        int i=0,status,in=0,out=0,j=0;
        if(!p){
                for(;(cmd)[i]!=NULL;i++){
			for(;(cmd)[i][j]!=NULL;j++)
                           if(strcmp((cmd)[i][j],"<")==0){
                              (cmd)[i][j]=NULL;
                              strcpy(input,(cmd)[i][j+1]);
                              in=2;
                           }
                           else if(strcmp((cmd)[i][j],">")==0){
                              (cmd)[i][j]=NULL;
                              strcpy(output,(cmd)[i][j+1]);
                              out=2;
                           }
                } 
              if(in){
                 int fd0;
                 printf("%s\n",input);
                 if((fd0=open(input,O_RDONLY,0644))<0){
                     perror("\nCouldn't open the file");
                     exit(1);
                 }
                 dup2(fd0,0);
                 close(fd0);
	      }
	      if(out){
                int fd1;
                printf("%s\n",output);
                if((fd1=open(output,O_CREAT | O_WRONLY | O_TRUNC,0644))<0){
                            perror("\nCouldn't open the file");
                            exit(1);
                }
                dup2(fd1,1);
                close(fd1);
             }
              int pipefd[2];
              pid_t p1;
              int fdd=0;
	      while(*cmd!=NULL){
		     //printf("%s\n",(*cmd)[0]);
		     pipe(pipefd);
                     if(pipe(pipefd)<0){
		        perror("\npipe couldn't started");
		        _exit(EXIT_FAILURE);
	              }
	              p1=fork();
	              if(p1<0){
		         perror("\nNo process created!!!");
		         exit(1);
	               }
	              if(!p1){
			 dup2(fdd,0);
			 if(*(cmd+1)!=NULL)
				dup2(pipefd[1],1);
			 close(pipefd[0]);
			 execvp((*cmd)[0],*cmd);
			 printf("\ncouldn't execute command 1!!!");
			 _exit(EXIT_FAILURE);
	             }
		     else{
			wait(NULL);
			close(pipefd[1]);
			fdd=pipefd[0];
			cmd++;
		       // return;
		     }
	      }
	}
	else if(p<0){
                perror("\nCouldn't create child");
                _exit(EXIT_FAILURE);
        }
        else{
		while(!(wait(&status)==p));
		return;
	}
}
void openHelp(){
	puts("\nwelcome to my shell""\nCommands supported are listed below:"
	     "\n>cd""\n>ls""\n>exit""\n>all other general commands availble in unix shell""\n>pipe handling");
	return;
}

int builtin_cmd_processor(char **parsedstr){
	int nocmds=5,ch=0,j=0;
	char *cmdslist[]={(char*)("exit"),(char*)("cd"),(char*)("help"),(char*)("hello"),(char*)("echo")};
	char *username;
	char* env[] = {(char*)"USER", (char*)"HOME", (char*)"PATH",(char*) "SHELL", (char*)"OSTYPE", (char*)"PWD", (char*)"GROUP"};
	for(int i=0;i<nocmds;i++){
		if(strcmp(parsedstr[0],cmdslist[i])==0){
			ch=i+1;
			break;
		}
	}
	switch(ch){
		case 1:
			printf("\nGoodbye\n");
			exit(0);
		case 2:
			chdir(parsedstr[1]);
			return 1;
		case 3:
			openHelp();
			return 1;
		case 4:
			username=getenv(env[0]);
			printf("\nHello %s",username);
			return 1;
		case 5:
			char arg[100];
			for(int i=1;parsedstr[1][i]!='\0';i++)
				arg[j++]=parsedstr[1][i];
			arg[j]='\0';
			for(int i=0;i<7;i++){
				if(strcmp(env[i],arg)==0){
				   char *myenv=getenv(arg);
				   printf("%s\n",myenv);
				}
			}
			return 1;
		default:
			break;
	}
	return 0;
}
void redirection(char **tokens){
        char input[1024],output[1024];
        pid_t p;
        p=fork();
        int i=0,status,in=0,out=0;
        if(!p){
                for(;tokens[i]!=NULL;i++){
                if(strcmp(tokens[i],"<")==0){
                        tokens[i]=NULL;
                        strcpy(input,tokens[i+1]);
                        in=2;
                }
                else if(strcmp(tokens[i],">")==0){
                        tokens[i]=NULL;
                        strcpy(output,tokens[i+1]);
                        out=2;
                }
              }
              if(in){
                 int fd0;
		 printf("%s\n",input);
                 if((fd0=open(input,O_RDONLY,0644))<0){
                     perror("\nCouldn't open the file");
                     exit(1);
                 }
                 dup2(fd0,0);
                 close(fd0);
              }
              if(out){
                int fd1;
                printf("%s\n",output);
                if((fd1=open(output,O_CREAT | O_WRONLY | O_TRUNC,0644))<0){
                            perror("\nCouldn't open the file");
                            exit(1);
                }
                dup2(fd1,1);
                close(fd1);
               }
               execvp(tokens[0],tokens);
               perror("Error");
               _exit(EXIT_FAILURE);
        }
        else if(p<0){
                perror("\nCouldn't create child");
                exit(1);
        }
        else{
                while(!(wait(&status)==p));
        }
}

void tokenize(char *str,char **parsedstr){
	int i=0;
	char *token=strtok(str," ");
	while(token){
		parsedstr[i]=token;
		token=strtok(0," ");
		i++;
	}
	parsedstr[i]=NULL;
	tokenslen=i;
}
int sep_pipe(char *str,char **pipedstr){
	int i=0,len=0;
	char *temp=str;
	char *token=strtok(temp,"|");
	while(token){
		pipedstr[i]=token;
		len++;
	        token=strtok(0,"|");
		i++;
	}
	pipedstr[i]=NULL;
	return len;
}
int proc_string(char *str,char ***cmd){
	char *pipedstr[100];
	int e_flag=0,len;
	len=sep_pipe(str,pipedstr);
	tokenlistlen=len; 
	e_flag=(pipedstr[1]!=NULL);
	char **parsedstr[cmd_cardinality];
	if(e_flag){
		for(int i=0;i<len;i++){
			parsedstr[i]=(char**)malloc(MAXCOM*sizeof(char**));
                        tokenize(pipedstr[i],parsedstr[i]);
		        cmd[i]=parsedstr[i];
		}
	}
	else{
		parsedstr[0]=(char**)malloc(MAXCOM*sizeof(char**));
		tokenize(str,parsedstr[0]);
		cmd[0]=parsedstr[0];
		int i=0;
		for(;parsedstr[0][i]!=NULL;i++){
			if(strcmp(parsedstr[0][i],"<")==0 or
			   strcmp(parsedstr[0][i],">")==0){
				e_flag=2;
				break;
			}
		}
				
	}
	cmd[len]=NULL;
	return e_flag;
}
void execute_cmd(char ***tokenlist,int val){
	bool flag=false;
	int i;
	if(tokenlist[tokenlistlen-1][tokenslen-1][0]=='&'){
		flag=true;
		tokenlist[tokenlistlen-1][tokenslen-1]=NULL;
	}
	if(!val) i=builtin_cmd_processor(*tokenlist); 
	if(!flag){
		if(!i) fg(*tokenlist);
		else if(val==1)
			exec_pipe(tokenlist);
		else if(val==2)
			redirection(*tokenlist);
	}
	else{
		if(!i) bg(*tokenlist);
	}
}

int main(){
  char command[cmd_size],**cmd[cmd_cardinality];
  int e_flag=0;
  initialize();
  while(1){
	  p_Dir();
	  if(read_input(command)) continue;
	   e_flag=proc_string(command,cmd);
	   execute_cmd(cmd,e_flag);
  }
  return 0; 
}
