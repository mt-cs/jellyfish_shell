# Command Line Shell

Author: Marisa Tania  

## About Jellyfish
Jellyfish is a custom implementation of the Unix command line shell. 

### What is shell?
Shell is the outermost layer of the operating system or the command line interface. The sh utility is a command language  interpreter  that  shall  execute
commands  read  from  a  command  line string, the standard input, or a specified file. To get a better understanding of shell, check out the man page: man sh. 

### Built-In Commands
Jellyfish shell supports:
```bash
Commands:
    * cd                      To change the CWD. cd without arguments should return to the userโs home directory.
    * '#' (comments)          Strings prefixed with '#' will be ignored by the shell.
    * history                 Prints the last 100 commands entered with their command numbers.
    * ! (history execution)   Entering !39 will re-run command number 39.
    * !!                      Re-runs the last command that was entered. 
    * !ls                     Re-runs the last command that starts with โls.โ
    * jobs                    To list currently-running background jobs.
    * exit                    To exit the shell.
```

### Functions
- <b>Basics</b>: Upon startup, Jellyfish will print its prompt and wait for user input. The shell can run commands in both the current directory (with prefix ./) and those in the PATH environment variable. If a command isnโt found, Jellyfish will print an error message. The process exit status is shown as an emoji: a smiling face for success (exit code 0) and a sick face for failure (any nonzero exit code or failure to execute the child process).
```bash
[๐]โ[1]โ[mtania@nemo:~/P2-mt-cs]$ ./hello
Hello world!

[๐]โ[2]โ[mtania@nemo:~/P2-mt-cs]$ ls /usr
bin  include  lib  local  sbin  share  src

[๐]โ[3]โ[mtania@nemo:~/P2-mt-cs]$ echo hello there!
hello there!

[๐]โ[4]โ[mtania@nemo:~/P2-mt-cs]$ ./blah
jellyfish: no such file or directory: ./blah

[๐คฎ]โ[5]โ[mtania@nemo:~/P2-mt-cs]$ cd /this/does/not/exist
chdir: no such file or directory: /this/does/not/exist

[๐คฎ]โ[6]โ[mtania@nemo:~/P2-mt-cs]$
```
- <b>Prompt</b>: Jellyfish prompt displays some helpful information, including :
   - Command number (starting from 1)
   - User name and host name: (username)@(hostname) followed by :
   - The current working directory
   - Process exit status
The current working directory format: if the CWD is the userโs home directory, then the entire path is replaced with ~. Subdirectories under the home directory are prefixed with ~; if I am in /home/mtania/test, the prompt will show ~/test:
```bash
[๐]โ[6]โ[mtania@nemo:~]$ whoami
mtania

[๐]โ[7]โ[mtania@nemo:~]$ cd P2-mt-cs

# Create a directory with our full home directory in its path:
# **Must use the username outputted above from whoami)**
[๐]โ[8]โ[mtania@nemo:~/P2-mt-cs]$ mkdir -p /tmp/home/mtania/test

[๐]โ[9]โ[mtania@nemo:~/P2-mt-cs]$ cd /tmp/home/mtania/test

# Note that the FULL path is shown here (no ~):
[๐]โ[10]โ[mtania@nemo:/tmp/home/mtania/test]$ pwd
/tmp/home/mtania/test
```
- <b>Scripting</b>: Support scripting mode to run the test cases. Scripting mode reads commands from standard input and executes them without showing the prompt.
```bash
cat <<EOM | ./jellyfish
ls /
echo "hi"
exit
EOM

# Which outputs (note how the prompt is not displayed):
bin  boot  dev  etc  home  lib  lost+found  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
hi

# Another option (assuming commands.txt contains shell commands):
./jellyfish < commands.txt
(commands are executed line by line)
```
- <b>Signal Handling</b>: Handle the user pressing Ctrl+C and making sure ^C doesnโt terminate the shell. 
```bash
[๐]โ[11]โ[mtania@nemo:~]$ hi there oh wait nevermind^C

[๐]โ[11]โ[mtania@nemo:~]$ ^C

[๐]โ[11]โ[mtania@nemo:~]$ ^C

[๐]โ[11]โ[mtania@nemo:~]$ sleep 100
^C

[๐คฎ]โ[12]โ[mtania@nemo:~]$ sleep 5
```
- <b>History</b>: In the demo below, the user has entered 142 commands. Only the last 100 are kept, so the list starts at command 43. Blank command would not be shown in the history or increment the command counter. The entire, original command line string is shown in the history โ not a tokenized or modified string.
```bash
[๐]โ[142]โ[mtania@nemo:~]$ history
  43 ls -l
  43 top
  44 echo "hi" # This prints out 'hi'

... (commands removed for brevity) ...

 140 ls /bin
 141 gcc -g jellyfish.c
 142 history

```
- <b>I/O Redirection</b>: Support file input/output redirection.
```bash
# Create/overwrite 'my_file.txt' and redirect the output of echo there:
[๐]โ[14]โ[mtania@nemo:~]$ echo "hello world!" > my_file.txt
[๐]โ[15]โ[mtania@nemo:~]$ cat my_file.txt
hello world!

# Append text with '>>':
[๐]โ[16]โ[mtania@nemo:~]$ echo "hello world!" >> my_file.txt
[๐]โ[17]โ[mtania@nemo:~]$ cat my_file.txt
hello world!
hello world!

# Let's sort the /etc/passwd file via input redirection:
[๐]โ[18]โ[mtania@nemo:~]$ sort < /etc/passwd > sorted_pwd.txt

# Order of < and > don't matter:
[๐]โ[19]โ[mtania@nemo:~]$ sort > sorted_pwd.txt < /etc/passwd

# Here's input redirection by itself (not redirecting to a file):
[๐]โ[20]โ[mtania@nemo:~]$ sort < sorted_pwd.txt
(sorted contents shown)
```
- <b>Background Jobs</b>: If a command ends in &, it will run in the background without waiting for the command to finish before prompting for the next command. If user enter jobs, Jellyfish shell will print out a list of currently-running backgrounded processes (the original command line as it was entered, including the & character). The status of background jobs is not shown in the prompt.

- <b>Readline Library</b>: Jellyfish shell's basic โterminal UI.โ supports moving through the current command line with arrow keys, backspacing over portions of the command, and even basic file name autocompletion.

### Included Files
There are several files included. These are:
   - <b>Makefile</b>: Including to compile and run the program.
   - <b>shell.c</b>: The program driver.
   - <b>history.c</b>: Contains shell history data structures and retrieval functions.
   - <b>ui.c</b>: Text-based UI functionality, primarily concerned with interacting with the readline library.
   - <b>util.c</b>: Util to tokenize the command input.
   - <b>job.c</b>: Background jobs functionality.
   - <b>signal.c</b>: Signal functionality.
   - <b>clist.c</b>: Circular list data structure to store the history.
   - <b>elist.h</b>: Elastic list data structure built from scratch.
   - <b>logger.h</b>: Helps facilitate debugging by providing basic logging functionality. 
There are also header files util.h, jobs.h, signal.h, history.h, clist.h and ui.h.


To compile and run:

```bash
make
./jellyfish
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```


