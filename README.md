# B-Tree-Indexing-Demo
Clustered indexing demo using b-trees on fixed sized block partitioning file system

## Overview
*This is a command line application which basically mimics the working of database and file system memory management. As we know generally in file systems memory is divided into fixed partitions called memory blocks. Data in memory blocks is stored in a non-contiguous manner. Now, linear search in these blocks makes the querying very slow and algorithms like binary search on the whole dataset is a very RAM heavy task. So we use B-trees to keep a clustered index on the data. B-trees perform search insert and delete operations in O(Logn) time and memory blocks (or we can say a node of the B-tree) are sent to the main memory one-by-one which makes it RAM efficient. So this project is a demo of this whole process.*
