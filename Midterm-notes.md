# COMP3230 Midterm Review
Jiang Feiyu
## What is an Operation System
- primarily as a resource manager  
    - Managing resources – CPUs, memory, disks, files, . . .  
    - Decides between conflicting requests for efficient and fair use of resources
- An interface between applications and hardware
    - Separates applications from the hardware they access
    - OS provides the APIs (Application Program Interfaces) for programs to ask for OS services/resources
- Three pieces
    - Virtualization
    - Concurrency
    - Persistence
## Operation system structure
### User mode and Kernal mode
- User mode:
    - CPU is put in user mode when a user application is executing
    - Running in a lower privilege level
    - Ensures that one user program cannot execute instruction that may interfere with operation of other user programs
- Kernal mode: 
    - CPU is put in kernel mode when the OS kernel executes
    - It is at the **highest privilege** level and can execute **any instruction** the machine
### System calls
- When a system call occurs, the system switches from user mode to kernel mode and executes the corresponding kernal's system function.
### Monolithic Architecture 单体架构
- Every OS component is contained in the kernal
- 特点：
    - 非模块化结构 didn't structure its components
    - 集中运行 all componnents are interwoven into one large program that runs in privilege mode
    - 无障碍通信 any component can directlty communicate with any other
    - 共享数据结构 easily shared data structure
- Pros and Cons:
    - Pros: Highly efficient
    - Cons: Components can interact with hardware directly; Components can access each other’s data and functions directly
### Layered Architecture 分层架构
- 一种操作系统设计方法，它将操作系统的各个组件按照功能或特定角色分组，然后将这些模块层叠在一起，每一层只与其上下相邻的层进行通信
- 特点：
    - Each layer communicates only with layers immediately above and below it
    - Processes’ requests might pass through many layers before being serviced
    - System efficiency can be less than monolithic kernels
- Pros and Cons:
    - Pros: 1. Simplicity of construction and debugging; 2. Information hiding – each layer only knows the interface provided by immediate lower layer
    - Cons: 1. Poor performance; 2. Difficult to define and order layers; 3. Not flexible; 4. Disjunction between model and reality: real systems are more complex, difficult for such layered abstraction

### Microkernel Architecture 微内核架构
- Moves as many functionalities from the kernel space into user space processes (being called as servers)
<p align=center><img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/484331f6860b49b4990a3362cee83596~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=532&h=388&s=66522&e=png&b=fefdfd" alt="螢幕截圖 2023-10-29 下午12.43.09.png" width="50%" /></p>

- 特点：
    - Attempt to keep kernel small 
    - The system becomes more stable as only the bare essentials are running in kernel mode Easier to extend and port to other platforms
    - Easier to extend and port to other platforms
- Pros and Cons:
    - Pros: 1. Extensible, portable and scalable; 2. More secure & reliable (less code is running in kernel mode)
    - Cons: 1. Server processes (in user space) are interacting by means of message exchanges, which need the helps from kernel (由于通信必须通过内核，因此存在较大的性能开销，消息传递可能导致较高的延迟)
    - Most embedded systems adopt microkernel

### Modular Approach
- Most modern operating systems still adopt the monolithic architecture but implement kernel modules
- 特点：
    - Each core component is separated and implemented as module 
    - The whole kernel is a collection of modules
    - More efficient to communicate between modules as they are all in kernel
    - Allows certain features to be implemented dynamically and loaded as needed (dynamically loadable modules)
    - More extensible as kernel modules can be modified separately and new modules can be added easily
<p align=center><img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/65b480774d324ecaaf4d6b1331338910~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=550&h=381&s=53544&e=png&b=fdfbfb" alt="螢幕截圖 2023-10-29 下午12.54.14.png" width="50%" /></p>

#### Modern OSs tend to use the monolithic approach with layers and kernel modules for extensibility

