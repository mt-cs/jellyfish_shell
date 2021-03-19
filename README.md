# Project 2: Command Line Shell

Author: Marisa Tania  

## About Jellyfish
Jellyfish is a custom implementation of the Unix command line shell. 

### What is shell?
Shell is the outermost layer of the operating system or the command line interface. The sh utility is a command language  interpreter  that  shall  execute
commands  read  from  a  command  line string, the standard input, or a specified file. To get a better understanding of shell, check out the man page: man sh. 

### Functions
- <b>Basics</b>: Upon startup, Jellyfish will print its prompt and wait for user input. The shell can run commands in both the current directory (with prefix ./) and those in the PATH environment variable. If a command isn’t found, Jellyfish will print an error message. The process exit status is shown as an emoji: a smiling face for success (exit code 0) and a sick face for failure (any nonzero exit code or failure to execute the child process).
```bash
[🙂]─[1]─[mtania@nemo:~/P2-mt-cs]$ ./hello
Hello world!

[🙂]─[2]─[mtania@nemo:~/P2-mt-cs]$ ls /usr
bin  include  lib  local  sbin  share  src

[🙂]─[3]─[mtania@nemo:~/P2-mt-cs]$ echo hello there!
hello there!

[🙂]─[4]─[mtania@nemo:~/P2-mt-cs]$ ./blah
crash: no such file or directory: ./blah

[🤮]─[5]─[mtania@nemo:~/P2-mt-cs]$ cd /this/does/not/exist
chdir: no such file or directory: /this/does/not/exist

[🤮]─[6]─[mtania@nemo:~/P2-mt-cs]$
```
- <b>Prompt</b>: Jellyfish prompt displays some helpful information, including :
   - Command number (starting from 1)
   - User name and host name: (username)@(hostname) followed by :
   - The current working directory
   - Process exit status
The current working directory format: if the CWD is the user’s home directory, then the entire path is replaced with ~. Subdirectories under the home directory are prefixed with ~; if I am in /home/mtania/test, the prompt will show ~/test:
```bash
[🙂]─[6]─[mmalensek@gamestop:~]$ whoami
mmalensek

[🙂]─[7]─[mtania@nemo:~]$ cd P2-mt-cs

# Create a directory with our full home directory in its path:
# **Must use the username outputted above from whoami)**
[🙂]─[8]─[mtania@nemo:~/P2-mt-cs]$ mkdir -p /tmp/home/mtania/test

[🙂]─[9]─[mtania@nemo:~/P2-mt-cs]$ cd /tmp/home/mtania/test

# Note that the FULL path is shown here (no ~):
[🙂]─[10]─[mtania@nemo:/tmp/home/mtania/test]$ pwd
/tmp/home/mtania/test
```
- <b>Scripting</b>:Support scripting mode to run the test cases. Scripting mode reads commands from standard input and executes them without showing the prompt.
```bash
cat <<EOM | ./crash
ls /
echo "hi"
exit
EOM

# Which outputs (note how the prompt is not displayed):
bin  boot  dev  etc  home  lib  lost+found  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
hi

# Another option (assuming commands.txt contains shell commands):
./crash < commands.txt
(commands are executed line by line)
```
### Program Options
Each portion of the display can be toggled with command line options. Here are the options:
```bash
$ ./inspector -h
Usage: ./inspector [-ahlrst] [-p procfs_dir]

Options:
    * -a              Display all (equivalent to -lrst, default)
    * -h              Help/usage information
    * -l              Task List
    * -p procfs_dir   Change the expected procfs mount point (default: /proc)
    * -r              Hardware Information
    * -s              System Information
    * -t              Task Information
```
The task list, hardware information, system information, and task information can all be turned on/off with the command line options. By default, all of them are displayed.

### Included Files
There are several files included. These are:
   - <b>Makefile</b>: Including to compile and run the program.
   - <b>shell.c</b>: The program driver.
   - <b>history.c</b>: Contains shell history data structures and retrieval functions.
   - <b>ui.c</b>: Text-based UI functionality, primarily concerned with interacting with the readline library.
   - <b>logger.h</b>: Helps facilitate debugging by providing basic logging functionality. 
There are also header files history.h and ui.h.


To compile and run:

```bash
make
./shell
```


### Program Output
```bash
$ ./inspector
inspector.c:133:main(): Options selected: hardware system task_list task_summary

System Information
------------------
Hostname: lunafreya
Kernel Version: 4.20.3-arch1-1-ARCH
Uptime: 1 days, 11 hours, 49 minutes, 56 seconds

Hardware Information
------------------
CPU Model: AMD EPYC Processor (with IBPB)
Processing Units: 2
Load Average (1/5/15 min): 0.00 0.00 0.00
CPU Usage:    [--------------------] 0.0%
Memory Usage: [#-------------------] 9.5% (0.1 GB / 1.0 GB)

Task Information
----------------
Tasks running: 88
Since boot:
    Interrupts: 2153905
    Context Switches: 3678668
    Forks: 38849

  PID |        State |                 Task Name |            User | Tasks 
------+--------------+---------------------------+-----------------+-------
    1 |     sleeping |                   systemd |               0 | 1 
    2 |     sleeping |                  kthreadd |               0 | 1 
    3 |         idle |                    rcu_gp |               0 | 1 
    4 |         idle |                rcu_par_gp |               0 | 1 
    6 |         idle |      kworker/0:0H-kblockd |               0 | 1 

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

See: https://www.cs.usfca.edu/~mmalensek/cs521/assignments/project-2.html

