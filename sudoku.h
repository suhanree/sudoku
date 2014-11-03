//
//	File: sudoku.h
//	Date: November 2014
//
//	Programmed by Suhan Ree for Insight Engineering Fellows Coding Challenge.
//
//	The program solves the puzzle Sudoku.
//
//	Everything is put into two files (header & source) 
//		to make it easier to read and compile.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

// Some names from std to be used in the code frequently.
using std::cout;	
using std::cin;
using std::endl;
using std::vector;
using std::string;

// Classes for error exceptions.
struct BadSize {};
struct BadInput {};
struct BadValue {};
struct NoInputFile {};
struct InvalidPuzzle {};

// Class that represents the position of the board (or grid).
// 	For example, for 9x9 board, the position is represented by (r,c) 
// 	where r: row number (0<=r<=sizeR-1), c: col number (0<=c<=sizeC)
class Pos {
   public :
   	// Constructor.
	Pos() : r(0), c(0) {};
	Pos(short x, short y)
		: r(x % sizeR), c(y % sizeC) {}; // r & c in range always.
	// Destructor.
	~Pos() {};

	// Prints the values.
	void print() const {
		std::cout << '(' << r << ',' << c << ")\n";
	};

	// Sets the size of the board.
	static void setSize(short sR, short sC) {
		if (sR < 4 || sC < 4 || sR > 100 || sC > 100) throw BadSize();
		sizeR = sR;
		sizeC = sC;
	};

   public : // public, to access the point values easier.
   	short r, c; 
	static short sizeR, sizeC; // size of the board: sizeR x sizeC.
				// In Sudoku puzzles, sizeR = sizeC always.	
};

// Initializes static variables of Pos class(can be changed later using setSize)
short Pos::sizeR = 9; 
short Pos::sizeC = 9;

// A class that defines the "less than" function for the Pos class
// Needed because Pos will  be used as keys for a map container.
// The row-wise order: (0,0), (0,1), ..., (1,0), (1,1), (1,2), ...
class PosLessThan {
   public :
	inline bool operator() (const Pos & p1, const Pos & p2) {
		return (p1.c==p2.c ? p1.r < p2.r : p1.c < p2.c);
	};
};

// Class that represents a board for Sudoku.
// The size of the board can be set by the user (default: 9x9).
// Values at cells are represented by the type 'unsigned char',
// 	(0 means empty, and numbers between 1 and size will be used as values.)
// Two arrays will be used to store board information: for value and emptiness.
class Board {
   public:
	// Constructor.
	// n^2 will be the size of the board. (Assumed 2 <= n <= 10)
	// For example, for n = 3 (default), 9x9 board will be used.
	Board(short n = 3): sizeBox(n), size(n*n), board(n*n), empty(n*n), \
		nEmpty(n*n*n*n) { // All cells are empty in the beginning.
		Pos::setSize(size, size); // Setting the size for the Pos class.
		// Initializing arrays (assuming every cell is empty).
		for (short i = 0; i < size; i++) {
			board[i].resize(size);
			empty[i].resize(size);
			for (short j = 0; j < size; j++) empty[i][j] = true;
		};
	};

	// Returns the size of the box of the board (=sqrt(size)).
	short getSizeBox() const {return sizeBox;};

	// Returns the size of the total board.
	short getSize() const {return size;};

	// Gets the number of empty cells.
	short getNEmpty() const {return nEmpty;};

	// Initializes the board by getting the puzzle from a csv file.
	// 0 means empty (an example of a row: 0,0,1,0,0,2,0,0,3)
	void read(string filename);

	// Writes current contents of the board to a csv file.
	// (Uses the same format as input.)
	void write(string filename);

	// Gets the value of a cell (r,c)
	unsigned char get(short r, short c) const {return board[r][c];};

	// Sets the value of a cell (r,c) to v.
	void set(short r, short c, unsigned char v);

	// Returns true if the given cell (r,c) is empty (false if not).
	bool ifEmpty(short r, short c) const {return empty[r][c];}

	// Makes a given cell empty.
	void makeEmpty(short r, short c) {
		if (!ifEmpty(r,c)) { // Only when the cell is not empty.
			empty[r][c] = true;
			nEmpty++;
		};
	};		

	// Returns true if the value v of the given empty cell (r,c) 
	// has any conflict with other cells in the same row.
	bool ifRowConflict(short r, short c, unsigned char v) const {
		for (short j = 0; j < size; j++)
			// For non-empty same-row cells.
			if (!ifEmpty(r,j) && j != c && board[r][j] == v)
				return true;
		return false;
	}

	// Returns true if the value v of the given empty cell (r,c) 
	// has any conflict with other cells in the same column.
	bool ifColConflict(short r, short c, unsigned char v) const {
		for (short i = 0; i < size; i++)
			// For non-empty same-col cells.
			if (!ifEmpty(i,c) && i != r && board[i][c] == v)
				return true;
		return false;
	}