#### Questions related
1. One key difference between traditional monolithic kernel and a modular kernel and one key difference between a modular kernel and a microkernel.

    Ans:
    1. An important difference between the traditional monolithic kernel and a modular kernel is that the modular kernel features dynamic loading of code into the kernel address space.  
    2. An important difference between a modular kernel and a microkernel is that the modular kernel still runs all of its code with kernel privileges, whereas the microkernel runs most of its services at user level.
2. Difference between kernel mode and user mode

    Ans: 
    1. In kernel mode, the software has complete control of the processor and all its instructions, registers, and memory. This level of control is not necessary and for safety is not desirable for user programs. This is the mode, in which the operating system executes with. 
    2. For user applications, they run in user mode, in which only a subset of the machine instructions is available and certain areas of memory are protected.
    
## Life cycle of a Process
<p align=center><img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/9d395e3e4819437b8d9000094223c627~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=677&h=319&s=175043&e=png&b=ffffff" alt="image.png"  width="100%"/></p>

### Valid state transitions
- **Ready -> Running**</br>There is an idle CPU, the scheduler selects a process and dispatches it to be executed. 
    - Case 1: the previous process has its quantum expired; scheduler dispatches the current process. 
    - Case 2: The previous process terminated; scheduler dispatches the current process.

    When a process is dispatched to be executed by the CPU, it transitions from ready to running. Scheduler: SJF, RR, STCF…

- **Running -> Ready**</br>The process’s time slice ran out; the scheduler preempts it and moves it to the ready queue.
    OS decides to switch a process from running to ready. 
    - Case 1: Current process exceeds the time limitation with time quantum will be de-scheduled. 
    - Case 2: If other processes have higher priority, the current process will be de-scheduled. 
    - Case 3: The process yields to give up CPU.
- **Running -> Blocked**</br>Any blocking action, e.g., I/O request. The scheduler puts the process in the blocked queue.
- **Blocked -> Ready**</br>I/O completion or lock is acquired, or the waiting condition has met. The scheduler puts the process in the ready queue.
    - Case 1: When the waiting event occurred, the blocked process transitions from blocked to ready. 
    - Case 2: When the required resources are ready.

- **Running -> Terminated**</br>With terminated state, it allows parent process to examine the return code of the process and see if it executed successfully. 
    - Case 1: A signal was sent to terminate the process. 
    - Case 2: The process completed its task and returned.

### PCB: Process Control Block
To manage a process, OS makes use of a data structure to maintain information about a process.PCB通常被称为"进程描述符"，其主要任务是存储与进程相关的关键信息，以便操作系统有效地管理进程的执行。以下是PCB通常包括的主要信息：

- Process identification number (PID) - a unique ID
- Program counter - indicates the address of next instruction
- Register Context: a snapshot of the register contents in which the process was last running before it transitioned out of the running state
- Process State
- Program counter - indicates the address of next instruction

### How to create and terminate a process
`ps`: show the Processes info
</br>`pstree`: show the relations between processes

- Actions taken 
    1. Assign a unique process ID
    2. Allocate memory for the process
- Initialize the process control block
    1. Save the process ID, parent ID
    2. Set program counter and stack pointer to appropriate values
- Set links so it is in the appropriate queue
- Create other data structures
- Set the process state to Ready and put it to the Ready queue
- Code:
    - `fork()`: A system call that creates a new process by duplicating the calling process
    - `exec()`: OS replaces the current program image with a new program image


### Child process and its' parent's process
The child process does not copy the parent process exactly. Specifically, although the child process has its own address space, registers, program counters, and so on, the value it returns from `fork ()` is different. The return value obtained by the parent is the PID of the newly created child process, and the return value obtained by the child process is 0. 

Therefore, parent and child processes in the system are deterministic. Assuming we are running on a system with a single CPU (for simplicity), either the child process or the parent process can run at this time. The child process will only enter the running state when it obtains CPU resources and starts executing.

### Time sharing

