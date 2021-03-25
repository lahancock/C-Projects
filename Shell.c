//Lauren Hancock
//Shell Project 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  char line[1024];// get command line
  char* args[100];
  
  //Run command loop
  while (1){
    int redirect = -1;
    printf("~~~~~~~~> ");//print prompt
    fgets(line, 1024, stdin);//Get input
    
    if(line[0] != '\n'){//if nothing is entered, reprint prompt
      
      //Tokenize line, split command into token strings 
      char *token;
      token = strtok(line, " \n");
      int i=0;
      while(token){
        args[i]=malloc(100);
        //args[i] = token;
        strcpy(args[i], token);
        token = strtok(NULL," \n");
        if(strcmp(args[i], ">")==0){//Check for outout redirect
          redirect = i;
        }//redirect if end
        i++;   
      }//end token while
      args[i]=NULL;//place NULL at end of token array
            
      if(strcmp(args[0], "exit")==0){//Shall we exit?
        printf("Exiting...\n");
        break;
      }else if(strcmp(args[0], "pwd")==0){//Shall we print the current working directory?
        char cwd[FILENAME_MAX];
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("Current working dir: %s\n", cwd);
        }else{
          perror("getcwd() error");
        }
      }else if(strcmp(args[0], "cd") == 0){//Shall we change directories?
        if ( args[1] == NULL ){
          chdir(getenv("HOME"));
          char cwd[FILENAME_MAX];
          if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current working dir: %s\n", cwd);
            }else{
              perror("getcwd() error");
            }
        }else{
          chdir(args[1]);
          char cwd[FILENAME_MAX];
          if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current working dir: %s\n", cwd);
          }else{
            perror("getcwd() error");
          }
      }//end change directory else if
      //--------------Handle possible redirection --------------------------------------------------> 
      }else{
        if(redirect==-1){ //no redirection, use wait----------------------------------> NO Redirect
          int rc = fork();
          //rc from fork is 0 which means that this is the child process
          // Child executes another program (sleep) via execvp
          if (rc == 0) {
            // Call sleep to make the process nap for 10 seconds
            //char *arrayArgs[i];//size of old array
            execvp(args[0], args);
            perror("execvp");// We should never reach here
            return -1;
          }
          // rc isn't 0 which means this is the parent process.
          // Parent waits for child to finish
          else if (rc > 0) {
            // The basic wait system call passing NULL as argument suspends
            // the calling process until one of its children terminates
            int rc = wait(NULL);
            if (rc < 0) {
              perror("wait");
              return -1;
            }
          }else{
            perror("fork");
          }

        }else{//redirect found, use redirect code----------------------------------------> Redirect
          int rc = fork();
          // Child runs execvp
          if (rc == 0) {
            // Close stdout, freeing its filedescriptor (stdin has descriptor 0, stdout has 1, stderr has 2)
            close(1);

            // Open another file, which gets the lowest numbered file descriptor
            // available: the one just freed by closing stdout
            open(args[redirect+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

            // Now, any output that would go to stdout by default will go to
            // the file instead!
            args[redirect]=NULL;
            execvp(args[0], args);

            perror("execvp");// We should never reach here
            return -1;
          }else if(rc > 0){// Parent waits for child to finish
          // Basic wait system call suspends the calling process until one of its children terminates
            int rc = wait(NULL);
            if (rc < 0) {
                perror("wait");
                return -1;
            }
          }else{
            perror("fork");
          }
        }//End redirection block
      }//End possible redirection block 
    }//End if (nothing is entered) block
  }//End shell prompt while
  return 0;
}//End main