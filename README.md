# Two-mode-shell
* int main(int argc, char \*\* argv): Checks argc and enters the corresponding mode.

* void interactive(void): Calls all the necessary functions for the command execution and prints the prompt when it's done.

* void batch (char \* args): Gets the .txt file as input and reads the file line by line until the end of the file is reached or quit() is called from within the file.

* char *readLine(void): Reads the instruction given from the user.

* char \*\*splitLine(char \*line, int \*words): Splits the string line into tokens based on the delimeters given to strtok() until the end of the string is reached and puts NULL as the final word, while counting the number of words in the string.

* int shellLaunch(char \*\*args): Forks and executes the command, which is given as input, if the process is a child and waits for the child to finish if the process is a parent.

* int shellExecute(char \*\*args, int words): Exits if the first argument of the input is NULL, in which occastion we have an empty instruction. Then, compares each word from the input with the given delimeters ';' and '&&' to check whether multiple instructions were given. 'Counter' counts the number of instructions and 'last' keeps the last word which was a delimeter. 'Mode' is used to determine whether the following instruction is going to be skipped or not whether on the previous instruction's successful execution. Finally, checks how many instructions there actually are to execute and in case of having multiple instructions uses 'flag' to determine whether the instruction given at the current time was a built-in or not.

* int shellBuiltins(void): Returns the number of the shell's built-ins.

* int shellCd(char \*\*args): Changes the current directory of the shell.

* int shellQuit(char \*\*args): Used to exit the shell.
