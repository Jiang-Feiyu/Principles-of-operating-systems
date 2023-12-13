<p align=center>COMP3230 Principles of Operating Systems</p>
<p align=center>Bob Jiang</p>
<p align=center>Date: December 15, 2021</p>

<p align=center>如答案有不妥之处，欢迎批评指正</p>
<p align=center>If there are any inaccuracies in the answers, criticisms and corrections are welcome.</p>

</br>
*For more about the HKU COMP3230 Course Material, please refer to Github[1]*

*This is not a standard & official answer, just for self-review purposes*
个人复习用途，非官方答案

*试卷来源 SOURCE：`HKU library`[2]*

<div STYLE="page-break-after: always;"></div>

# Q1 — Multiple Choices Questions
<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5ba7885964f646c6bb0cf97770c1e5f3~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1562&h=380&s=284787&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-07 下午12.10.26.png" width="100%" />

Ans: ABDE, for question C:
`To be precise, it depends on whether or not the OS schedules another process to run after terminating the current process. If yes, then there is certainly a context switch; otherwise, it is only a "partial context switch".`
- Context Switch是一个更加接近硬件层面（寄存器层面有没有发生切换）的概念，而Mode Switch是操作系统层面的概念
- System Call属于软件中断, 有Interrupt就一定会发生从用户进程到内核进程的切换
- 用户态和内核态切换一定会发生Context Swicth
- 无论是用户进程和用户进程之间切换还是用户进程和内核进程之间切换，都会发生Context Switch

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c12f7ace7a0d4ba2bafeaf674c1e025b~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1678&h=852&s=781075&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-07 下午1.05.21.png" width="100%" />

Ans:BDE
- 线程共享堆区、代码区和数据区，全局变量是共有的；栈区维护的是局部变量，是线程独有的
- 线程A调用signal后陷入到内核态，而后不会返回线程A的用户态，而是根据调度算法确定下一个要被调度的线程；因此B不一定立马调度到等待在那个条件变量上的线程
- C:加锁不代表发生中断；锁通常是在用户空间实现的同步机制，而中断和上下文切换是由操作系统内核管理的
- 对POSIX而言，`trap`就是一种中断，有锁保护的时候就会去获取锁，首先会发生中断到内核态，如果锁有人用了，那么线程就会进入阻塞态再重新调度；没有锁的话，就直接访问，不会陷入内核态

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/ca2338ec15a24ade8220fee519755bbc~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1520&h=410&s=279026&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-10 下午9.22.59.png" width="100%" />

Ans: ABC
- Dynamic relocation是指在程序运行时，操作系统能够将程序中的某些地址（通常是指针或跳转指令中的地址）映射到实际内存地址的过程；分段是指将程序的内存空间划分为不同的逻辑段，每个段具有不同的属性和权限。这些段包括代码段、数据段、堆、栈等。分段是一种在程序执行期间为不同类型的数据提供不同保护级别和访问权限的机制。
- 分段解决了一些问题，帮助我们实现了更高效的虚拟内存。不只是动态重定位，通过避免地址空间的逻辑段之间的大量潜在的内存浪费，分段能更好地支持稀疏地址空间。**它还很快**，因为分段要求的算法很容易，**很适合硬件完成**，地址转换的开销极小。分段还有一个附加的好处:**代码共享**。如果代码放在独立的段中，这样的段就可能被多个运行的程序共享。
- **内部碎片**指的是分段中的一个段（segment）内部出现的未被充分利用的空间。这种情况发生在一个段所分配的内存空间超过了该段所实际需要的内存空间。例如，如果一个段被分配了100个单位的内存，但实际只需要80个单位，那么这个段内部就有20个单位的内部碎片；**外部碎片**是指未分配的零散内存块，这些内存块的总和足够满足一个段的内存需求，但由于它们分散在内存中，因此无法被有效利用。外部碎片的形成主要是由于先前分配的段被释放，但由于内存空间不连续，无法被新的段使用。
- 外部碎片是操作系统导致的，内部碎片是进程导致的；**分段存储会产生外存碎片、不会产生内存碎片**

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/e5a042366cbe4aecb213663c15a6af4f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1568&h=360&s=327303&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-11 上午10.36.28.png" width="100%" />

