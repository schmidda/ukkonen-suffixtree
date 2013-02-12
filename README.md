This is a basic implementation in C of Esko Ukkonen's online suffix tree 
building algorithm. It is intended as a teaching tool, since I have 
found that there are plenty of mathematical explanations of how the 
algorithm is truly linear and also plenty of implementations that are 
poorly written and hard to follow. From either source it is hard to work 
out exactly how to implement the algorithm. There is a full description 
of the code and the algorithm (minus the math) on 
http://programmerspatch.blogspot.com.au/2013/02/ukkonens-suffix-tree-algorithm.html. 
As explained there the tree.c file can be replaced by a faster 
implementation, e.g. one that uses small hash tables for large nodes. 
The supplied implementation uses linked lists which is adequate for 
testing and demonstration purposes.
