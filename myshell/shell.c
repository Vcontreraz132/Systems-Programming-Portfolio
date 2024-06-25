#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define DELIMITERS " \t\r\n\a"

void loop(void);
char *read_command(void);
char **split_command(char *line);
int execute_command(char **args);

int main(int argc, char **args) {

	loop();

	return EXIT_SUCCESS;
}

void loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = read_command();
		args = split_command(line);
		status = execute_command(args);

		free(line);
		free(args);
	} while(status);
}

char *read_command(void) {
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
}

char **split_command(char *line) {
	int bufsize = MAX_COMMAND_LENGTH, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if(!tokens) {
		fprintf(stderr, "myshell: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, DELIMITERS);

	while(token != NULL) {
		tokens[position] = token;
		position++;

		if(position >= bufsize) {
			bufsize += MAX_COMMAND_LENGTH;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if(!tokens) {
				fprintf(stderr, "myshell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, DELIMITERS);
	}
	tokens[position] = NULL;
	return tokens;
}

int execute_command(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if(pid == 0) {
		if(execvp(args[0], args) == -1) {
			perror("myshell: execution error\n");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0) {
		perror("myshell: process error\n");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}