Ans: B
- TLB misses表示在TLB中没有找到所需的映射，需要进行内存页表的查找，这可能导致较长的内存访问延迟
- 如果某个进程不发生TLB misses，会一直占用进程；不miss的情况实际上更多

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/2930bcabee7043a6b95f3b43403adc53~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1558&h=732&s=488217&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午12.54.24.png" width="100%" />

Ans: ABC
- Virtual memory和Physical memory不同。128bytes的virtual memory对应512Bytes的physical，Bounds是偏移量

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/989bedea3db642ae9839bde4a032ed07~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1198&h=1216&s=308043&e=png&b=f9f9f9" alt="螢幕截圖 2023-12-11 下午1.00.17.png" width="70%" />

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/67165c2fbe6c4b8b92cde98c999bd175~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1536&h=636&s=499389&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-10 上午10.53.41.png" width="100%" />

Ans: AD
- Key of solving this question is to detect whether the deadlock meets 4 requirements: 
    - Mutual exclusion condition 使用互斥锁--> 不用互斥锁 
    - Wait-for condition (hold-and-wait condition) 可以申请一部分资源，再申请另一部分资源--> 申请所有的资源都是原子操作（一次性可以申请所有资源）
    - No-preemption condition --> 可以强行回收资源 
    - Circular-wait condition --> 更改顺序，如：银行家算法 如果每把锁按照固定顺序，就不会发生死锁; 圈一样的循环等待
- Thread1->1, Thread2->2 
- Thread1->1, Thread2->3, Thread3->2

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/2abb7578e6d647bf8dff8c232adedd11~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1538&h=1380&s=634742&e=png&b=fefefe" alt="螢幕截圖 2023-12-10 下午2.05.50.png" width="100%" />

Ans: ADEF
- G: 只有P操作才会使线程进入阻塞, `sem_post()`是V操作，用于增加信号量的值；`sem_wait()`是P操作，用于减少信号量的值

```
semaphore mutex=1; //临界区互斥信号量
semaphore empty=n;  //空闲缓冲区
semaphore full=0;  //缓冲区初始化为空
producer ()//生产者进程 
{
    while(1)
    {
        produce an item in nextp;  //生产数据
        P(empty);  //获取空缓冲区单元
        P(mutex);  //进入临界区.
        add nextp to buffer;  //将数据放入缓冲区
        V(mutex);  //离开临界区,释放互斥信号量
        V(full);  //满缓冲区数加1
    }
}

consumer ()//消费者进程
{
    while(1)
    {
        P(full);  //获取满缓冲区单元
        P(mutex);  // 进入临界区
        remove an item from buffer;  //从缓冲区中取出数据
        V (mutex);  //离开临界区，释放互斥信号量
        V (empty) ;  //空缓冲区数加1
        consume the item;  //消费数据
    }
}
```


<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/bed62f2617024ebfa15327adae11cf9f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1608&h=426&s=401470&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-10 下午1.12.05.png" width="100%" />

Ans: ABC
- Limitation of hard link 
    - Can’t create a hard link to directory (软连接也不可以)
    - Can’t create hard link to files in other disk partitions (i.e. another file system) (但是软连接可以)
    - read文件的时候可能会储存在缓存中，则不需要block进程
    - file descriptor和inode类似，就是一个数字，不是数据结构



<div STYLE="page-break-after: always;"></div>

# Q2 Scheduling
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/3a1f7c3863e14d9bbe705f1aa8f4e53f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1744&h=460&s=380371&e=png&b=fdfdfd" alt="image.png" width="100%" />

FIFO
<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/f3b89108a5e94ae68d73961027a66272~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1655&h=882&s=174356&e=jpg&b=ffffff" alt="IMG_0968.jpg" width="100%" />

RR with 1 ms time slice
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c160a69e2e3440ce8813a722be131e43~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1241&h=2066&s=694954&e=jpg&b=ffffff" alt="IMG_D9D5418382DF-1.jpeg" width="80%" />

RR with 100ms time slice
<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/0216f971c4964dc9b0fe37072e8a156a~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2133&h=1062&s=329655&e=jpg&b=ffffff" alt="IMG_7AF1B604D804-1.jpeg" width="80%" />

