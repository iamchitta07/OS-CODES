# Notes

## exit(), sleep(), pause() and alarm()

### exit(): 

> **The exit() system call is the standard way a process terminates its execution in a C environment. When a program calls exit, it doesn't just stop; it performs a series of "cleanup" steps before handing control back to the Operating System.**

When `exit(status)` is invoked, the following sequence occurs:
- Cleanup Functions: Any functions registered with `atexit()` or `on_exit()` are called in the reverse order of their registration.

- Stream Flushing: All open output `streams` are flushed (buffered data is written out) and closed.

- Temporary Files: Any temporary files created with `tmpfile()` are removed.

- Process Termination: The kernel is notified. It releases the memory, closes remaining file descriptors, and transitions the process into a `Zombie` state until the parent process collects its `exit` status.

`status`:
- `0` (or `EXIT_SUCCESS`): Indicates the program completed successfully.
- Non-zero (or `EXIT_FAILURE`): Indicates an error occurred. The specific number can be used by the parent process or shell to determine what went wrong.
```c
#include <stdlib.h>

void exit(int status);
```

### sleep():

> The `sleep()` function suspends the execution of the calling thread for a specified number of seconds.

The process enters a "Waiting" or "Sleep" state. It remains there until either the time has elapsed or a signal is delivered that is not ignored.

Returns `0` if the requested time has elapsed. If interrupted by a signal, it returns the `number of seconds remaining`.

```C
#include "stdio.h"
#include "unistd.h"

int main() {
    printf("Taking a 5-second nap...\n");
    unsigned int remaining = sleep(5);
    
    if (remaining == 0) {
        printf("Woke up naturally.\n");
    } else {
        printf("Interrupted! %u seconds left.\n", remaining);
    }
    return 0;
}
```

### pause():

> **The `pause()` function causes the calling process to sleep until a signal is delivered that either terminates the process or causes the invocation of a signal-catching function.**

It is the ultimate "waiter." It consumes no CPU cycles while waiting.

It only returns when a signal handler finishes. It always returns `-1` and sets `errno` to `EINTR`.
```c
#include "stdio.h"
#include "unistd.h"
#include "signal.h"

void fn(int sig) {
    printf("Caught signal %d, waking up from pause!\n", sig);
}

int main() {
    signal(SIGUSR1, fn);    
    printf("Process ID: %d. Waiting for SIGUSR1...\n", getpid());
    printf("Use 'kill -USR1 %d' from another terminal.\n", getpid());
    pause();     
    printf("Pause finished.\n");
    return 0;
}
```

### alarm(): 

> **The `alarm()` function sets a "timer" that delivers a `SIGALRM` signal to the process after a specified number of seconds.**

It does not pause the program. The program continues to run, and the signal arrives asynchronously. Non-blocking nature

Only one alarm can be active at a time. Calling alarm() again cancels the previous one and returns the seconds remaining on it. `alarm(0)` cancels any existing alarm.
```c
#include "stdio.h"
#include "unistd.h"
#include "signal.h"
#include "stdlib.h"

void fn(int sig) {
    printf("Alarm Rigned! After 5s");
    exit(0);    
}

int main() {
    signal(SIGALRM, fn);
    alarm(5);
    for(;;);
    return 0;
}
```

### wait():

> **In C, the `wait()` system call is used by a parent process to synchronize its execution with its child processes. It specifically allows a parent to pause until one of its children terminates and then retrieve that child's exit status.**

```c
#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "sys/types.h"

int main() {
    pid_t pid = fork();
    printf("ParentId: [%d], PID: [%d]\n", getppid(), getpid());
    if(pid) {
        printf("Parent is Executing!\n");
        wait(NULL);
    } else {
        printf("Child is Executing!\n");
        sleep(6);
        exit(0);
    } printf("Process finished.\n");
    return 0;
}
```

#### **Syntax:**

***
> `stat_loc`: A pointer to an integer where the system stores the child's exit information.

Return Value: Returns the `PID` of the terminated child on success, or `-1` if there are no children to `wait` for.
```C
#include "sys/wait.h"
#include "sys/types.h"

pid_t childPID = wait(int *stat_loc);
```

