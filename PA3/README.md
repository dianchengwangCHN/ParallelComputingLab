## Lab3

Matrix multiplication acceleration using **CUDA**.

| Version       | Description                                                                                           | Performance (GFLOPS) |
| ------------- | ----------------------------------------------------------------------------------------------------- | -------------------- |
| Based version | Sequential martrix multiplication grogram                                                             | 5.5                  |
| PA3-v1        | Simple parallel version without using shared memory.                                                  | 248.6                |
| PA3-v2        | Utilizing shared memmory to speed up performance.                                                     | 984.2                |
| PA3-v3        | 2-way loop unrolling along i to improve instruction level parallelism and reduce memory access times. | 1174.2               |
| PA4-v4        | 2-way loop unrolling along j.                                                                         | 1277.4               |
| PA4-v5        | 2-way loop unrolling along both i and j.                                                              | 1716.6               |
| PA4-v6        | 2-way loop unrolling along i and 4-way loop unrolling along j.                                        | 2407.7               |
