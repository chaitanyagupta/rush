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

## Splitting a C string

Replace all the delimiters in a string

```
    f   o   o       b   a   r       b   a   z
    66  6f  6f  20  62  61  72  20  62  61  7a  00
```

with the null terminator

```
    f   o   o       b   a   r       b   a   z
    66  6f  6f  00  62  61  72  00  62  61  7a  00
```

---


[The End]