#### **Analyzing the Exit Status:**
***
> The integer populated in `stat_loc` isn't just the exit code; it contains bitwise information about how the child died (normally, via a signal, etc.). We use standard macros to decode it:

- `WIFEXITED(status)`: Returns true if the child terminated normally.

- `WEXITSTATUS(status)`: Returns the actual exit code (only if `WIFEXITED` is true).

- `WIFSIGNALED(status)`: Returns true if the child was killed by a signal (like a crash).

```C
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"

int main(int argc, char* const argv[]) {
    pid_t pid = fork();

    if (!pid) {
        printf("Child (PID: %d) is running...\n", getpid());
        sleep(2);
        printf("Child is exiting now.\n");
        exit(42); // Exit with a specific status code
    } else {
        int status;
        printf("Parent (PID: %d) is waiting for Child (PID: %d)...\n", getpid(), pid);
        
        pid_t child_pid = wait(&status);

        if (WIFEXITED(status)) {
            printf("Parent: Child %d finished with status: %d\n", 
                    child_pid, WEXITSTATUS(status));
        } else {
            printf("Child did not terminate normally.\n");
        }
    }

    return EXIT_SUCCESS;
}
```

### waitpid():

> **While `wait()` is simple, it is also blunt—it waits for any child process and blocks the parent until one finishes. `waitpid()` is the more surgical version, giving you control over which child to wait for and how to wait.**

#### **Syntax:**
***
```c
#include "sys/wait.h"
#include "sys/types.h"

pid_t waitpid(pid_t pid, int *status, int options);
```
1. The `pid` Argument (Who to wait for?)
    - `pid > 0`: Wait for the specific child with that Process ID.
    - `pid == -1`: Wait for any child (behaves exactly like `wait()`).
    - `pid == 0`: Wait for any child in the same process group as the parent.
2. The `options` Argument (How to wait?)<br/>
This is where waitpid becomes powerful. You can pass 0 for default behavior, or flags like:
    - `WNOHANG`: Non-blocking mode. The parent checks if the child is done. If not, it returns 0 immediately instead of pausing. This allows the parent to do other work while the child runs.
    - `WUNTRACED`: Also returns if a child has stopped (e.g., via `SIGSTOP`).

#### Why use `waitpid()`?
1. **Handling Multiple Children**: If you fork three children (A, B, and C), `wait()` might catch "C" first even if you needed "A". `waitpid(pid_A, ...)` ensures you get exactly who you want.
2. **Polling (Avoid Blocking)**: With `WNOHANG`, your parent process can stay responsive (like a GUI or a web server) while checking on background tasks.

### **Example:**

```C
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"

int main() {
    pid_t pid = fork();
    if (!pid) {
        printf("Child: I'm busy for 3 seconds...\n");
        sleep(3);
        exit(7);
    } else {
        int status;
        pid_t result;

        printf("Parent: I'm not stopping! Checking on child...\n");

        while ((result = waitpid(pid, &status, WNOHANG)) == 0) {
            printf("Parent: Child still running... I'll do some work.\n");
            sleep(1); 
        } if (result == -1) {
            perror("waitpid error");
        } else {
            if (WIFEXITED(status)) {
                printf("Parent: Finally! Child exited with code %d\n", WEXITSTATUS(status));
            }
        }
    } return 0;
}
```

### **fork():**
> In C, the `fork()` system call is used to create a new process by duplicating the existing one. It is the fundamental way processes are spawned in Unix-like operating systems.
### **How `fork()` Works**
When `fork()` is called, the Operating System creates a nearly identical copy of the calling process (the Parent) and names the new one the Child.

- Memory Duplication: The child gets a copy of the parent's data, heap, and stack. However, they do not share memory; if the child changes a variable, it doesn't change for the parent.

- Execution Point: Both processes continue executing from the exact same point—the statement immediately following the `fork()` call.

- Unique Attributes: The child has its own unique Process ID (`PID`).

***

### **The Return Value**

The most important part of `fork()` is its return value, which allows the program to distinguish if it is currently the parent or the child.

1. To the Parent: `fork()` returns the PID of the child.

2. To the Child: `fork()` returns 0.

3. On Failure: `fork()` returns `-1` (usually if the system limit for processes is reached).

