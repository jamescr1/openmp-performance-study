#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_TASKS 10000
#define NUM_RUNS 5
#define NUM_THREADS 8

uint64_t perform_work(uint64_t state, int iterations);
void run_workload(int workload[], uint64_t results[]);
void run_workload_parallel(int workload[], uint64_t results[], int num_threads);
void initialise_workload(int workload[]);
void checkSumsMatch(uint64_t checksumSerial, uint64_t checksumParallel);
double calculateAverageTime(double times[], int numberOfRuns);
double calculateSpeedup(double serial_average, double parallel_average);
double calculateEfficiency(double speedup, int num_threads);

int main(void)
{
    uint64_t results[NUM_TASKS];
    int workload[NUM_TASKS];

    initialise_workload(workload);

    // Warm-up serial run
    run_workload(workload, results);

    // Warm-up parallel run
    run_workload_parallel(workload, results, NUM_THREADS);

    // creating arrays to store run times
    double serial_times[NUM_RUNS];
    double parallel_times[NUM_RUNS];

    // variables to store average execution time
    double serial_average = 0.0;
    double parallel_average = 0.0;

    // variables to store performance metrics
    double speedup = 0.0;
    double efficiency = 0.0;

    // benchmark serial implementation
    for (int run = 0; run < NUM_RUNS; run++)
    {
        double start = omp_get_wtime();

        run_workload(workload, results);

        double end = omp_get_wtime();

        serial_times[run] = end - start;

        printf("Serial run %d: %f seconds\n", run + 1, serial_times[run]);
    }

    serial_average = calculateAverageTime(serial_times, NUM_RUNS);
    printf("Serial Implementation Average Run Time: %f seconds\n", serial_average);

    uint64_t checksumSerial = 0;

    for (int i = 0; i < NUM_TASKS; i++)
    {
        checksumSerial += results[i];
    }

    printf("Serial implementation checksum: %llu\n", (unsigned long long)checksumSerial);

    // benchmark parallel implementation
    for (int run = 0; run < NUM_RUNS; run++)
    {
        double start = omp_get_wtime();

        run_workload_parallel(workload, results, NUM_THREADS);

        double end = omp_get_wtime();

        parallel_times[run] = end - start;

        printf("Parallel run %d: %f seconds\n", run + 1, parallel_times[run]);
    }

    parallel_average = calculateAverageTime(parallel_times, NUM_RUNS);
    printf("Parallel Implementation (%d threads) Average Run Time: %f seconds\n", NUM_THREADS, parallel_average);

    uint64_t checksumParallel = 0;

    for (int i = 0; i < NUM_TASKS; i++)
    {
        checksumParallel += results[i];
    }

    printf("Parallel implementation checksum: %llu\n", (unsigned long long)checksumParallel);

    checkSumsMatch(checksumSerial, checksumParallel);

    speedup = calculateSpeedup(serial_average, parallel_average);
    printf("Speedup is: %f\n", speedup);

    efficiency = calculateEfficiency(speedup, NUM_THREADS);
    printf("Efficiency is: %.2f%%\n", efficiency * 100);

    return 0;
}

double calculateSpeedup(double serial_average, double parallel_average)
{
    return serial_average / parallel_average;
}

double calculateEfficiency(double speedup, int num_threads)
{
    return speedup / num_threads;
}

double calculateAverageTime(double times[], int numberOfRuns)
{
    double totalTime = 0.0;
    for (int i = 0; i < numberOfRuns; i++)
    {
        totalTime += times[i];
    }
    double averageTime = totalTime / numberOfRuns;
    return averageTime;
}

void checkSumsMatch(uint64_t checksumSerial, uint64_t checksumParallel)
{
    if (checksumSerial == checksumParallel)
    {
        printf("Serial and Parallel implementations both produce same sum.\n");
    }
    else
    {
        printf("ERROR: Serial and Parallel implementations produced different sums.\n");
    }
}

uint64_t perform_work(uint64_t state, int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    }
    return state;
}

void run_workload(int workload[], uint64_t results[])
{

    for (int i = 0; i < NUM_TASKS; i++)
    {
        int iterations = workload[i];

        results[i] = perform_work(i + 1, iterations);
    }
}

void run_workload_parallel(int workload[], uint64_t results[], int num_threads)
{

#pragma omp parallel for num_threads(num_threads) schedule(static)

    for (int i = 0; i < NUM_TASKS; i++)
    {
        int iterations = workload[i];

        results[i] = perform_work(i + 1, iterations);
    }
}

void initialise_workload(int workload[])
{
    // Assign the initial workload.
    for (int i = 0; i < NUM_TASKS; i++)
    {
        if (i < 1000)
        {
            workload[i] = 500000;
        }
        else
        {
            workload[i] = 10000;
        }
    }
}