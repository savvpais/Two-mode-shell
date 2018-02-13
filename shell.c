#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


void interactive(void);
void batch(char *args);
char *readLine(void);
char **splitLine(char *line, int *words);
int shellLaunch(char **args);
int shellExecute(char **args, int words);
int shellCd(char **args);
int shellQuit(char **args);
int shellBuiltins(void);

//builtin instruction names
char *builtin_str[] = {
	"cd","quit"
};

//builtin instruction function call pointers
int (*builtin_func[]) (char**) = {
	&shellCd,
	&shellQuit
};

int main(int argc, char **argv){
	if(argc == 1){
		interactive();
	}
	else if (argc == 2){
		batch(argv[1]);
	}
	else{
		printf("Wrong usage of shell");
	}
	return 0;
}

//interactive shell mode
void interactive(void){
	char *line;
	char **args;
	int status;
	int words;
	
	do{
		printf("paisiadis_8723>");
		words = 0;
		line = readLine();
		args = splitLine(line, &words);
		status = shellExecute(args, words);
		
		free(line);
		free(args);
	}while(status);
}

//batch shell mode
void batch(char *args){
	FILE *file = fopen( args, "r");
	
	if (file == NULL){
		printf("Coudln't open %s", args);
		exit(EXIT_FAILURE);
	}
	
	char line[512];
	char **args2;
	int words;
	int status;
	
	//read .txt file line by line
	while(fgets( line, 512, file) != NULL && status){
		words = 0;
		args2 = splitLine(line, &words);
		status = shellExecute(args2, words);
		
		free(args2);
	}
}

//read instruction from user
char *readLine(void){
	char *line = NULL;
	ssize_t bufsize = 512;
	getline(&line, &bufsize, stdin);
	return line;
}

//split the instruction into words
char **splitLine(char *line,int *words){
	int bufsize = 64, position = 0;
	char **tokens = malloc(sizeof(char*) * bufsize);
	char *token;
	
	if (!tokens) {
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	//split string into tokens based the delimeters
	token = strtok(line, " \t\r\n\a");
	while (token != NULL){
		tokens[position] = token;
		position++;
		
		if (position >= bufsize) {
			bufsize += bufsize;
			tokens = realloc(tokens, sizeof(char*) * bufsize);
			if (!tokens){
				fprintf(stderr, "Memory allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		
		token = strtok(NULL, " \t\r\n\a");
	}
	*words=position;
	tokens[position] = NULL;
	return tokens;
}

//instruction execution
int shellLaunch(char **args){
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	//give the instruction execution to the child so the program continues
	if (pid == 0) {
		if(execvp(args[0], args) == -1){
			perror("Error executing the command\n");
			return 2;
		}
	}
	else if (pid < 0) {
		perror("Error forking\n");
	}
	else{
		//wait for the child to finish the command execution
		do{
			wpid = waitpid(pid , &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}


//preparation for instruction execution
int shellExecute(char **args, int words){
	int i,j,k,counter=0;
	char *delim[2]={";","&&"};
	char ***command = malloc(sizeof(char **) * words);
	
	for(i = 0; i < words; i++){
		command[i]=malloc(sizeof(char *) * words);
		for(k = 0; k < words; k++){
			command[i][j]=malloc(sizeof(char) * 512);
		}
	}
	
	int *mode = malloc(sizeof(int) * words);

	//return if command is empty
	if(args[0] == NULL){
		return 1;
	}
	
	//split further based on the ; and && characters
	int last = 0;
	
	for(i = 0; i < words; i++){
		
		if(strcmp(args[i], delim[0]) == 0){
			mode[counter] = 1;
			k = 0;
			for(j = last; j < i; j++){
				command[counter][k] = args[j];
				k++;
			}
			counter++;
			last = i+1;
		}
		else if (strcmp(args[i], delim[1]) == 0){
			mode[counter] = 2;
			k = 0;
			for(j = last; j < i; j++){
				command[counter][k] = args[j];
				k++;
			}
			counter++;
			last = i+1;
		}
	}
	
	i = last;
	k=0;
	while (args[i] != NULL){
		command[counter][k]=args[i];
		i++;
		k++;
	}
	mode[counter] = 1;
	counter++;

	int check;
	//if counter is 1 then there's only one instruction
	if (counter == 1){
		for (i = 0; i < shellBuiltins(); i++){
			if(strcmp(args[0], builtin_str[i]) == 0){
				return (*builtin_func[i])(args);
			}
		}
		return shellLaunch(args);
	}
	else{
		check = 1;
		for(i = 0; i < counter; i++){
			if (check == 0){
				return check;
			}
			else if(mode[i-1] == 2 && check == 2){
				if(mode[i] == 1){
					check = 1;
				}
				continue;
			}
			else{
				int flag = 1;
				for (j = 0; j < shellBuiltins(); j++){
					if(strcmp(command[i][0], builtin_str[j]) == 0){
						check = (*builtin_func[j])(command[i]);
						flag = 0;
					}
				}
				if (flag == 1){
					check = shellLaunch(command[i]);
				}
				else{
					flag = 1;
				}
			}
		}
	}
	return check;
}

//number of builtins
int shellBuiltins(void){
	return sizeof(builtin_str) / sizeof (char *);
}

//builtin cd instruction
int shellCd(char **args){
	if (args[1] == NULL) {
		fprintf(stderr, "expected path argument to \"cd\"\n");
	}
	else{
		if (chdir(args[1]) != 0){
			perror("Error changing directory");
		}
	}
	return 1;
}

//built in quit instruction
int shellQuit(char **args){
	return 0;
}