***

### **Important Concepts**
1. **Orphan Processes**: If a parent process terminates before its child, the child becomes an `Orphan`. The system handles this by making the `init process` (`PID 1`) the new parent of the orphan to ensure it eventually gets reaped.

2. **Zombie Processes**: If a child terminates but the parent hasn't yet called `wait()`, the child becomes a `Zombie`. It’s "dead" but still takes up an entry in the process table.

3. **Copy-on-Write (COW)**: Modern OSs don't actually copy all the memory immediately (which would be slow). Instead, they share the same memory pages until one of the processes tries to write to them. At that moment, a copy is made.
#### **Example:**
```C
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"

int main() {
    pid_t pid;
    int x = 10;

    printf("Before forking, x is %d\n", x);

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    } 
    else if (!pid) {
        x = 20; 
        printf("CHILD: My PID is %d, Parent PID is %d. x = %d\n", getpid(), getppid(), x);
    } 
    else {
        sleep(1); 
        printf("PARENT: My PID is %d, Child PID is %d. x = %d\n", getpid(), pid, x);
    }

    return 0;
}
```

### **vfork()**
> `vfork()` creates a child process similar to `fork()`, but it is designed to be more efficient when the child immediately calls `exec()`.

### Syntax
```c
#include "unistd.h"
#include "sys/types.c"

pid_t vfork(void);
```

### **How `vfork()` Works**
When `fork()` is called:
1. A **child process is created**.
2. The **child shares the same memory space** as the parent.
3. The **parent process is suspended** until the child:
    * calls `exec()` or
    * calls `_exit()`.
    ***
So the child **does not get its own copy of memory**.

### Return Values
| Return Value | Meaning |
| - | - |
| 0 | Returned to child process |
| Child PID | Returned to parent |
| -1 | Error occurred |

#### **Example:**
```C
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"

int main(int argc, char *const argv[]) {
    int x = 10;

    pid_t pid = vfork();

    if(pid == 0) {
        x = 20;
        printf("Child: x = %d\n", x);
        _exit(0);
    }
    else {
        printf("Parent: x = %d\n", x);
    }

    return EXIT_SUCCESS;
}
```
### Possible Output
```bash
Child: x = 20
Parent: x = 20
```
### Restrictions of `vfork()`:
While using `vfork()`:
* Child **must not**:
    * modify variables
    * return from the function
    * call `exit()`
* Child **should only**:
    * call `exec()`
    * call `_exit()`
    ***
Otherwise undefined behavior occurs.

### **clone()**
> `clone()` creates a child process but allows the programmer to decide which parts of the execution environment are shared.

* These can include:
    * Memory space
    * File descriptors
    * Signal handlers
    * Filesystem information
    * Thread group
    ***
So unlike `fork()` (which copies everything), `clone()` **allows fine-grained control**.

### Syntax
```c
#define _GNU_SOURCE
#include "sched.h"
#include "sys/types.h"
#include "unistd.h"

int clone(int (*fn)(void *), void *child_stack, int flags, void *arg);
```

### Parameters Explained
1. `fn`: Function executed by the child.
    ```c
    int child_function(void *arg);
    ```
    
2. `child_stack`: Pointer to the **top of the stack for the child process**.
    Unlike `fork()`, Linux **requires a manually allocated stack**.
    ```c
    #define STACK_SIZE 1024*1024
    char stack[STACK_SIZE];
    // pass
    stack + STACK_SIZE
    ```

3. `flags`: This is the most **important parameter**. It determines **what the child shares with the parent**.
Examples of Flags:<br/>
    | Flag | Meaning |
    |-|-|
    | `CLONE_VM` | Share memory |
    | `CLONE_FS` | Share filesystem info |
    | `CLONE_FILES` | Share file descriptors |
    | `CLONE_SIGHAND` | Share signal handlers |
    | `CLONE_THREAD` | Same thread group |
    | `CLONE_PARENT` | Same parent process |
    | `CLONE_NEWNS` | New mount namespace |
    | `CLONE_NEWUTS` | New hostname domain |
    Flags are combined using **bitwise** `OR`.

    Example 
    ```c
    int flags = CLONE_VM | CLONE_FILES;
    ```
