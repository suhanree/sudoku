Solving the Sudoku Puzzle
=====================

####By Suhan Ree
  
##1. Introduction

Here I briefly introduce the Sudoku puzzle, chosen for this
coding challenge, describe the algorithm for solving the puzzle,
and show how the code was implemented. 

The puzzle starts with a board with 81 cells (9 by 9) (it can be called as a grid, but from here on we will call it a *board*). Numbers from 1 to 9 are initially given to some cells, and remaining cells are empty. The goal of the puzzle is filling out all empty cells without breaking the rules. The rules are simple: (1) in any row of 9, any number cannot appear twice, which means all 9 numbers should appear once each, (2) the same for any column of 9, (3) the same for all 9 boxes (a board can be divided into 9 3x3 subgrids, and each subgrid is called a *box* here), too. (See Ref. **[1]**.)
The initial puzzle is supposed to have a unique answer, but it is possible that some wrongly constructed initial settings have no solution or multiple solutions. The program should be able to find out no-solution cases.

This puzzle is equivalent to the exact-cover problem **[2]**, which is NP-complete **[3]**.
The algorithm I use here is the recursive backtracking algorithm **[4]**, which basically searches through all empty cells one by one trying values. If there is no possible value for a cell, it goes back to the previous cell and try another value (backtracking). It is called recursive because it can be implemented using a simple recursive function.

The programming language I chose is C++. I made 6 classes: Pos, Board, Sudoku, Sudoku1, Sudoku2, and Sudoku3. The Pos class represents the position of a cell, the Board class represents the board for the puzzle, and Sudoku (and its derived classes, Sudoku1, Sudoku2, and Sudoku3) represents the solver. To solve the puzzle, I chose the recursive backtracking algorithm, which basically goes through empty cells, assigning values, by trial and error. The order of choosing empty cells can change the running time, so I chose three methods, which are implemented in three derived classes respectively.

  
  
  
##2. Algorithm: how to solve


  
  
  
##3. Implementation

The language of choice is C++, which I am most familiar with, for this
challenge (only using features supported by C++03). First, I describe
classes I made for this program; second, I explain how the flow of the
game is managed; third, I show how to run the code and how to play the
game using this program briefly; and lastly, I describe how each card is
represented during the game.

###3.1. Classes

**Pos class**

This class represents a position of a cell in a board.
***
`short r, c` :  `r`: row number (`0 <= r <= sizeR - 1`), `c`: column number (`0 <= c <= sizeC - 1`).

`static short sizeR, sizeC`: numbers of rows and columns of the given board respectively (default: 9).
***
`Pos(short r , short c)` : constructor.

`static void setSize(short sizeR, short sizeC)` : sets the sizes of the given board.

`void print() const` : prints the position to the stdout.  
***
***
**PosLessThan class**

This class defines the less-than operation between `Pos` objects. This class is needed when we store `Pos` objects in a set or as a key of a map.
***
***
**Board class**

This class represents the board of the Sudoku puzzle.
***
`vector< vector<unsigned char> > board`: 2D array to represent the board.

`vector< vector<bool> > empty`: 2D array to store true or false for emptiness of cells.

`short sizeBox`: size of the inner box (default: 3).

`short size`: size of the whole board (default: 9). `size = sizeBox * sizeBox`.

`short nEmpty`: number of current empty cells.
***
`Board(short sizeBox)`: constructor. `sizeBox` is given.

`short getSizeBox() const`: returns `sizeBox`.

`short getSize() const`: returns `size`.

`short getNEmpty() const`: returns `nEmpty`.

`void read(string filename)`: initializes the board by getting the puzzle from a *csv* file, named 'filename'. Here 0 means empty (an example of a row: 0,0,1,0,0,2,0,0,3).

`void write(string filename) const`: writes current contents of the board to a *csv* file, named 'filename'. It uses the same format as input files.

`unsigned char get(short r, short c) const`: returns the value of a cell at (`r`,`c`).
 
`void set(short r, short c, unsigned char v)`:  sets the value of a cell (`r`,`c`) to `v`.

`bool ifEmpty(short r, short c) const`:  returns true if the given cell (`r`,`c`) is empty (false if not).