Multilevel feedback queue(MLFQ) with 4 levels.The time slice for the highest priority level is 1 ms, and it doubles for each lower-priority level, i.e., 2ms, 4ms, and 8ms for the other three levels respectively. Assume that priority boosting is disabled.
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/48f0dfcd8fee405c97f907edc7b11e47~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1446&h=340&s=163902&e=png&b=8cc674" alt="螢幕截圖 2023-12-10 上午10.02.50.png" width="100%" />

针对规则4的解释：`whether B uses up the time slice within a continuous 4 ms when being scheduled`需要连续用完时间片才会降级

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c897e7e9a6684254b65294228aaa80d6~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1562&h=1629&s=731139&e=jpg&b=fefefe" alt="IMG_0974.jpg" width="90%" />

<div STYLE="page-break-after: always;"></div>

# Q3 Concurrency
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/acc935951dcb4132ae5226741554e24e~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1202&h=1018&s=700651&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-07 上午9.08.49.png" width="100%" />

**确定哪里是全局变量，哪里可能发生竞态**
</br>在这道题里， Buy这个操作转换成汇编语言需要多步进行，比如，他需要把coffee储存在寄存器中再进行比较，这样子就容易被打断进一步产生问题，因此，最好的解决方案是强行规定顺序，让一个线程先开始，一个线程进行自旋等待。

在这里，我们令A先去拿咖啡，则

Thread A
```
if (noteA == 1 && coffee == 0) {
    buy;
}

noteA = 0;
```

Thread B
```
while (noteA == 1) ;

if (noteB == 1 && coffee == 0) {
    buy();
}

noteB = 0:
```

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/91c803dcf0054d00b0f9648d8e0b5171~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1844&h=396&s=75063&e=png&b=ffffff" alt="螢幕截圖 2023-12-07 上午10.57.25.png" width="100%" />

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/a6bb99afee3e4bbfb564bc8608d4088c~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1614&h=282&s=290735&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-07 上午11.02.25.png" width="100%" />

```
Pthread_mutex_lock(&mutex);

if (coffee = 0){
    Buy ();
}

Pthread_mutex_unlock(&mutex);
```
    
<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/63d4f64d4c8e410caf6207fa8e5d7c85~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1426&h=1436&s=1046988&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-07 下午1.57.49.png" width="100%" />


<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/a84fd0f376ef4de3acba0b3e4118d769~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=948&h=908&s=78681&e=png&b=f8f8f8" alt="image.png" width="70%" />

<div STYLE="page-break-after: always;"></div>

# Q4 Paging

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/e6460b19b4ab458ab90b9d1d1150d454~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1570&h=510&s=378911&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午2.18.36.png" width="100%" />

a)

$size\ of(Page\ table) = size\ of(PTE) \times no.\ of(PTE)$

$no.\ of(PTE) = 2^{52-12} = 2^{40}$ 

$ size\ of(PTE) = 40-12+10 \approx 64 bit = 8B$

$size\ of(Page\ table) = 2^{43}B $

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/6d28d63235e443569408e2b084e0df8f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=608&h=512&s=63654&e=png&b=fafafa" alt="螢幕截圖 2023-12-11 下午2.29.59.png" width="30%" />

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/e58ddcecc38f4011aeb8c8e2baca09d0~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1312&h=202&s=61986&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午2.37.55.png" width="50%" />

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/0d25abd803a54cbd85d58de43c9f7bf4~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1246&h=812&s=298405&e=png&b=ffffff" alt="螢幕截圖 2023-12-11 下午4.15.53.png" width="70%" />

According to the question, the virtual address is `52-bit` and physical address is `40-bit`. The page size is `4KiB` and this `single-level paging scheme` needs the entry size to be in a unit of bytes and is a power of `2`.

$Space\ of\ page\ table = 2^{PFN+偏移量}$

`页表是一种将虚拟地址映射到物理地址的数据结构。操作系统通过虚拟页号（VPN）检索数组，并在该索引处查找页表项（PTE），以便找到期望的物理帧号（PFN）`

- $size\ of(Page\ table) = size\ of(PTE) \times no.\ of(PTE)$, PTE是`Page table entry`, 偏移量（页面大小）在物理内存和虚拟内存中都相等

- $no.\ of(PTE)$只与$VPN$有关，$size\ of(PTE)$与$PFN$（有多少个frame）有关