4. `args`: Argument passed to the child function.
### Example:
```c
#define _GNU_SOURCE
#include "sched.h"
#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "sys/wait.h"
#include "sys/types.h"
#define STACK_SIZE 1024*1024 // 1MB

int childFunc(void *arg) {
    printf("Hello from child\n");
    return EXIT_SUCCESS;
}

int main(int argc, char* const argv[]) {

    char stack[STACK_SIZE];

    pid_t pid = clone(childFunc, stack + STACK_SIZE, CLONE_VM | CLONE_FS | CLONE_FILES | SIGCHLD, NULL);

    printf("Parent running\n");
    waitpid(pid, NULL, 0);

    return EXIT_SUCCESS;
}
```
* POSIX threads (`pthread_create`) internally call:
```c
clone(
   flags = CLONE_VM | CLONE_FS | CLONE_FILES |
           CLONE_SIGHAND | CLONE_THREAD
)
```
This makes threads share:
* memory
* files
* signals




### **kill()**

> In `C`, the `kill()` function is the programmatic way to **send** signals.

#### **The `pid` Argument**

- `pid>0`: Signal is sent to the specific process.

- `pid==0`: Signal is sent to every process in the process group of the caller.

- `pid==-1`: Signal is sent to every process the caller has permission to send signals to.

#### **Example**
```C
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "signal.h"
#include "sys/types.h"

int main() {
    pid_t pid = fork();

    if (!pid) {
        for(;;) {
            printf("Child: I am running...\n");
            sleep(1);
        }
    } else {
        sleep(3);
        printf("\nParent: Pausing child (SIGSTOP)...\n");
        kill(pid, SIGSTOP);
        
        sleep(3);
        printf("Parent: Resuming child (SIGCONT)...\n");
        kill(pid, SIGCONT);
        
        sleep(3);
        printf("Parent: Killing child (SIGKILL)...\n");
        kill(pid, SIGKILL);
        
        printf("Parent: Done.\n");
    } return 0;
}
```

### **signal()**

> In Linux and C programming, a `signal` is a software interrupt sent to a program to indicate that an important event has occurred. Think of it as a "notification" from the Operating System (or another process) that forces your program to stop what it's doing and handle the situation.

#### **Example:**

> By default, `SIGINT` (Ctrl+C) kills your program. Here is how you "catch" it using the `signal()` function so the program stays alive.

```C
#include "stdio.h"
#include "signal.h"
#include "unistd.h"
#include "stdlib.h"

void my_handler(int sig_num) {
    printf("\nCaught signal %d (SIGINT). Cleaning up...\n", sig_num);
    printf("I'm not exiting yet! Press Ctrl+C again if you want.\n");
    
    // If we wanted to exit gracefully:
    // exit(0);
}

int main() {
    signal(SIGINT, my_handler);

    printf("Program running. Try pressing Ctrl+C...\n");

    while(1) {
        printf("Working...\n");
        sleep(2);
    } return 0;
}
```
### **clone()**

> In C and Linux systems, `clone()` is the "Swiss Army Knife" of process and thread creation. It is the underlying system call that powers both `fork()` and `pthread_create()`.<br/><br/>While `fork()` creates a full copy of a process, `clone()` allows you to choose exactly what the parent and child should share (memory, file descriptors, signals, etc.).

#### **Syntax:** 

`clone()` is more complex than `fork()` because it requires a manual stack setup and a function for the new process to run.

```C
#define _GNU_SOURCE
#include "sched.h"

int clone(int (*fn)(void *), void *stack, int flags, void *arg, ...);
```

- The Arguments:
    1. `fn`: The function the new process will start executing.

    2. `stack`: Unlike `fork()`, you must allocate memory for the child’s stack.

    3. `flags`: This is where the magic happens. You tell the kernel what to share.

    4. `arg`: The data passed to the function `fn`.

#### **Example:**
> In this example, we create a child that shares memory with the parent using `CLONE_VM`. Notice how the child changes a variable and the parent sees the change.