- What will happen if there is no time-sharing?

    When time-sharing is not supported by the operating system, this means that each process can continue running the program without interruption, indicating that neither the timer interrupt nor the OS scheduler will exist. As a result, Block status will no longer exists. 

    At this point, we still need Ready, Running, and Terminated states. The difference is that switching from Ready to Running no longer involves running out of time slices; instead, the process holds on to the CPU resource until it is complete, although it can still voluntarily release the CPU (Ready <-> Running). 

## Context Switch and Mode Switch
1. **Context Switch**: 
   - 当一个进程停滞或者恢复运行，操作系统需要从内核栈保存并恢复一些寄存器的值
   - 从一个进程向另一个进程切换
   - 时序共享程序的的特点，是虚拟化CPU的关键操作
   - 引入一定的性能开销
2. **Mode Switch**: 
   - 使用系统调用允许从用户模式调用内核函数，这些函数提供对操作系统功能的访问
   - 从User mode 向Kernel Mode进行切换
   - 内核被称为“代表进程执行”；内核充当中介，以其特权级别执行所需的操作
   - 内核可以访问进程的内存和状态，包括其地址空间
   - 退出内核后，进程可以继续并返回到用户空间执行
3. **关系**
    - 每个Context Switch都会包含Mode Switch;但并非每个Mode Switch都包含Context Swicth
    - 例子：
    
        | Operations | Context Switch | Mode Switch |
        | --- | --- |--- |
        |  Continuously call the same process    | X | √ |
        | I/O | X | √ |
        | Using fork() to create a child process | √ | √ |
        | Change from one process to the other | √ | √ |
        
4. 抢占与非抢占
    - **抢占式调度（Preemptive Scheduling）：** 在抢占式调度中，操作系统具有权力中断当前运行的进程，并将控制权交给另一个具备更高优先级的进程。The process may be interrupted by the OS scheduler at any time of its execution and be switched to ready state.如： RR, STCF(最短完成时间)
    - **非抢占式调度（Non-preemptive Scheduling）：** 在非抢占式调度中，进程只有在自愿放弃CPU控制权（例如，等待某个事件发生或系统调用完成）时，才会发生上下文切换。Once a process is dispatched to be executed by the processor, it runs till its completion or until it voluntarily relinquishes the processor.  如： FIFO, SJF（最短任务优先)
6. 注意
    - 计算mode switches的时候一般要乘2，一次mode switch伴随着两次system call
    - Each System Call has 2 Mode Switch, one trap (user mode to kernel mode) and one return-from-trap (kernel mode to user mode)
    - 几种情况会导致上下文切换，而不受调度策略（抢占式或非抢占式）的影响
        1. **Blocking system call（阻塞系统调用):** 当一个运行的进程执行了一个阻塞系统调用(例如等待I/O操作完成)，它将自愿放弃CPU
        2. **Servicing a disk interrupt(处理磁盘中断):** 当一个进程正在处理磁盘中断，而且另一个被标记为"就绪"的进程等待运行，操作系统会在中断处理完成后选择切换到等待的进程，以便它可以运行
       


## Scheduling
### Calculations
- Turnaround time := time from arrival to complete; 
- Wait time := time in waiting queue; 
- Response time := time from arrival to first execution.

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/56b2ed1e468d4b2ab65ce8895827ccbe~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=432&h=30&s=8846&e=png&a=1&b=000000" alt="image.png" width="100%" />
<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5e9c27fb5ec04bcd8aff77b16efe6cd2~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=432&h=60&s=12735&e=png&a=1&b=000000" alt="image.png" width="100%" />

### Round Robin (RR) scheduling policy 轮转调度
RR在一个时间片内运行一个工作，然后切换到运行队列中的下一个任务，而不是运行一个任务直到结束。它反复执行，直到所有任务完成。时间片长度对于 RR 是至关重要的。越短，RR在响应时间上表现越好。然而，时间片太短是有问题的:突然上下文切换的成本将影响整体性能。

