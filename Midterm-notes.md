# COMP3230 Midterm Review
Jiang Feiyu
## 操作系统内核构架
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

### 微内核架构 Microkernel Architecture
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

#### Questions related: 
1. One key difference between traditional monolithic kernel and a modular kernel and one key difference between a modular kernel and a microkernel.

    Ans:
    1. An important difference between the traditional monolithic kernel and a modular kernel is that the modular kernel features dynamic loading of code into the kernel address space.  
    2. An important difference between a modular kernel and a microkernel is that the modular kernel still runs all of its code with kernel privileges, whereas the microkernel runs most of its services at user level.
2. Difference between kernel mode and user mode

    Ans: 
    1. In kernel mode, the software has complete control of the processor and all its instructions, registers, and memory. This level of control is not necessary and for safety is not desirable for user programs. This is the mode, in which the operating system executes with. 
    2. For user applications, they run in user mode, in which only a subset of the machine instructions is available and certain areas of memory are protected.
    
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
    - **抢占式调度（Preemptive Scheduling）：** 在抢占式调度中，操作系统具有权力中断当前运行的进程，并将控制权交给另一个具备更高优先级的进程
    - **非抢占式调度（Non-preemptive Scheduling）：** 在非抢占式调度中，进程只有在自愿放弃CPU控制权（例如，等待某个事件发生或系统调用完成）时，才会发生上下文切换
6. 注意
    - 计算mode switches的时候一般要乘2，一次mode switch伴随着两次system call
    - 几种情况会导致上下文切换，而不受调度策略（抢占式或非抢占式）的影响
        1. **Blocking system call（阻塞系统调用):** 当一个运行的进程执行了一个阻塞系统调用(例如等待I/O操作完成)，它将自愿放弃CPU
        2. **Servicing a disk interrupt(处理磁盘中断):** 当一个进程正在处理磁盘中断，而且另一个被标记为"就绪"的进程等待运行，操作系统会在中断处理完成后选择切换到等待的进程，以便它可以运行
       
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

### Child process and its' parent's process
The child process does not copy the parent process exactly. Specifically, although the child process has its own address space, registers, program counters, and so on, the value it returns from fork () is different. The return value obtained by the parent is the PID of the newly created child process, and the return value obtained by the child process is 0. Therefore, parent and child processes in the system are deterministic. Assuming we are running on a system with a single CPU (for simplicity), either the child process or the parent process can run at this time. The child process will only enter the running state when it obtains CPU resources and starts executing.

### Time sharing

- What will happen if there is no time-sharing?

    When time-sharing is not supported by the operating system, this means that each process can continue running the program without interruption, indicating that neither the timer interrupt nor the OS scheduler will exist. As a result, Block status will no longer exists. 

    At this point, we still need Ready, Running, and Terminated states. The difference is that switching from Ready to Running no longer involves running out of time slices; instead, the process holds on to the CPU resource until it is complete, although it can still voluntarily release the CPU (Ready <-> Running). 