`void makeEmpty(short r, short c)`: makes a given cell empty.

`bool ifRowConflict(short r, short c, unsigned char v) const`: returns true if the value v of the given empty cell (`r`,`c`) `  has any conflict with other cells in the same row.

`bool ifColConflict(short r, short c, unsigned char v) const`: returns true if the value v of the given empty cell (`r`,`c`) `  has any conflict with other cells in the same column.

`bool ifBoxConflict(short r, short c, unsigned char v) const`: returns true if the value v of the given empty cell (`r`,`c`) `  has any conflict with other cells in the same box.

`bool ifAnyConflict(short r, short c, unsigned char v) const`: returns true if the value v of the given empty cell (`r`,`c`) `  has any conflict with other cells in the same row or column or box.

`bool ifValid() const`: checks if a given board has any conflict for non-empty cells,
and if all values are valid. Returns true if there is no problem, false if not.
It will be used to check the validity of the initial puzzle.
(We still don't know if the puzzle is solvable or not.
It can still have no solution or multiple solutions.)

***
***
**Sudoku class**

This class represents a solver of the puzzle of Sudoku. This is the abstract class, and there will be three derived classes from this class.
***
`Board board`:  board of the puzzle.

`short depth`: depth of the current recursive step.

*** 
`Sudoku(short sizeBox)` : constructor. The default value of `sizeBox` is 3.

`void read(string filename)`: reads the initial puzzle from a *csv* file and initialize some data if necessary.

`void write(string filename) const`: writes the solution to a *csv* file.

`bool solve()`: solves the sudoku using the recursive backtracking algorithm.

`virtual void initialize()`: initializes the information for possibilities of all empty cells (defined in derived classes). 

`virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values)`: finds the next cell to explore, and returns true if successful, false if there is no more empty cell (defined in derived classes).
The position and possible values of the given cell are returned as arguments using references. 
***
***
**Sudoku1 class**

This class inherits the Sudoku class.
To store the order and the information about the empty cells in the beginning puzzle,
two arrays will be used.
The order the empty cells are stored is spatial, which means that,
starting at the top-left corner, it goes from left to right, 
and then top to bottom.
***
`vector<Pos> emptyCells`: the order of empty cells.                                                                

`vector<  vector<unsigned char> > possibleValues`: possible values at each empty cell.
***
`void initialize()`: initialize two arrays based on the given puzzle.

`bool getNextCell(short &r, short &c, vector<unsigned char> &values)`: finds the next cell to explore using the array `emptyCells`, and returns true if successful,  false if there is no more empty cell.
***
***
**Sudoku2 class**

This class inherits the Sudoku1 class.
The same two arrays as in Sudoku1 class will be used, but
the order the empty cells are stored is sorted by the number of possible                        
values. The order is kept throughout the process even though the number of 
possibilities change as other cells change.
The function `getNextCell` will be the same as in Sudoku1 class.
***
`void initialize()`: initialize two arrays based on the given puzzle.
***
***
**Sudoku3 class**

This class inherits the Sudoku class.
Every time the next cell is needed it finds all possible numbers for all empty cells, and choose the cell with the smallest number of possibilities.
There is no need to store extra information and no initialization is necessary.
***
`bool getNextCell(short &r, short &c, vector<unsigned char> &values)`: choose the cell with the smallest number of possibilities by searching all remaining empty cells, at every step.
The row and column number of that cell and its possible values will be returned as arguments.
***




###3.2. How to run and its performance

To make the code easy to evaluate, I put everything into two files (hearder and source),
called *sudoku.h* (\~300 lines) and *sudoku.C* (\~300 lines). I used the gcc compiler (g++,
version 4.4.7) in a CentOS virtual machine, but it should work fine in
other platforms with other compilers, I assume.

An input file is assumed to be a *csv* file. An example of an input file (*given.csv*) is as follows.

0,3,5,2,9,0,8,6,4  
0,8,2,4,1,0,7,0,3  
7,6,4,3,8,0,0,9,0  
2,1,8,7,3,9,0,4,0  
0,0,0,8,0,4,2,3,0  
0,4,3,0,5,2,9,7,0  
4,0,6,5,7,1,0,0,9   
3,5,9,0,2,8,4,1,7  
8,0,0,9,0,0,5,2,6