- $PFN$是offset的位数，等于$物理内存位数-offset位数$，$offset位数$只和页面大小有关

- 注意读题，如控制位和对齐要求

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/03c6a380cf9146488695abe613f4c602~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1498&h=128&s=83114&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午4.50.01.png" width="100%" />

b)

$size\ of(Page\ table) = size\ of(PTE) \times no.\ of(PTE)$
</br>$size\ of(PTE) = 40-30+10\approx32\ bit$
</br>$no.\ of(PTE) = 2^{52-30} = 2^{22}$

Thus, $size\ of(Page\ table) = 2^{27} bits=16MB$

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/4a8f9688e3be4153aac6ecf1666baa0f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1502&h=228&s=179380&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午4.55.39.png" width="100%" />

c)

假设：
-   Each page directory entry consists of a valid bit and a page frame number (which is the physical location of a valid smaller page table) （Page table/directory和Page size一样）表的大小一样
-   **构建多级页表的目标:使页表的每一部分都能放入一个页**
-   Page directory Entry和Page Table Entry大小相同（页表项和页目录大小相同）

思路:
- 假设不进行分级，那么这个page的大小为: $256KiB=2^{18}Byte$，那么他的`page table`的大小为$2^{52-18}\times(40-18+10)=64GB$, 物理内存为$2^{40}B=1024GB$
- 由此可知，页表有$2^{52-18}=2^{34}$个页表项,因此, 每个页表项大小为$\frac{2^{39}}{2^{34}}=2^{5}bit=4B$
- 对于一个256KB的页表（节点），它能存储$\frac{2^{18}}{2^{2}}=2^{16}$个页表项
- 因为每个页最多能存储$2^{16}$个页表项，总共有$2^{34}$个页表项，因此最多储存$2^{18}$个叶子节点
- 对于上述叶子节点的上一级，由于其表的大小和表的项大小相同，因此，储存这$2^{18}$个叶子节点需要$\frac{2^{18}}{2^{16}}$个页表，也就是需要4个上级页表
- 这4个上级页表需要一个顶级目录管理
- 因此，这是一个三级文件结构

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/922518a292254fbcb743a037744b3cd6~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1514&h=366&s=321299&e=png&b=fcfcfc" alt="image.png" width="100%" />

d)
- Don't use TLB: 没有缓存
- `VM address` -> `TOP顶级页表`(Physical address) -> `next directory` -> `directory` -> `required page` -> `content`
- $5\times80=400ns$

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/7645426df6ca46948fc9b8064ccd7f49~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1470&h=256&s=191073&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-13 上午10.30.50.png" width="100%" />

e)
- 由上面的分析可知,要真正获取一个内存中的内容实际需要加载两次内存: 一次是读取页表,一次是根据页表找到对应内存,因此可以进行时间上的优化;

- 将最常访问的几个(一般8-128个左右)页表项储存到访问速度更快的硬件中,一般是在MMU(内存管理单元),这个小表的名称为*TLB(Translation Lookaside Buffer)*,可以称其为快表;

- 在寻址时，会先查询TLB,在miss后再查PT; 快表的命中率很高，这是由于程序的局部性原理,并且程序最常访问的页没几个;

- 缓存的是从虚拟内存地址到物理内存地址的映射，读取一个即可：`5ns`

<div STYLE="page-break-after: always;"></div>

# Q5 Locality
数据访问集中化
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/8b2ed1718c514700b48f783d773ac4e0~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=753&h=291&s=104241&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-12 上午9.39.47.png" width="100%" />

- Fault Rate(故障率): 发生在程序试图访问虚拟内存中未加载到物理内存（RAM）中的页面时,触发中断的次数，越少越好
- Instruction Rate(指令速率): 每秒执行多少个指令，希望越多越好

Fault Rate越低越好， Instruction Rate越高越好：综上，我们认为比较优秀的方案是`2048`和`4096`

由题目可知，随着Page Size的增加， Fault Rate先减小后增加， Instruction Rate先增加后减少.
- 当Page Size过大时：
    - 页面占用内存过多，导致内存可用空间变少，故障率增加；因为中断消耗CPU时间，所以中断越多，指令速率越低
