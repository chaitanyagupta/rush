# Writing a UNIX shell in C

---

## Topics
* Parsing a command line
* Spawning a process
* Handling signals
* Job control
* Files
* Pipes and redirection

---

# Parsing a command line

---

## The command line

```
  $  echo foo bar baz
  |   |    |   |   |
  |   |    |___|___|
  |  name      |
  |        arguments
shell
prompt
```

---

## The entry function

```c
// printargs.c
#include <stdio.h>

int main (int argc, char *argv[]) {
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
```

---

## The entry function

```
$ printargs foo bar baz
argv[0] = printargs
argv[1] = foo
argv[2] = bar
argv[3] = baz

$ printargs foo "bar baz"
argv[0] = printargs
argv[1] = foo
argv[2] = bar baz

$ ./bin/printargs foo bar baz
argv[0] = ./bin/printargs
argv[1] = foo
argv[2] = bar
argv[3] = baz

$ ln -s ./bin/printargs echoargs
$ ./echoargs foo bar baz
argv[0] = ./echoargs
argv[1] = foo
argv[2] = bar
argv[3] = baz
```

---

## BusyBox

"The Swiss Army Knife of Embedded Linux"
[https://www.busybox.net/]

```
/bin $ ls -l
lrwxrwxrwx    1 609      609             7 Jan 18  2016 addgroup -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 adduser -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 ash -> busybox
-rwxr-xr-x    1 609      609        292965 Jan 18  2016 busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 cat -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 chmod -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 cp -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 date -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 delgroup -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 deluser -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 echo -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 false -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 grep -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 kill -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 ln -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 login -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 ls -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 mkdir -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 mknod -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 mount -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 mv -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 ping -> busybox
lrwxrwxrwx    1 609      609             7 Jan 18  2016 ps -> busybox
```

---

## Non-ASCII characters in args

* Print non-ASCII characters via a special bash synax: `$'\x<2 digit hex>'`
* `echo`: print arguments (exlcuding `argv[0]`), separated by a whitespace
* `xxd`: convert binary to hex

```
$ echo -n foo bar baz
foo bar baz

$ echo -n foo bar baz | xxd
0000000: 666f 6f20 6261 7220 6261 7a              foo bar baz

$ echo -n $'\x09' foo
	 foo

$ echo -n $'\x09' foo | xxd
0000000: 0920 666f 6f                             . foo

$ echo -n $'\x12\x13\x14' | xxd
0000000: 1213 14                                  ...

$ echo -n $'\x12\x00\x14' | xxd
0000000: 12                                       .
```

---

## Reading a command line

* Use `fgets` to read from standard input into a C string
* `gets` is more commonly known, but **never** use it. It lacks bounds checking.

```c
void prompt_and_read(char *line, size_t max_len) {
    fputs("$ ", stdout);
    fgets(line, max_len, stdin);
    strsep(&line, "\n");
}

int main() {
    char line[MAX_LINE_LENGTH] = { 0 };
    int should_exit;
    do {
        prompt_and_read(line, sizeof(line));
        should_exit = strcmp(line, "exit") == 0;
        if (!should_exit) {
            // do further processing
        }
    } while (!should_exit);
    return 0;
}

```

---

## Splitting a C string

Replace all the delimiters in a string

```
    e   c   h   o       f   o   o       b   a   r
    65  63  68  6f  20  66  6f  6f  20  62  61  72  00
```

with the null terminator

```
    e   c   h   o       f   o   o       b   a   r
    65  63  68  6f  00  66  6f  6f  00  62  61  72  00
```

---

## strsep

```c
char *strsep(char **stringp, const char *delim);
```

`parse_line`: Reads a `line`, splits it in-place and places each token in `argv`, and returns the number of args.

```c
int parse_line(char *line, char *argv[], size_t max_argc) {
    char **ap = argv;
    while ((*ap = strsep(&line, " \t")) != NULL) {
        if (**ap != '\0')
            if (++ap >= &argv[max_argc - 1])
                break;
    }
    *ap = NULL;
    return ap - argv;
}

```

Quiet limited; e.g. doesn't handle quotes to delimit arguments.

---

## parse_line

Take this line

```
          e   c   h   o       f   o   o       b   a   r
          65  63  68  6f  20  66  6f  6f  20  62  61  72  00
```

split and place its tokens into `argv`:

```
          e   c   h   o       f   o   o       b   a   r
          65  63  68  6f  20  66  6f  6f  20  62  61  72  00
          |                   |               |
argv[0]---+                   |               |
argv[1]-----------------------+               |
argv[2]---------------------------------------+
argv[3] = NULL
```

---

## Read line and split tokens

```c
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

int main() {
    char line[MAX_LINE_LENGTH];
    char *argv[MAX_ARGC];
    int should_exit;
    do {
        prompt_and_read(line, sizeof(line));
        should_exit = strcmp(line, "exit") == 0;
        if (!should_exit) {
            int argc = parse_line(line, argv, ARRAY_SIZE(argv));
            // do further processing
        }
    } while (!should_exit);
    return 0;
}
```

---

# Spawning a process

---

## Executing the program

```
$ echo foo bar

argv[0] = "echo"
argv[1] = "foo"
argv[2] = "bar"
argv[4] = NULL
```

Where is `echo`?

---

## PATH search

* To find `echo`, look for it in all the locations specified by the `PATH` environment variable
* The "environment" is usually inherited from the parent process and is available to any C program in the global variable `environ`.

---

## The Environment

```c
// printenv.c
#include <stdio.h>

extern char **environ;

int main() {
    char **env;
    for (env = environ; *env != NULL; ++env) {
        printf("%s\n", *env);
    }
    return 0;
}
```

Usage:

```
$ printenv
TERM=xterm-256color
SHELL=/bin/bash
TMPDIR=/var/folders/h2/yzs4zslx1yn6v78pgrfgxsd40000gn/T/
PWD=/Users/chaitanya/directi-projects/rush
OLDPWD=/Users/chaitanya
USER=chaitanya
PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
HOME=/Users/chaitanya
...
```

---

## The Environment

Much easier to use these helper functions than accessing `environ` directly.

```c
char *getenv(const char *name);

int putenv(char *string);

int setenv(const char *name, const char *value, int overwrite);

int unsetenv(const char *name);
```

---

## search_path

`search_path`: Lookup name in the list of paths provided by the `PATH` environment variable. If found, return non-zero and copy the path into `path`, else return zero.

```c
int search_path(char *name, char *path, size_t maxlen) {
    char pathdirs[MAX_PATH_VAR_LEN];
    strlcpy(pathdirs, getenv("PATH"), sizeof(pathdirs));
    char *dir, *head = pathdirs;
    int success = 0;
    while ((dir = strsep(&head, ":")) != NULL) {
        strlcpy(path, dir, maxlen);
        strlcat(path, "/", maxlen);
        strlcat(path, name, maxlen);
        if (access(path, X_OK) == 0) {
            return 1;
        }
    }
    return 0;
}
```
---

## search_path

```c
// main routine
do {
    prompt_and_read(line, sizeof(line));
    should_exit = strcmp(line, "exit") == 0;
    if (!should_exit) {
        int argc = parse_line(line, argv, ARRAY_SIZE(argv));
        char binpath[MAX_PATH_LEN];
        if (search_path(argv[0], binpath, sizeof(binpath))) {
            // do further processing
        } else {
            fprintf(stderr, "Couldn't locate %s\n", argv[0]);
        }
    }
 } while (!should_exit);
```

???

One more thing that a UNIX shell does is that, if the program name contains a slash, then PATH is not searched.

---

# The End
