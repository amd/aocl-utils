# The Threadpinning module testplan

## The thread pinning module

The thread pinning module provides an interface that can facilitate pinning threads to cores based on the pinning strategy requested.

## The objective

To have a compreshensive test coverage for the threadpinning API

## Classes of testcases

* **mock Tests**
* **native tests**

## APIs tested

``` bash
* ThreadPinning::pinThreads()       -- Cpp API   -- External API
* Affinity::getAffinityVector()     -- Cpp API   -- Internal Using mock tests
* au_pin_threads_core()             -- C API     -- External API
* au_pin_threads_logical()          -- C API     -- External API
* au_pin_threads_spread()           -- C API     -- External API
* au_pin_threads_custom()           -- C API     -- External API
```

## The test matrix for the threadpinning module mock tests

combinations of 5 variables are tested. They are:

* **Number of threads**
* **Number of cores**
* **Pinning strategy**
* **Hyperthreading**
* **Cache sharing**

The below tests tests getAffinityVector() API which is an internal API by creating a mock environment described by each entry in the test matrix.
and verifies that the affinity vector returned is as expected.

| ID | No. of core groups | Hyperthreading | Cache groups | No of threads | Pinning strategy |
|----|--------------------|----------------|--------------|---------------|------------------|
| 1  | 1                  |  NO HT         | 1            | > no of cores |          core    |
| 2  | 1                  |  NO HT         | 1            | > no of cores |          Logical |
| 3  | 1                  |  NO HT         | 1            | > no of cores |          Spread  |
| 4  | 1                  |  NO HT         | 1            | < no of cores |          core    |
| 5  | 1                  |  NO HT         | 1            | < no of cores |          Logical |
| 6  | 1                  |  NO HT         | 1            | < no of cores |          Spread  |
| 7  | 1                  |  NO HT         | 1            | = no of cores |          core    |
| 8  | 1                  |  NO HT         | 1            | = no of cores |          Logical |
| 9  | 1                  |  NO HT         | 1            | = no of cores |          Spread  |
| 10 | 1                  |  NO HT         | Multi        | > no of cores |          core    |
| 11 | 1                  |  NO HT         | Multi        | > no of cores |          Logical |
| 12 | 1                  |  NO HT         | Multi        | > no of cores |          Spread  |
| 13 | 1                  |  NO HT         | Multi        | < no of cores |          core    |
| 14 | 1                  |  NO HT         | Multi        | < no of cores |          Logical |
| 15 | 1                  |  NO HT         | Multi        | < no of cores |          Spread  |
| 16 | 1                  |  NO HT         | Multi        | = no of cores |          core    |
| 17 | 1                  |  NO HT         | Multi        | = no of cores |          Logical |
| 18 | 1                  |  NO HT         | Multi        | = no of cores |          Spread  |
| 19 | 1                  |     HT         | 1            | > no of cores |          core    |
| 20 | 1                  |     HT         | 1            | > no of cores |          Logical |
| 21 | 1                  |     HT         | 1            | > no of cores |          Spread  |
| 22 | 1                  |     HT         | 1            | < no of cores |          core    |
| 23 | 1                  |     HT         | 1            | < no of cores |          Logical |
| 24 | 1                  |     HT         | 1            | < no of cores |          Spread  |
| 25 | 1                  |     HT         | 1            | = no of cores |          core    |
| 26 | 1                  |     HT         | 1            | = no of cores |          Logical |
| 27 | 1                  |     HT         | 1            | = no of cores |          Spread  |
| 28 | 1                  |     HT         | Multi        | > no of cores |          core    |
| 29 | 1                  |     HT         | Multi        | > no of cores |          Logical |
| 30 | 1                  |     HT         | 2            | > no of cores |          Spread  |
| 31 | 1                  |     HT         | Multi        | < no of cores |          core    |
| 32 | 1                  |     HT         | Multi        | < no of cores |          Logical |
| 33 | 1                  |     HT         | Multi        | < no of cores |          Spread  |
| 34 | 1                  |     HT         | Multi        | = no of cores |          core    |
| 35 | 1                  |     HT         | Multi        | = no of cores |          Logical |
| 36 | 1                  |     HT         | Multi        | = no of cores |          Spread  |
| 37 | Multi                  |  NO HT         | 1        | > no of cores |          core    |
| 38 | Multi                  |  NO HT         | 1        | > no of cores |          Logical |
| 39 | Multi                  |  NO HT         | 1        | > no of cores |          Spread  |
| 40 | Multi                  |  NO HT         | 1        | < no of cores |          core    |
| 41 | Multi                  |  NO HT         | 1        | < no of cores |          Logical |
| 42 | Multi                  |  NO HT         | 1        | < no of cores |          Spread  |
| 43 | Multi                  |  NO HT         | 1        | = no of cores |          core    |
| 44 | Multi                  |  NO HT         | 1        | = no of cores |          Logical |
| 45 | Multi                  |  NO HT         | 1        | = no of cores |          Spread  |
| 46 | Multi              |  NO HT         | Multi        | > no of cores |          core    |
| 47 | Multi              |  NO HT         | Multi        | > no of cores |          Logical |
| 48 | Multi              |  NO HT         | Multi        | > no of cores |          Spread  |
| 49 | Multi              |  NO HT         | Multi        | < no of cores |          core    |
| 50 | Multi              |  NO HT         | Multi        | < no of cores |          Logical |
| 51 | Multi              |  NO HT         | Multi        | < no of cores |          Spread  |
| 52 | Multi              |  NO HT         | Multi        | = no of cores |          core    |
| 53 | Multi              |  NO HT         | Multi        | = no of cores |          Logical |
| 54 | Multi              |  NO HT         | Multi        | = no of cores |          Spread  |
| 55 | Multi              |     HT         | 1            | > no of cores |          core    |
| 56 | Multi              |     HT         | 1            | > no of cores |          Logical |
| 57 | Multi              |     HT         | 1            | > no of cores |          Spread  |
| 58 | Multi              |     HT         | 1            | < no of cores |          core    |
| 59 | Multi              |     HT         | 1            | < no of cores |          Logical |
| 60 | Multi              |     HT         | 1            | < no of cores |          Spread  |
| 61 | Multi              |     HT         | 1            | = no of cores |          core    |
| 62 | Multi              |     HT         | 1            | = no of cores |          Logical |
| 63 | Multi              |     HT         | 1            | = no of cores |          Spread  |
| 64 | Multi              |     HT         | Multi        | > no of cores |          core    |
| 65 | Multi              |     HT         | Multi        | > no of cores |          Logical |
| 66 | Multi              |     HT         | Multi        | > no of cores |          Spread  |
| 67 | Multi              |     HT         | Multi        | < no of cores |          core    |
| 68 | Multi              |     HT         | Multi        | < no of cores |          Logical |
| 69 | Multi              |     HT         | Multi        | < no of cores |          Spread  |
| 70 | Multi              |     HT         | Multi        | = no of cores |          core    |
| 71 | Multi              |     HT         | Multi        | = no of cores |          Logical |
| 72 | Multi              |     HT         | Multi        | = no of cores |          Spread  |

## The test matrix for the threadpinning module native tests

Native tests are run on the actual hardware. The test matrix is as follows:
Verifies that the thread pinning is done correctly on the actual hardware.
by checking the affinity of the threads to the cores.
Variables testsed are:

* **Number of threads**
* **Pinning strategy**

| ID |  No of threads | Pinning strategy |
| ---|----------------|------------------|
| 1  |  > no of cores |          core    |
| 2  |  > no of cores |          Logical |
| 3  |  > no of cores |          Spread  |
| 4  |  < no of cores |          core    |
| 5  |  < no of cores |          Logical |
| 6  |  < no of cores |          Spread  |
| 7  |  = no of cores |          core    |
| 8  |  = no of cores |          Logical |
| 9  |  = no of cores |          Spread  |

Similar tests are provided for individual capis.
