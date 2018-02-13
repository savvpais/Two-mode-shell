all: shell

shell:shell.c
	gcc shell.c -o shell 

clear:
	rm shell

