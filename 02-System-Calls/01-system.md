## `system()` Funcnction
### Header File
```c
#include "stdlib.h"
```
### Syntax
```c
int system(const char* command);
```
### Parameter
| Parameter | Meaning |
| - | - |
|`command`| A shell command to be executed |

## How `system()` works internally
### When `system()` is called, the following happens internally:

1. `fork()` is called
→ Creates a child process.

2. `exec()` is called
→ Executes /bin/sh -c command.

3. `wait()` is used
→ Parent waits until command finishes.

### Return Value
| Return Value | Meaning |
|-|-|
| -1 | Error Occurred|
| command status| Exit status of shell command |

### Example Programe
``` c 
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[]) {

    system("ls >> 01-system.txt");
    system("cat 01-system.txt");

    return EXIT_SUCCESS;
}
```
### Compile and Run:
``` bash
clear && gcc 01-system.c -o 01-system && ./01-system
```

### Output 
```bash
01-system
01-system.c
01-system.md
01-system.txt
01-system
01-system.c
01-system.md
01-system.txt
```

## Equivalent Without `system()`
### Instead of `system()`, we can use `fork` + `exec`.
```c
#include "unistd.h"
#include "sys/wait.h"
#include "sys/types.h"

int main() {

    pid_t pid = fork();

    pid != 0 ? wait(NULL) : execl("/bin/ls", "ls", NULL);;
    
    return 0;
}
```