### STCF 最短完成时间优先
向 SJF 添加抢占，每当新工作进入系统时，它就会确定剩余工作和新工作中， 谁的剩余时间最少，然后调度该工作。

<p align=left><img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/b4ee461ffa9d443593fddad2e2f2a331~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=238&h=111&s=8808&e=png&b=fafafa" alt="image.png" width="40%" /></p>

### SJF 最短任务优先
先运行最短的任务，然后是次短的任务，如此下去。

<p align=left><img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/3f4b22a6d44f4d3bb1de0255be6af101~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=508&h=135&s=16344&e=png&b=fcfcfc" alt="image.png" width="70%" /></p>

## Threads
- A sequence of instructions that performs a task (within the application process)
- Multithreaded process
    - A process has multiple threads of execution
    - A thread is an entity within a process
    - Multiple threads within a process  
        ¤ can execute concurrently  
        ¤ share the same address space and other global info
- 线程切换与进程切换相比，开销更小，因为线程共享相同的地址空间，而进程具有独立的地址空间，因此切换进程时需要更多的资源和时间；如果一个进程中的某个线程被阻塞，其他线程仍然可以继续运行。这有助于保持程序的响应性，因为不需要等待整个进程完成。
### Thread creation
```
#include <pthread.h> 
int pthread_create(pthread_t * thread, 
                const pthread_attr_t attr, 
                void *(*thrfunc)(void *), 
                void *args);
```
### Thread termination
```
#include <pthread.h>
                void pthread_exit (void *retval)
```
Any files opened inside the thread will remain open after the thread is terminated
### Thread join
“Joining” is one way for a thread (especially main thread) to wait for other threads to exit.The pthread_join() blocks the calling thread until the specific thread terminates.
<p align=center><img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5af43c7217a94255a69759191113bba4~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=806&h=321&s=44656&e=png&b=fefefe" alt="image.png" width="70%" /></p>

```
#include <pthread.h>
int pthread_join (pthread_t thread_id,
                  void ** value_ptr);
```

### Example
```
#include <stdio.h>
#include <pthread.h>
void *func1 (void *arg){
    int x = *((int*)arg);
    printf("The integer passed in is %d\n", x); 
    printf("Thread: Process id is %d\n", (int)getpid()); 
    pthread_exit(NULL);
}
int main() {
   pthread_t thread_id;
    int x = 1;
    printf("Main process: Process id is %d\n", (int)getpid());
    
    pthread_create(&thread_id, NULL, func1,(void*)&x); 
    pthread_join(thread_id, NULL);
    return 0;
}
```
## Concurrency
### Concurrency Issue
- The main issue is uncontrolled scheduling：When will a thread be executing is not predictable
- Race condition: several threads access and modify a shared data item concurrently and the outcome of the execution depends on the particular order in which the accesses took place
### Critical sections 临界区
- Blocks of code where a particular shared data is modified must be guarded
- We would like to have only one thread be in its critical section accessing the specific protected shared data at one time
- 在多线程程序中，许多代码可以同时执行，而不会产生问题，因为它们不涉及共享数据的访问和修改。这允许程序更高效地利用多核处理器。多线程程序中，只有在访问和修改共享数据时才需要特别的关注，因为这可能会导致竞争条件和数据不一致的问题。当多个线程需要访问和修改相同的共享数据时，需要确保只有一个线程能够同时访问，这就是临界区的概念。
### Mutual exclusion 互斥
- Only one thread is allowed access at one time  
- The winner thread has in-effect “locked” the shared data
- Others must wait until the shared data is unlocked
### Atomic Operation 原子操作
- "The operation cannot be interrupted in the middle"：原子操作是不可中断的，这意味着它要么被完全执行，要么根本不执行。这可以确保在多线程环境中，其他线程不能在原子操作进行过程中干扰或修改共享数据。
- “hardware can guarantee either has done or not done at all”
## Locks
### Evaluation:
- Correctness
- Fairness
- Performance
### Building locks
- Disabling interrupts
   
   ¤ Prevents current executing thread from being preempted, as without interrupts, scheduler will not be invoked; thus, no other threads will be able to turn to running state
