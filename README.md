# Cache-Virtual-Memory-Simulator
CS 3853 Computer Architecture Group Project: Cache &amp; Virtual Memory Simulator
The simulator has the following input parameters:

  1. –f <trace file name> [ name of text file with the trace ]
          a. You must accept 1, 2, or 3 files as input
          b. Each file will use a “-f “ to specify it
  2. –s <cache size in KB> [ 1 KB to 8 MB ]
  3. –b <block size> [ 4 bytes to 64 bytes ]
  4. –a <associativity> [ 1, 2, 4, 8, 16 ]
  5. –r <replacement policy> [ RR or RND]  Implement any one of these
  6. –p <physical memory in KB> [64 KB to 512 GB]

Example input:
./cache_simulator –f trace1.trc –s 512 –b 16 –a 2 –r RR –p 1GB
