# B-Tree-Indexing-Demo
Clustered indexing demo using b-trees on fixed sized block partitioning file system

## Overview
*This is a command line application thst mimics the working of a database or a file system memory management. In file systems, memory is divided into fixed partitions called memory blocks. Data in memory blocks is stored in a non-contiguous manner.*

*Now, linear search in these blocks makes the querying very slow and algorithms like binary search on the whole dataset is a very RAM heavy task. So we use B-trees to keep a clustered index on the data. B-trees perform search insert and delete operations in O(Logn) time and memory blocks (or we can say a node of the B-tree) are sent to the main memory one-by-one which makes it RAM efficient.*

*So this project is a demo of this whole process.*

## Walkthrough of the project
![Run the program](/ss/s1.png)

> Enter the password to enter the system.


![Run the program](/ss/s2.png)

> This step shows the list of databases present in the system.


![Run the program](/ss/s3.png)

> This is the query terminal where we can perform various queries on the DB and tables.


![Run the program](/ss/s6.png)

> Let's create a few DBs.


![Run the program](/ss/s7.png)

> Here we can see the DBs created in form of directories.


![Run the program](/ss/s9.png)

> We can also delete the databases and enter the DB where we want to work.


![Run the program](/ss/s10.png)

> Now we have entered the "users" Db.


![Run the program](/ss/s11.png)

> Here we can see that initially DB is empty and root of the B-Tree index is NULL


![Run the program](/ss/s12.png)

> Here we are creating a table and specifying the schema.


![Run the program](/ss/s13.png)

> Creating a table creates a schema file and an index directory to store the B-Tree.


![Run the program](/ss/s14.png)

> Let's insert a few users in our table.


![Run the program](/ss/s15.png)

> Now here we see a file with a random name is created. This random number represents the physical memory address of a memory block which can store a fixed amount of data sequentially. In this implementation, a memory block can store 4 tuples of a table.


![Run the program](/ss/s16.png)

> In the index directory we can see that an index node is created and stored in a memory block. It can also be observed that index node contains the memory address of the data block which contains the users.


![Run the program](/ss/s17.png)

> Let's insert a few more users.


![Run the program](/ss/s19.png)

> Now our B-Tree has grown in height. Here root file (245615398.csv) contains 3 memory addresses. One on the left is the address of an index node (left child of 1st key). One on the right is the address of another index node (right child of 1st key). One in the middle is memory address of the block containing data tuples.


![Run the program](/ss/s20.png)

> Now we can search rows in O(logn) time using B-Tree.