	// Returns true if the value v of the given empty cell (r,c) 
	// has any conflict with other cells in the same box (region).
	bool ifBoxConflict(short r, short c, unsigned char v) const {
		// Starting row & col number for the box of the given cell.
		short rBox0 = r - r % sizeBox;
		short cBox0 = c - c % sizeBox;
		for (short i = rBox0; i < rBox0 + sizeBox; i++) 
			for (short j = cBox0; j < cBox0 + sizeBox; j++) 
				if (!ifEmpty(i,j) && i != r && j != c \
					&& board[i][j] == v) 
					return true;
		return false;
	}

	// Returns true if the value v of the given empty cell (r,c) 
	// has any conflict with other cells in the same row or col or box.
	bool ifAnyConflict(short r, short c, unsigned char v) const {
		return (ifRowConflict(r,c,v) || ifColConflict(r,c,v) || \
			ifBoxConflict(r,c,v));
	};

	// Checks if a given board has any conflict for non-empty cells.
	// Returns true if there is no conflict, and it has valid values.
	// It will be used to check the validity of the initial puzzle.
	// 	(We still don't know if the puzzle is solvable or not.
	// 	It can still have no solution or multiple solutions.)
	bool ifValid() const;

   private:
	short sizeBox; // size of the box (region) inside the board.
   	short size; // size of the board (size x size).
	vector< vector<unsigned char> > board; // 2D array representing board.
	vector< vector<bool> > empty; 	// 2D array for emptiness of cells 
					//	(true if empty).
	short nEmpty; // number of empty cells.
};		

// Class to solve sudoku puzzles using the recursive backtracking algorithm.
// Three different heuristics will be used to find next empty cells 
// for recursions, and they will be represented by 3 derived classes.
//
// Get the next cell to explore (implemented 3 methods to get it).
// 1, Get empty cells from the initial puzzle and store them in an array
// 	using the spatial order starting from the top left to the right.
// 2, Find all possible numbers for all empty cells from the initial
// 	puzzle, and sort them by the number of possible numbers in an
// 	ascending order. Get the next one using this order.
// 3, Find all possible numbers for all empty cells at each step, and
// 	choose the cell with the smallest number of possibilities
// 	as the next cell to explore.

// Base class for the sudoku solver class.
class Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the board will be sizeBox^2.
	Sudoku(short sizeBox=3): board(sizeBox), depth() {};

	// Reads the initial puzzle from a csv file and initialize if necessary.
	void read(string inFilename) {
		board.read(inFilename);
		initialize();
	};

	// Writes the solution.
	void write(string outFilename) {
		board.write(outFilename);
	};

	// Solves the sudoku using the recursive backtracking algorithm.
	bool solve();

   protected:
   	// Initializes the information for possibilities of all empty cells.
	// It will be used to calculate extra data for the given method of 
	// finding the next cell.
	virtual void initialize() {};

	// Finds the next cell to explore (returns true if successful).
	// The position and possible values of the given cell are 
	// 	returned as arguments using references.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values)=0;

   protected:
   	// Board for the puzzle.
	Board board;
	// Depth of the current recursive step. (1st step will be depth 0).
	short depth;
};

// Derived class of the sudoku class (method 1).
// 1, Get empty cells from the initial puzzle and store them in an array
// 	using the spatial order starting from the top left to the right.
class Sudoku1: public Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be sizeBox^2.
	Sudoku1(short sizeBox=3): Sudoku(sizeBox), emptyCells(), \
		possibleValues() {};

   protected:
   	// Initializes the information for possibilities of all empty cells
	// using two arrays for positions and possibilities.
	// The order the empty cells is stored are spatial, which means that,
	// starting at the top-left corner, it goes from left to right, 
	// and top to bottom.
	virtual void initialize();

	// Finds the next cell to explore (returns true if successful).
	// The position and possible values of the given cell are 
	// 	returned as arguments using references.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values);

   protected:
	// Arrays for storing possible values for all empty cells.
	vector<Pos> emptyCells;
	vector< vector<unsigned char> > possibleValues;
};

// Derived class of the sudoku class (method 2).
// 2, Find all possible numbers for all empty cells from the initial
// 	puzzle, and sort them by the number of possible numbers in an
// 	ascending order. Get the next one using this order.
class Sudoku2: public Sudoku1 {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be sizeBox^2.
	Sudoku2(short sizeBox=3): Sudoku1(sizeBox) {};

   protected:
   	// Initializes the information for possibilities of all empty cells
	// using two arrays for positions and possibilities.
	// The order the empty cells are stored are sorted by the number of 
	// possible values.
	virtual void initialize();
};

// Derived class of the sudoku class (method 3).
// 3, Find all possible numbers for all empty cells at each step, and
// 	choose the cell with the smallest number of possibilities
// 	as the next cell to explore.
class Sudoku3: public Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be sizeBox^2.
	Sudoku3(short sizeBox=3): Sudoku(sizeBox) {};

   protected:
	// Finds the next cell to explore (returns true if successful).
	// The position and possible values of the given cell are 
	// 	returned as arguments using references.
	// In this case, it will choose the cell with the smallest number 
	// of possibilities by searching all remaining empty cells.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values);

   protected:
	// Nothing extra to store for this method (no need to initialize, too).
};