In this case, the solution is also written as a *csv* file (*solution.csv*) by the program as follows.

1,3,5,2,9,7,8,6,4  
9,8,2,4,1,6,7,5,3  
7,6,4,3,8,5,1,9,2  
2,1,8,7,3,9,6,4,5  
5,9,7,8,6,4,2,3,1  
6,4,3,1,5,2,9,7,8  
4,2,6,5,7,1,3,8,9  
3,5,9,6,2,8,4,1,7  
8,7,1,9,4,3,5,2,6  

The command line should look like this if the execution file is `a.out`,

`$ ./a.out given.csv solution.csv`

If the initial puzzle given by a *csv* file is invalid (*invalid.csv*), the program does not attempt to solve the puzzle, and will show the error message. If the puzzle does not have a solution (*nosol1.csv*, *nosol2.csv*, *nosol3.csv*), the program shows that it is an unsolvable puzzle. The puzzle can have multiple solutions, too; in these cases (*multi1.csv*, *multi2.csv*), the program will find just one possible solution.
It is possible for the program to check if it has multiple solutions, but I decided not to
implement this feature because (1) it takes longer to search through all possible solutions
even for legitimate puzzles with a unique solution, (2) we can say that the puzzle is solvable even when there are more than one answer.

Since I implemented three ways to find the next empty cell, I tried to find which method is best in performance. For 4 hard cases, I checked the CPU times for Sudoku1, Sudoku2, and Sudoku3, respectively.

*hard1.csv*: 0.03, 0.24, 0.05 (sec)   
*hard2.csv*: 0.53, 0.06, 0.37 (sec)   
*hard3.csv*: 0.13, 11.02, 1.77 (sec)   
*hard4.csv*: 0.03, 0.15, 0.05 (sec)   

It is hard to tell which is the best based on only 4 examples, but Sudoku2 does not perform well for *hard3.csv*, while surprisingly Sudoku1 performs very well in all 4 cases.

Then to really test these methods I tried a case with no solution with only 5 given values (*nosol2.csv*), and the CPU times are:

*nosol2.csv*: >2 hrs, 252 sec, and 6.5 sec,   

respectively, which clearly shows Sudoku1 can take too long for some cases, and Sudoku3 is the best one because it performs well in almost all cases. Hence, I decided to use Sudoku3 for this program. 

  
##4. Summary

In this problem, the user interface is not complicated, but the algorithm (logic) to find solutions is not easy.
Even though the problem is equivalent to an NP-complete problem (exact-cover problem **[2,3]**), it can be solved within seconds in most cases because the size is small enough.
Still  heuristics can be quite important as seen here, especially for hard puzzles.

I used in this code the recursive backtracking algorithm **[4]**. In this algorithm, the program visits empty cells and try a possible value one by one. If a value is valid, then try another empty cell and try possible values. If there is no possible value for a cell, go back to the previous cell and try another value for that cell. If it reaches the point where there is no more empty cell, the solution is found. It is easy to see that how to find the next cell can be crucial when it comes to the performance.
Here I implemented three different methods when finding the next empty cell, and the third one, where the cell with the minimum possibilities is found at every step, was chosen.
It can be time-consuming to search through all remaining empty cells at every step, however, it can save a lot of computing time in extreme cases, while comparable to other methods in other cases.

The reason is quite simple: we are basically performing a DFS (depth-first search) on a big tree, and if we reduce the number of branches near the top of the tree, we can reduce the number of visiting cells by a lot for big trees.


##5. References


1. [http://en.wikipedia.org/wiki/Sudoku](http://en.wikipedia.org/wiki/Sudoku)

1. [http://en.wikipedia.org/wiki/Exact\_cover](http://en.wikipedia.org/wiki/Exact\_cover)

1. [http://en.wikipedia.org/wiki/Karp%27s\_21\_NP-complete\_problems](http://en.wikipedia.org/wiki/Karp%27s\_21\_NP-complete\_problems)

1. [http://see.stanford.edu/materials/icspacs106b/H19-RecBacktrackExamples.pdf](http://see.stanford.edu/materials/icspacs106b/H19-RecBacktrackExamples.pdf)