- 当Page Size过小时：
    - 页面过小时，会频繁发生swap导致的故障；同上

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/255a6ac5c49f4b31b3b0c8238836f400~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=609&h=106&s=47267&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-12 下午1.00.22.png" width="100%" />

Ans: 
</br>i) Page Fault应该相对较低，因为大部分Page都已经加载到了内存里面去。
</br>ii) 由题意，它说了是Working set of M pages,所以我们认为都是活跃的page，也就意味着,最坏的情况下User Bit都为1（用了就为1），需要遍历一圈才能够找到一个User Bit为0的进行驱逐；最好的情况下，是找一次就找到了User Bit为0的进行驱逐，因此可以认为$Avg\ Steps=\frac{M}{2}$

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/eea3d99709724d02b6a6033c612db0db~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=774&h=313&s=109696&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-12 下午5.34.40.png" width="100%" />
由题意，the physical memory has 4 page frames，有四个物理帧

i) ALRU 
- Assumption 驱逐策略: When all use bits set to 1, still use `FIFO` policy. 我假设当use bit都为1的时候，按照FIFO的规则进行驱逐，因此选择驱逐4号page.

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/3543b485f1d9450b8cb94e46ce1d3d56~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2360&h=1138&s=1162795&e=jpg&b=ffffff" alt="IMG_0887B3F67097-1.jpeg" width="100%" />


ii) LFU
<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/296e9daad6de4f31808c351b685d9544~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1595&h=680&s=437420&e=jpg&b=fefefe" alt="IMG_EA4492FF0AB9-1.jpeg" width="100%" />

The hit rate is $\frac{6}{20}=0.3$

iii) FIFO
<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/40b1d82d917c4f60bb264bd17e707f9f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2022&h=855&s=563268&e=jpg&b=fefefe" alt="IMG_0975.jpg" width="100%" />

The hit rate is $\frac{5}{20}=0.25$

iv)Clock

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/d0a55724b5b34a2fa48fed9b1465375e~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2328&h=1064&s=1048747&e=jpg&b=fefefe" alt="IMG_3C20DD57FE6B-1.jpeg" width="100%" />

The hit rate is $\frac{4}{20}=0.2$

<div STYLE="page-break-after: always;"></div>

# Q6
<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/be952899f63f47bc90ad9b7ca31a6161~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1554&h=446&s=399969&e=png&b=fcfcfc" alt="螢幕截圖 2023-12-11 下午3.07.49.png" width="100%" />

Assumption: 有10个direct pointer

由题意，系统中只有Super Block的信息被load到了内存中，这意味着其他所有信息都要通过I/O获得。同时Assume利用System memory(内存)去做缓存。

Assume the 3rd block data can be access by direct pointer. `root -> home -> c3230 -> bin -> monitor -> root -> usr -> bin -> monitor`. Since root have been duplicate access and access to each node need 2 times of disk block accesses.(root已经加载到内存中不需要重复加载)monitor中的指针Access 3rd Block的Inode，继续读取Block的内容，因为Inode不储存data，所以还要再access磁盘，因此monitor需要再读取两次，因此答案应该是2+2+2+2+2+2+2+2=16次


*(由图可知，读取第三个Block需要先从Inode找到Direct Pointer第三个指针，再进行读取；一次是读取指针指向的磁盘)*

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/7c9f091644d34b8c890a38ad52f6b325~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=751&h=97&s=38687&e=png&b=fdfdfd" alt="image.png" width="100%" />

同上，但是不需要再访问`root -> usr -> bin -> monitor`，因此可以省略6次，总计10次。

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/16c319a34f5b47e1bb117f65e3c5d853~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=791&h=212&s=89220&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-11 下午11.05.15.png" width="100%" />

- 随机读取: 编译器不可预测要访问哪个位置的变量，随机指定位置
    - Ans: multi-level更快
- 顺序读取：依次遍历数组/链表中的所有元素
    - Ans: 二者差不多
    
<div STYLE="page-break-after: always;"></div>

# 附录：知识点复习
#### 虚拟内存知识点复习
- 虚拟内存三大实现机制:
    - Page and frame 分页
    - Swap 交换
    - Page Fault 缺页异常
    