```C
#define _GNU_SOURCE
#include "stdio.h"
#include "stdlib.h"
#include "sched.h"
#include "sys/wait.h"
#include "unistd.h"

#define STACK_SIZE (1024 * 1024) 

int child_func(void *arg) {
    int *shared_var = (int *)arg;
    printf("CHILD: Read shared_var = %d. Changing it to 100...\n", *shared_var);
    *shared_var = 100;
    return 0;
}

int main() {
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    int data = 42;
    printf("PARENT: Before clone, data = %d\n", data);

    if (clone(child_func, stack + STACK_SIZE, CLONE_VM | SIGCHLD, &data) == -1) {
        perror("clone");
        exit(1);
    }

    wait(NULL); // Wait for child to finish
    printf("PARENT: After child finished, data = %d\n", data);

    free(stack);
    return 0;
}
```

### **pause()**
 The `pause()` system call is the simplest way to make a process wait. It essentially puts a process into an "indefinite sleep" until it receives a signal. Unlike `sleep()`, which wakes up after a timer expires, `pause()` only wakes up when a signal is delivered.

#### **How it Works**
- When a process calls `pause()`, the following happens:

    1. `Suspension`: The process execution is suspended. It is moved from the `Running` state to the `Waiting`(or Interruptible Sleep) state.

    2. `Resource Efficiency`: While paused, the process consumes zero CPU cycles. It stays in the process table but does nothing.

    3. `Waking Up`: The process stays suspended until a signal is received that either:

        - Executes a signal-catching function (handler).

        - Terminates the process.

#### **Syntax:**
```C
#include "unistd.h"

int pause(void);
```
- Return Value: `pause()` only returns if a signal was caught and the signal-handling function returned.

- The "Error" Return: In this case, it always returns `-1` and sets the global variable `errno` to `EINTR` (Interrupted system call). This is one of the few functions where a return value of `-1` is actually expected behavior for a successful wake-up.

#### **Example:**
```C
#include "stdio.h"
#include "unistd.h"
#include "signal.h"

void fn(int sig) {
    printf("\nSignal %d received! pause() has been interrupted.\n", sig);
}

int main() {
    // Register a handler for SIGINT (Ctrl+C)
    signal(SIGINT, fn);

    printf("Process ID: %d\n", getpid());
    printf("The program is now paused. Press Ctrl+C to wake it up...\n");

    // The process goes to sleep here indefinitely
    int result = pause();

    if (result == -1) {
        printf("pause() returned -1 as expected. Program continuing...\n");
    } printf("Program exiting gracefully.\n");
    return 0;
}
```

## Process Code and Tasks

| Process Code | Tasks |
| --- | --- |
| R | Running or Runnable (ready to run) |
| S | Interruptible Sleep |
| D | Un-Interruptiable Sleep |
| T | Stopped |
| Z | Zombie |

### **Runnig or Runnable(R):**

    In the running state, the process takes up a CPU core to execute its code and logic. Although the running and runnable states are distinct, they are collectively grouped into a single state denoted by the character R

### **Uninterruptible Sleeping State(D):**

    Waits for the resources to be available before it moves into a runnable state and doesn’t react to any signals, eg. request for these resources are IO-based such as to read a file from disk or make a network request.

### **Interruptible Sleeping State(S):**

    Is the interruptible sleeping state(s) that will react to signals and the availability of resources. A process enters the S state, or Interruptible Sleep, when it waits for an event or condition that is not directly related to an I/O operation, e.g. application waits for the user to type a command or input text.

### **Stopped State(T):**

    From a running or runnable state, we could put a process into the stopped state (T) using the SIGSTOP or SIGTSTP signals. SIGSTOP is programmatic, whereas SIGTSTP signal using the keyboard CTRL+Z. Unlike SIGSTOP, the process can optionally ignore this signal and continue to execute upon receiving SIGTSTP. While in this state, we could bring back the process into a running or runnable state by sending the SIGCONT signal.

### **Zombie(Z):**

    When a process has completed its execution or is terminated calling exit(), it’ll send the SIGCHLD signal to the parent process and go into the zombie state(Z).The zombie process, also known as a defunct process, will remain in this state until the parent process clears it off from the process table. 


## Inter Process Communication(IPC)
### Two models of IPC
1. Shared memory
2. Message passing


