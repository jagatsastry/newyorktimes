
This code takes a graph adjacency file as input and outputs (overlapping) 
low conductance clusters

------------------------------
Author: Leman Akoglu
Date: 11/6/2012

Email: leman@cs.stonybrook.edu
------------------------------

**) To compile the code, simply run

make clean
make

Make sure that you have the C++ BoostGraph installed on your system


**) To run the code, after compilation type

./run

There are 3 command inputs that the code expects

1- File name : adjacency file, expected to be in a folder called "dataset" (without quotes) 
2- Number of seeds : Typically the number of clusters wanted
3- Output folder name : self explanatory

** A toy example graph of 900 nodes is given in folder dataset/
**In the output folder, there will be files of the name format cluster_i,
each listing the node IDs they contain
** Note that clusters may be overlapping, and total number of nodes
in all clusters may be larger than total number of nodes in the input graph
  




**) Other parameters that can be played with:
In projectheader.h

%% const double OVERLAPF = 0.50;  
--> Maximum allowed fraction of the nodes of a new cluster with nodes in existing clusters
--> We drop the newly found clusters if its overlap fraction is larger than this 

%% const int MINSZ = 50; and
%% const int MAXSZ = 250;
--> Minimum and maximum cluster sizes allowed

No need to change any other parameters for clustering only (they are for some other functionality that are disabled)

If/when you change those 3 parameters in projectheader.h,
you need to recompile your code to get them effective, simply run
make clean
make






**) For a demo: --------------------------------------

-bash-3.1$ make clean
rm -f boostGraph.o approximatePageRank.o storeloadMethods.o run

-bash-3.1$ make
g++ -Wall -pedantic-errors -g  -c boostGraph.cpp
g++ -Wall -pedantic-errors -g   -c -o approximatePageRank.o approximatePageRank.cpp
g++ -Wall -pedantic-errors -g   -c -o storeloadMethods.o storeloadMethods.cpp
g++ -Wall -pedantic-errors -g boostGraph.o approximatePageRank.o storeloadMethods.o main.cpp -o run

-bash-3.1$ ./run
Enter filename in dataset/:
toy_5clus.txt

There are 143941 that is 143941 edges in g

There are 900 vertices in g

Enter number of seed nodes k:
10
Enter output folder name:
clusters


** the demo finds non-overlapping 5 clusters, that is because the synthetic clusters are well rounded and separated.
** BoostGraph library adds a node with ID 0 (don't know why), 
and thinks it is in cluster 6, so simply ignore output file cluster_6 (which is empty anyway) 