<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/75d6916d8c9b4e5fa39155e6a31923db~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=772&h=433&s=128813&e=png&b=403d4d" alt="螢幕截圖 2023-12-12 上午12.53.32.png" width="70%" />

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c4a8095838834fbfaf29f780171e9bee~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=756&h=491&s=66620&e=png&b=403d4d" alt="螢幕截圖 2023-12-12 下午1.22.59.png" width="70%" />

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/01a3bdff734e48058d815585569e74d9~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=703&h=513&s=142551&e=png&b=403d4d" alt="螢幕截圖 2023-12-12 下午1.23.47.png" width="70%" />

##### Process Address Space
- 每一个process有自己的地址空间
 
    <img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/a9356332e2c5404c8051d905f75c2366~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=170&h=219&s=19143&e=png&b=fdfafa" alt="螢幕截圖 2023-12-12 下午1.26.15.png" width="20%" />
    
    <img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/af91caf41b774334a57f6bf86739540d~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=235&h=330&s=40149&e=png&b=fdede2" alt="螢幕截圖 2023-12-12 下午1.30.31.png" width="20%" />
- 单位转换：
    - $1 Byte = 8 bit$
    - $KiBytes = KiB = 2^{10} bytes = 1024 bytes $
    - $MiBytes = MiB = 2^{20} bytes = 1048576 bytes $
    - $GiBytes = GiB = 2^{30} bytes $
- Responsibility of OS（软件）
    - Setup MMU
    - keep track of utilization of physical memory
- 三个假设
    - Assumption 1
        - Assume the address space of a process is relatively small, i.e., size of physical memory is much larger than a process's address space.
    - Assumption 2
        - Assume the process's address space must be placed contiguously in physical memory.
    - Assumption 3
        - Assume that the address space of every process is exactly the same size.
- Dynamic Relocation
    - Dynamic relocation是指在程序运行时，操作系统能够将程序中的某些地址（通常是指针或跳转指令中的地址）映射到实际内存地址的过程；分段是指将程序的内存空间划分为不同的逻辑段，每个段具有不同的属性和权限。这些段包括代码段、数据段、堆、栈等。分段是一种在程序执行期间为不同类型的数据提供不同保护级别和访问权限的机制。

##### Segmentation Paging

##### Improve Paging
<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/59efa715a058469fba11bcddb765583d~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=848&h=369&s=109568&e=png&b=fefcfc" alt="螢幕截圖 2023-12-12 下午1.39.14.png" width="80%" />

##### Find Free Page Frame
<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/d5c631551352471183491d857fd1a123~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2006&h=872&s=171392&e=jpg&b=ffffff" alt="image.png" width="80%" />

*CLOCK页面驱逐策略: 假设操作系统有一个进程，有M个页面，同时这M个Page已经被全部分配到了物理frame上; 循环中找找User Bit为0的页面进行驱逐*

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/2e304c79c590458abae1029597d5c02f~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1061&h=566&s=103634&e=png&b=fefefe" alt="螢幕截圖 2023-12-12 下午5.35.50.png" width="80%" />

<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/4fd64502795e4fc78019d5bb6f607244~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1086&h=609&s=86768&e=png&b=fefefe" alt="螢幕截圖 2023-12-12 下午5.36.04.png" width="80%" />

LRU和LFU的区别是，LRU每次找距离当前位置最远的驱逐，LFU每次找出现次数最小的进行驱逐（同样次数按照FIFO）

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/9f07af867b4246338e9f444efe13f55b~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1079&h=572&s=130730&e=png&b=fefefe" alt="螢幕截圖 2023-12-12 下午5.47.09.png" width="80%" />

-   近似LRU算法采用一种更简化的方法来估计页面的使用情况，以减少开销。
-   使用一个被称为"use bit"的标志位来表示一个页面是否最近被访问。当一个页面被加载到内存时，或者当再次引用该页面时，硬件会将该页面的use bit设置为1。
-   为了清除use bit，系统会定期将所有页面的use bit重置为0。这样，不活跃的页面的use bit将会被清零。
-   近似LRU的思想是，被频繁使用的页面在不久的将来可能会再次被访问，因此只关注最近的访问情况。

<div STYLE="page-break-after: always;"></div>