## Threading
### Benifits
1. **Responsiveness**: may allow continued execution if part of process is blocked, especially important for user interfaces.
2. **Resource Sharing**: threads share resources of process, easier than shared memory or message passing
3. **Economy**: cheaper than process creation, thread switching lower overhead than context switching
4. **Scalability**: process can take advantage of multicore
architectures

## Unix/Linux Signals

    In Unix/Linux C signal handling, signals are asynchronous notifications sent to a process to notify it about events such as interrupts, illegal operations, termination requests, etc. They are defined in <signal.h>.

### List of Signals (1-31)
| No | Signal Name | Default Action | Description |
|-|-|-|-|
| 1 | SIGHUP | Terminate | Sent when a terminal controlling a process is closed (Hangup). Often used to tell daemons to reload configuration. |
| 2 | SIGINT | Terminate | Interrupt signal sent when user presses `Ctrl+C` in terminal. Requests program to stop. |
| 3 | SIGQUIT | Core Dump | Sent by `Ctrl+\`. Terminates process and produces core dump for debugging. |
| 4 | SIGILL | Core Dump | **Illegal instruction**. Happens when CPU executes invalid machine code. |
| 5 | SIGTRAP | Core Dump | Trap signal used by debuggers (breakpoints). |
| 6 | SIGABRT | Core Dump | Abort signal sent by `abort()` function. Indicates abnormal termination. |
| 7 | SIGBUS | Core Dump | Bus error due to invalid **memory access** (alignment issues or non-existent memory). |
| 8 | SIGFPE | Core Dump | **Floating point** exception such as divide-by-zero or overflow. |
| 9 | SIGKILL | Terminate | **Forcefully kills process**. Cannot be caught, ignored, or handled. |
| 10 | SIGUSR1 | Terminate | **User-defined signal 1**. Used by applications for custom purposes. |
| 11 | SIGSEGV | Core Dump | **Segmentation fault** due to invalid memory access. |
| 12 | SIGUSR2 | Terminate | **User-defined signal 2**. Used for application-specific usage. |
| 13 | SIGPIPE | Terminate | Occurs when writing to a **pipe with no reader.** |
| 14 | SIGALRM | Terminate | Sent when a timer set by `alarm()` expires. |
| 15 | SIGTERM | Terminate | Default **termination signal** used by `kill`. Allows program to clean up. |
| 16 | SIGSTKFLT | Terminate | Stack fault (rarely used on modern systems). |
| 17 | SIGCHLD | Ignore | Sent to parent when **child process terminates or stops**. |
| 18 | SIGCONT | Continue | Resumes a process that was stopped. |
| 19 | SIGSTOP | Stop | **Stops (pauses) process immediately**. Cannot be caught or ignored. |
| 20 | SIGSTP | Stop | Terminal stop signal from `Ctrl+Z`. Can be caught by program. |
| 21 | SIGTTIN | Stop | Background process tries to **read from terminal.** |
| 22 | SIGTTOU | Stop | Background process tries to **write to terminal.** |
| 23 | SIGURG | Ignore | Urgent condition on **socket** (out-of-band data). |
| 24 | SIGXCPU | Core Dump | CPU time limit exceeded. |
| 25 | SIGXFSZ | Core Dump | File size limit exceeded. |
| 26 | SIGVTALRM | Terminate | Virtual timer expired. |
| 27 | SIGPROF | Terminate | Profiling timer expired (used by profilers). |
| 28 | SIGWINCH | Ignore | Window size change in terminal. |
| 29 | SIGIO | Terminate | I/O now possible (asynchronous I/O). |
| 30 | SIGPWR | Terminate | Power failure warning. |
| 31 | SIGSYS | Core Dump | Bad system call (invalid syscall). |

### Signal Default Actions Explained
| Action | Meaning |
| - | - |
| Terminate | Process exits normally |
| Core Dump | Process terminates and creates memory dump file |
| Ignore | Process paused until `SIGCONT` |
| Stop | Signal ignored |

### Signals That Cannot Be Caught or Ignored
These signals **cannot be handled** by `signal()` or `sigaction()`.
* **`SIGKILL(9)`** : Force Kill
* **`SIGSTOP(19)`** : Stop process immediately