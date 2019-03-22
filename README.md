# myfsinfo
A system program that provides useful information about processes, files and the use of Linux resources by a user. 

It uses both system calls and the information provided by the /proc virtual filesystem in order to display information from the i-nodes as well as to have access to the content of directories, symbolic links and files of various types.

Project in the context of the Operating Systems course in Fall 2015 at University of Athens.

## Features

The program can:

1) Read a directory containing regular files, other directories,symbolic links, and special type files like character
device files, block device files, socket files and named pipes.

```
./myfsinfo -cnt <directory>
```

2) Read a hierarchy of directories/files and display the elements of the filesystem that have been modified or read during a time period specified by the user.

```
./myfsinfo -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory>
```

3) Display infromation about the processes that the user is executing simultaneously in a tty, in the same way the top and ps programs do.

```
./myfsinfo -ps
```

4) Display information about the files used by the processes executed by the user simultaneously in a tty session.

```
./myfsinfo -ft
```

5) Display information about the use of input/output by the user processes.

```
./myfsinfo -iostat -k <#records> -f <fieldname>
```

6) Display information about the network use by the user processes.

```
./myfsinfo -netstat [tcp|udp]
```