#### File system 相关知识复习
<img src="https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/b5ac54958b304a1db65ffd841f6b51ea~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1079&h=508&s=113831&e=png&b=ffffff" alt="image.png" width="80%" />

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/de76e08f7fd94e92bb3b4fe1d1ce11e1~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=163&h=546&s=27881&e=png&b=fefefe" alt="image.png" width="15%" />

##### Files and Directories
- 机械硬盘HDD最小存储单位Sector(`512Byte`)，读取单位logical Block(`4KB`，不一定相连)；SSD是晶体管储存的page为最小单位读写，但是读取整个Block，擦除也是擦除整个block（效率相对较低）
- SSD：存储读写效率高；HDD存储稳定，容量大，成本低 
    - Random I/O: SSD >> HDD
    - Sequential I/O: SSD > HDD
- 文件的永久储存: Files and directories
    - 每一个文件都有一个Inode Number, Inode是一个数据结构（文件的Meta Data）
    - Directory本质也是一个文件： `file name + Low level id(inode)`
- 文件系统:
    - File integrity, Security, file management, storage management...
- 文件的抽象:
    - 多个File descriptors对应一个Inode
    - Inode的数据存储在磁盘中
- Hard link: 硬连接
    - 创建多个文件路径都对应同一个inode 
- Symbolic link: 软连接-快捷方式 
    - create another file that contains the pathname of original file as its data, 新创建一个文件，文件内容是路径；删除软连接不影响inode中硬连接的数量 

##### File system implementation
- Super block中储存基本磁盘信息: 
    - 文件系统格式
    - No. of inode available 
    - where is the inode table(就一块，在磁盘里，每一个单元是inode的具体信息): 存储inode data 
    - where is the inode bitmap: 一串二进制数字，一个文件被创建/可用就生成1，一个文件被删除就改成0 
    - inode of root directory: 告诉根目录位置，就可以按照树形结构去寻找了
- Inode:
    - 要通过Inode找到存储在磁盘上的文件，inode needs to have some way to tell us **which data blocks** are associated to this file in the disk, 这会影响文件在系统中的最大大小；**指针越多，可存储文件越大**
    - Inode中的指针类型
        - direct(指向实际数据块)
        - indirect(指向block)
        - double indirect pointer...
    - Disadvantage
        - Used up some space in the inode to store the pointers 导致Inode结构体大
        - Disk blocks used by a file may be scattered all over the disk 分散而不是连续存的
    - Advantages
        - Can support a very large file size 
        - Brings all pointers together (centralized) into the inode
        - facilitate searching for a particular data block，树状结构寻找时间较短log
- Linked list
    - cons:
        - 安全性，一个断了就找不到了
        - 随机访问较慢
- **Tabular**: The logical structure is like a linked list, but the pointers in a tabular approach are centralized, i.e., stored at one or a few disk blocks rather than spread over many blocks.It is faster than linked-list approach as again the pointers are centralized. However, compared with multi-level index, the table can be very large (as basically all the pointers are just "direct pointers").
    -   优势：
        -   数据块位置的指针是集中管理的，便于查找和修改。
        -   表格可以被缓存，直接读到内存中，从而提高搜索速度，改善访问时间。
    -   劣势：
        -   随着磁盘规模的增加，块分配表可能变得相当庞大，占用较多的存储空间。
        -   对于大型磁盘，可能需要更复杂的机制来管理块分配表，以避免性能问题。
    - *Tabluar虽然是list，但是他实际上是由指针构成的，每个位置实际上存储的是指向下一个位置的指针，因此无论是随机查找还是顺序查找都需要从起点开始一个一个找*
- Caching and buffering:
    - 在没有缓存的情况下，磁盘I/O每读一个文件夹至少需要两次操作: inode+data
    - Inode table是储存在磁盘里的，所以刚上电的时候就要做磁盘I/O,Inode table只能告诉我们磁盘编号，我们还要拿着这个编号去找数据存在磁盘的什么位置
    <img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/ab411698b1fb454d8c122a31ee98a68e~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=828&h=193&s=115214&e=png&b=fdfdfd" alt="螢幕截圖 2023-12-12 上午10.55.16.png" width="70%" />
    - *如果要读取txt文件中的某个block，就需要I/O了*

<div STYLE="page-break-after: always;"></div>

