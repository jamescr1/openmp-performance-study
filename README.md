###Experimental Environment
Experiments were conducted on a Windows11 operating system. The machine was running a 12th Gen Intel Core i3-1215U processor, with six physical cores and eight logical processors. It had a hybrid architecture, made up of both performance cores and efficiency cores.

The machine's was equipped with approximately 7.7GiB RAM.

The compiler used was GCC 16.2.0, with flags '-O2', '-Wall', '-Wextra' and OpenMP flag 'fopenmp'. The programming language used was C, the OpenMP library was used in order to make use of shared-memory parallel programming. The OpenMP standard used was 5.2.

###Benchmark Methodology
A single warm up run for the serial implementation and one for the parallel implementation were first conducted. Then, the serial implementation was executed five times, followed by the parallel implementation being executed five times. Five execution runs were conducted in order to obtain an accurate representation of the average execution speed for each implementation and minimise the influence of one unrepresentatively quick or slow run on the results and analysis.

OpenMP's omp_get_wtime() function was used to measure each run time. Timing measurements covered only the workload computation, not the checksum calculation, warm up runs or other operations.

When setting up the workload, a fixed random seed (srand(42)) was used to ensure workload distribution was consistent throughout the experiment.

A checksum was calculated prior to the termination of the experiment to ensure that each implementation had executed correctly.

Two metrics were used to assess performance:

Speedup: Serial average execution time divided by parallel average execution time.
Parallel efficiency: Speedup divided by the number of threads used, expressed as a percentage.

###Workload Design
A workload consisting of 10,000 independent tasks was utilised for this experiment. A repeated arithmetic operation was performed for each task, with a variety of iteration numbers for each task in order to create differences in computational cost.

Two types of task were used for this experiment; an expensive task and a cheap task. There were 1,000 expensive tasks in total, and 9,000 cheap ones. Expensive tasks performed 500,000 iterations, whilst cheap tasks performed 10,000. Each task applied the same arithmetic operation to an initial 64-bit unsigned integer state:

[
s_{n+1} = s_n \times 6364136223846793005 + 1442695040888963407
]

###Workload Ordering
Two different workload distributions were tested.
The first distribution tested was a 'clustered' distribution, in which all 1,000 expensive tasks occurred in the first 1000 indexes of the array and the 9,000 expensive tasks occupied the following 9000 indexes in the array.

The second distribution tested was a 'randomised' distribution, in which the 1,000 expensive tasks were randomly distributed across the entire 10,000 slots of the array. Randomisation was achieved by using the Fisher-Yates algorithm, with a fixed seed of (srand(42)) to ensure consistency across each execution.

###OpenMP Scheduling Experiments
Static, dynamic, and guided scheduling strategies were used in order to analyse the impact that different scheduling had on execution times.

##Clustered Workload
This experiment incorporated the clustered workload, in which the 1,000 expensive tasks were all bunched at the start of the array.

For four threads, the measured average execution times were as below:

| Scheduling strategy     | Average execution time |
| ----------------------- | ---------------------: |
| Static                  |               1.0006 s |
| Dynamic (chunk size 10) |               0.3102 s |
| Guided (chunk size 10)  |               0.9650 s |

Dynamic scheduling was approximately 3.2 times faster than static scheduling, implying that dynamic work allocation was more effective at distributing the expensive tasks across threads than static work allocation.

Static scheduling was noticeably slower than dynamic scheduling because thread 0 was assigned all 1,000 heavy tasks, therefore threads 1, 2, and 3 were idle for much of the time that thread 0 was working. Guided scheduling performed poorly for a similar reason.

##Dynamic Scheduling Chunk Size
The chunk size of dynamic scheduling and its impact on execution time was also assessed. Similarly to the previous experiment, four threads were used along with a clustered workload.

| Chunk size | Average execution time |
| ---------- | ---------------------: |
| 10         |               0.3102 s |
| 100        |               0.3118 s |
| 1,000      |               0.9672 s |

As can be seen in the table above, a chunk size of 1,000 resulted in significantly poorer performance compared to a chunk size of 10 or 100. This is because for this experiment a chunk size of 1,000 results in an unbalanced distribution of workload across the four threads, since it causes one thread to be assigned all 1,000 expensive tasks. Chunk sizes of 10 or 100, despite the greater overhead caused by increased scheduling costs, results in a much more balanced workload, ultimately leading to significantly greater performance.

##Randomised Workload
For the randomised workload experiment, all 1,000 expensive tasks were randomly distributed across the entire array, as opposed to being grouped in the first 1,000 indexes of the array. The same number of tasks and iteration counts were retained, enabling a comparison with the clustered workload.

Using four threads, the execution times observed were as below:

| Scheduling strategy     | Average execution time |
| ----------------------- | ---------------------: |
| Static                  |               0.3262 s |
| Dynamic (chunk size 10) |               0.3070 s |

Static scheduling performed much better with the randomised workload compared to the clustered workload, since the more balanced workload caused by the randomisation of expensive tasks resulted in expensive tasks being more evenly distrubted across all four threads instead of being concentrated on thread 0.

###Discussion
These results suggest that the distribution and ordering of computational work can have an impact on the performance of OpenMP scheduling strategies.

When expensive tasks were grouped together, dynamic scheduling was much more effective than static scheduling. However, when expensive tasks, and therefore the computational workload, was more evenly spread, as in the randomised workload, there was a much smaller performance difference between static and dynamic scheduling.

These experiments demonstrate the significance of accounting for scheduling and workload distribution when developing parallel programs.

###Scalability
This experiment assessed how the number of OpenMP threads used in the parallel program impacted execution time and efficiecy.
The number of threads used ranged from a single thread all the way up to eight threads.

##Results

| Configuration | Average execution time | Speedup | Parallel efficiency |
| ------------- | ---------------------: | ------: | ------------------: |
| Serial        |               1.1334 s |   1.00× |                   — |
| 1 thread      |               1.0806 s |   1.05× |              104.9% |
| 2 threads     |               0.5874 s |   1.93× |               96.5% |
| 4 threads     |               0.3442 s |   3.29× |               82.3% |
| 6 threads     |               0.2408 s |   4.71× |               78.4% |
| 8 threads     |               0.2110 s |   5.37× |               67.1% |

##Discussion
The results above demonstrate the significant performance improvements available by exploiting multi-threaded programming. Using two threads as opposed to one thread resulted in an doubling of speedup, with eight threads resulting in a five-fold increase in speedup when compared to one thread. Interestingly, the increase in speedup become less effective with increasing thread count.