## Atomic instruction
### Test-and-Set Instruction 测试并设置指令
- 检查某个内存位置或标志位的当前状态，然后设置该位置的值为一个新值。It enables us to test the old value while simultaneously set the variable to a new value
- The instruction returns the old value and simultaneously updates the value to new in an atomic way
- Use`Spin-wait / Busy-waiting` to test whether it can enter its critical section
    - However, the processor is essentially doing nothing but just executes the while() loop Wastes significant processor time 
    - those threads may just spin-waiting until the time quantum expires
    - Spin-waiting cannot avoid indefinite postponement; thus, do not provide fairness guarantee

<p align=center><img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/b4d5f787fe1b4f8c94f7fb8f70086815~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=541&h=206&s=25952&e=png&b=fefefe" alt="image.png" width="70%" /></p>

### Compare-and-Swap Instruction 比较并交换指令
- 比较内存位置的当前值与期望的值，然后在比较匹配的情况下，将该位置的值更改为新值。Test whether the content of a variable is equal to expected; if so, update the variable to a new value; otherwise, do nothing
- The instruction always returns the current value of the variable

<p align=center><img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/172d84b0c5d3405a8fd6ee36dc6c80c7~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=871&h=380&s=52545&e=png&b=fefefe" alt="image.png" width="100%" /></p>

```
int lock = 0; // 全局共享的锁，初始值为0

// 用于原子地执行 Compare-And-Swap 操作的函数 
bool CompareAndSwap(int *ptr, int expected, int new_value) { 
    int current = *ptr; // 获取当前值 
    if (current == expected) { 
        *ptr = new_value; // 如果当前值等于期望值，将锁设置为新值 
        return true; // 返回 true 表示操作成功 
       } else { 
       return false; // 返回 false 表示操作失败 
    } 
}

// 尝试获取锁 
while (!CompareAndSwap(&lock, 0, 1)) { 
    // 如果锁获取失败，线程会不断尝试直到成功 
}
```
### Fetch-And-Add 获取并增加指令
- 用于从内存位置读取当前值，然后增加该值，并将结果写回该位置。
- Ticket Lock
    - ticket: the calling thread’s turn; 每个线程在尝试获取锁之前都会获取一个唯一的票号。票号通常是一个整数，按顺序递增，表示线程进入临界区的顺序。每个线程将自己的票号存储在一个变量中。
    - turn: determine which thread’s turn it is; Turn 变量用于确定哪个线程当前有权进入临界区。通常，Turn 的值与当前持有锁的线程所拥有的票号相匹配。当一个线程获得锁时，Turn 的值会递增，使下一个线程有机会进入。
- Fairness: Ensures progress for all threads
    - Every thread attempting to acquire the lock with be assigned a ticket value, and will be scheduled at some point in the future

<p align=center><img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/2d5cd4190d8e4ce38a607bae5514408b~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=517&h=155&s=19669&e=png&b=fefefe" alt="image.png" width="70%" /></p>

```
int shared_variable = 0; // 共享的整数变量 
void *thread_function(void *id) { 
    int thread_id = (int)id; 
    
    // 从内存位置读取当前值并增加 
    int current_value = __sync_fetch_and_add(&shared_variable, 1); 
    printf("Thread %d: Current value is %d\n", thread_id, current_value); 
    
    // 执行一些操作 usleep(100000); 
    usleep(100000); // 模拟临界区内的工作 
    
    printf("Thread %d: Updated value is %d\n", thread_id, shared_variable); 
    pthread_exit(NULL); 
    } 
    
int main() { 
    pthread_t threads[2]; 
    int i; 
    for (i = 0; i < 2; i++) { 
        pthread_create(&threads[i], NULL, thread_function, (void *)i); 
        } 
    for (i = 0; i < 2; i++) { 
        pthread_join(threads[i], NULL); 
        } 
    return 0; 
    }
```
### How to avoid Spinning?
- Performance: Spin-wait does nothing but waste CPU time
- OS Support:
    - Yield to give up CPU: `yield()` moves the caller from running to ready
    - Use queues to sleep and wait instead of spinning: `park()` to put the calling thread to sleep; `unpark()` to wake up a thread