#### 常见API
锁
```
// 互斥锁类型，用于在多线程环境中确保对共享资源的互斥访问
pthread _mutex t pthread _mutex_init()

// 初始化互斥锁
pthread mutex lock()

//解锁互斥锁，允许其他线程锁定该互斥锁
pthread _mutex_unlock()

// 销毁互斥锁
pthread _mutex_destory()
```
条件变量
```
pthread _cond_t 

// 初始化条件变量
pthread _cond_ init(pthread_cond_t *cond, const pthread_condattr_t *attr)

// 等待条件变量的信号，同时释放关联的互斥锁，使得其他线程可以进入临界区
pthread_cond_wait(&cond, &mutex) 

// 发送信号给等待在条件变量上的一个线程，使其从等待中醒来
pthread_cond_signal(&cond) 

// 发送信号给等待在条件变量上的所有线程，使它们从等待中醒来
pthread_cond_broadcast(&cond) 

// 销毁条件变量
pthread_cond_destory(&cond)
```
信号量

`sem_post()`是V操作，用于增加信号量的值；`sem_wait()`是P操作，用于减少信号量的值
```
// 信号量类型，用于在线程之间同步和互斥的控制
sema_t 

// 初始化
sema_init(&sema, 0, 100) 

// 尝试获取信号量，如果信号量的值大于零，则将其减一并继续执行；如果为零，则阻塞等待
sema_wait(sema_t *sema)

// 释放信号量，将其值加一。如果有其他线程在等待信号量，则其中一个线程将被唤醒。
sema_post(sema_t *sema)

sema_destroy(sema_t *sema)
```    

##### 死锁的发生条件和破除方法
- Mutual exclusion condition 使用互斥锁--> 不用互斥锁 
- Wait-for condition (hold-and-wait condition) 可以申请一部分资源，再申请另一部分资源--> 申请所有的资源都是原子操作（一次性可以申请所有资源）
- No-preemption condition --> 可以强行回收资源 
- Circular-wait condition --> 更改顺序，如：银行家算法 如果每把锁按照固定顺序，就不会发生死锁; 圈一样的循环等待

##### POSIX互斥锁
<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/f58984a976e04278a051ad9d6e6e1358~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1382&h=650&s=294558&e=png&b=292a2f" alt="螢幕截圖 2023-12-13 下午12.32.52.png" width="70%" />

##### 自旋锁
Test&Set

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/fcf27676199e46f096845fa0300700c9~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=904&h=548&s=104434&e=png&b=fefefe" alt="螢幕截圖 2023-12-13 下午12.34.04.png" width="70%" />

Compare&Swap

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/786a16c6b4ad40caa1cbb0a6b339de20~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=826&h=904&s=119384&e=png&b=fefefe" alt="螢幕截圖 2023-12-13 下午12.34.19.png" width="70%" />

Fetch&Add

<img src="https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/4ab850edd4af4ab4a40cba9c622ff3ad~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=980&h=712&s=93753&e=png&b=fefdfd" alt="螢幕截圖 2023-12-13 下午12.35.23.png" width="70%" />

##### Readers&Writers
- 信号量方法

<img src="https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/8efea721c93949689f0378259d74cbde~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1328&h=658&s=123721&e=png&b=fefefe" alt="螢幕截圖 2023-12-13 下午12.40.17.png" width="80%" />

- 条件变量+互斥锁方法

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/b432dbc898724bd5a541f49cbd7ff951~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=1348&h=994&s=167922&e=png&b=ffffff" alt="螢幕截圖 2023-12-13 下午12.41.12.png" width="80%" />

##### Producer&Consumer

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/9264b184a1104872a6645d92700e651c~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=994&h=896&s=191242&e=png&b=f7f7f7" alt="螢幕截圖 2023-12-13 下午12.44.38.png" width="70%" />

<img src="https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/3bd8f50fe55545548b7b1e86ce3804b7~tplv-k3u1fbpfcp-jj-mark:0:0:0:0:q75.image#?w=2250&h=928&s=208158&e=png&b=373544" alt="螢幕截圖 2023-12-13 下午12.44.16.png" width="90%" />

[1]:https://github.com/Jiang-Feiyu/Principles-of-operating-systems.git
[2]:https://lib.hku.hk/general/e-form/book-a-space.html