- Two-phase lock: Spin-wait once (or for a fixed amount of time) and go to sleep if not getting the lock

## Control Synchronization
一种简单的方法是线程A不断自旋，等待条件成为真，但这种方式效率低下，浪费CPU周期。

为了避免这种浪费，可以使用一种专门设计用于支持线程之间同步的数据类型，它包含一个队列，线程可以在那里等待某个条件或事件的发生。

当某个其他线程检测到条件或事件发生时，它可以通知（信号）那些正在等待的线程，并唤醒其中的一个线程。

使用条件变量时，每个条件变量通常只能与一个特定的条件或事件相关联。

<p align=center><img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/036fcbac5e1e4eefbf5f058c1091fcc8~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=909&h=347&s=71137&e=png&b=fefefe" alt="image.png" width="70%" /></p>

### Producer-consumer problem
- Synchronization requirements
    - Buffer pool is shared resource, producers and consumers need to use some method to coordinate the access to the pool
    - A producer must not overwrite a buffer when buffer pool is full
    - A consumer must not consume an empty buffer when buffer pool is empty
    - Mutual exclusion
    - Information must be consumed in FIFO order


<p align=center><img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/20796abb5c804ce5b140647949ba3c10~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1055&h=565&s=233807&e=png&b=ffffff" alt="image.png" width="100%" /></p>
<p align=center><img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/d4d1096f81574d1c9647fda82667d5ec~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1133&h=519&s=200237&e=png&b=ffffff" alt="image.png" width="100%" /></p>

### Semaphores 信号量
- 信号量是同步工具：信号量是一种用于控制多个线程之间同步操作的工具。它可以用于实现互斥（保护共享资源不被多个线程同时访问）以及线程之间的协调。
- 信号量的内部状态：信号量是一个对象，它内部包含一个整数变量，该变量用于控制同步。这个整数变量的范围和值决定了信号量的行为。
- 初始化信号量：为了使用信号量，你需要初始化它。你可以使用 sem_init 函数进行初始化，例如 sem_init(&k, 0, x)，其中 k 是信号量对象，x 是初始值。初始值通常用于表示信号量的资源数量或可用许可数。
- 信号量的原子操作：信号量内部的整数值只能通过两种原子操作来访问
- `sem_wait(&k)`，也称为 P()、wait 或 down：当线程调用 sem_wait 时，它会尝试获取信号量，如果信号量的值不大于0，线程将被阻塞，直到信号量的值大于0。
- `sem_post(&k)`，也称为 V()、signal 或 up：当线程调用 sem_post 时，它会增加信号量的值，如果有等待的线程，其中一个等待的线程将被唤醒。

条件变量和信号量：
- **Q**: In multithread programming, there are many cases where a thread wishes to wait for a condition to be true before continuing its execution. You have learned that both the semaphores and condition variables can be used for handling this kind of synchronization. However, there is a subtle difference between the two tools. Explain the key difference in the runtime behavior of the two tools.

- **Ans**: In the case of condition variable, when the cond_signal() operation is performed and if there are no waiting processes, the signal is simply ignored and the system does not remember the fact that the signal took place. If a subsequent cond_wait() operation is performed, then the corresponding thread simply blocks.

    In semaphores, on the other hand, every sem_post() results in a corresponding increment of the semaphore value even if there is not any process waiting. A future sem_wait() operation would immediately succeed because of the earlier increment.
