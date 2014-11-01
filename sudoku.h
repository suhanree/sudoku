//
//	File: sudoku.h
//	Date: October 2014
//
//	Programmed by Suhan Ree for Insight Engineering Fellows Code Challenge.
//
//	The program solve the puzzle Sudoku.
//
//	Everything is put into two files (header, source) 
//		to make it easier to read and compile.
//


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

// Some names from std to be used in the code.
using std::cout;	
using std::cin;
using std::endl;
using std::vector;
using std::string;

// classes for error exceptions.
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

	// Print the values.
	void print() const {
		std::cout << '(' << r << ',' << c << ")\n";
	};

	// Set the size of the board.
	static void setSize(short sR, short sC) {
		if (sR < 4 || sC < 4 || sR > 100 || sC > 100) throw BadSize();
		sizeR = sR;
		sizeC = sC;
	};

   public : // public, to access the point values easier.
   	short r, c; 
	static short sizeR, sizeC; // size of the board: sizeR x sizeC.
};

// Initialize static variables of Pos class(can be changed later using setSize)
short Pos::sizeR = 9; 
short Pos::sizeC = 9;

// A class that defines the "less than" function for the Pos class
class PosLessThan {
   public :
	inline bool operator() (const Pos & p1, const Pos & p2) {
		return (p1.c==p2.c ? p1.r < p2.r : p1.c < p2.c);
	};
};

// Class that represents a board for Sudoku.
// The size of the board can be set by the user (default: 9x9).
// Values at cells are represented by the type 'unsigned char',
// 	(0 means empty, and numbers between 1 and size will be used as values.
class Board {
   public:
	// Constructor.
	// (n*n) will be the size of the board. (Assumed 2 <= n <= 10)
	// For example, for n = 3 (default), 9x9 board will be used.
	Board(int n = 3): sizeBox(n), size(n*n), board(n*n), empty(n*n), nEmpty(0) {
		Pos::setSize(size, size); // Setting the size for the Pos class).
		for (int i = 0; i < size; i++) {
			board[i].resize(size);
			empty[i].resize(size);
			for (int j = 0; j < size; j++) empty[i][j] = false;
		};
	};

	// Return the size of the box of the board (=sqrt(size)).
	short getSizeBox() const {return sizeBox;};

	// Return the size of the total board.
	short getSize() const {return size;};

	// Get the number of empty cells.
	short getNEmpty() const {
		return nEmpty;
	};

	// Initialize the board (get the puzzle)(returns false if unsuccessful)
	// from a csv file.
	void read(string filename);

	// Write the contents of the board to a csv file.
	void write(string filename);

	// Get the value of a cell (r,c)
	unsigned char get(short r, short c) const {
		return board[r][c];
	};

	// Set the value of a cell (r,c) to v.
	void set(short r, short c, unsigned char v);


	// Returns true if the given cell (r,c) is empty (false if not).
	bool ifEmpty(short r, short c) const {
		return empty[r][c];
	}

	// Make a given cell empty.
	void makeEmpty(short r, short c) {
		if (!ifEmpty(r,c)) { // Only when the cell is not empty.
			empty[r][c] = true;
			nEmpty++;
		};
	};		

	// Returns true if the value v of the given empty cell (r,c) 
	// has conflict with other cells in the same row.
	bool ifRowConflict(short r, short c, unsigned char v) const {
		for (int j = 0; j < size; j++)
			// For non-empty same-row cells.
			if (!ifEmpty(r,j) && j != c && board[r][j] == v)
				return true;
		return false;
	}

	// Returns true if the value v of the given empty cell (r,c) 
	// has conflict with other cells in the same column.
	bool ifColConflict(short r, short c, unsigned char v) const {
		for (int i = 0; i < size; i++)
			// For non-empty same-col cells.
			if (!ifEmpty(i,c) && i != r && board[i][c] == v)
				return true;
		return false;
	}

	// Returns true if the value v of the given empty cell (r,c) 
	// has conflict with other cells in the same box (region).
	bool ifBoxConflict(short r, short c, unsigned char v) const {
		short rBox0 = r - r % sizeBox; // Starting row number for the box.
		short cBox0 = c - c % sizeBox; // Starting col number for the box.
		for (int i = rBox0; i < rBox0 + sizeBox; i++) 
			for (int j = cBox0; j < cBox0 + sizeBox; j++) 
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

	// Check if a given board has any conflict for all non-empty cells.
	// (returns true if there is no conflict, and it has valid values)
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
// for recursions, and they will be represented by derived classes.
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
	// 		the size of the total board will be size^2.
	Sudoku(short sizeBox=3): board(sizeBox), depth() {};

	// Read the initial puzzle.
	void read(string inFilename) {
		board.read(inFilename);
		initialize();
	};

	// Write the solution.
	void write(string outFilename) {
		board.write(outFilename);
	};

	// Solve the sudoku using the recursive backtracking algorithm.
	bool solve(); // Not defined for the base class.

   protected:
   	// Initialize the information for the possibilities of all empty cells.
	virtual void initialize()=0;

	// Find the next cell to explore and returns true if successful.
	// The position and possible values of the given cell are 
	// 	returned as arguments.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values)=0;

   protected:
   	// Board for the puzzle.
	Board board;
	// Depth of the current recursive step. (1st step will be depth 1).
	short depth;
};

// Derived class of the sudoku class (method 1).
// 1, Get empty cells from the initial puzzle and store them in an array
// 	using the spatial order starting from the top left to the right.
class Sudoku1: public Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be size^2.
	Sudoku1(short sizeBox=3): Sudoku(sizeBox), emptyCells(), \
		possibleValues() {};

   protected:
   	// Initialize the information for the possibilities of all empty cells.
	// in a map container.
	virtual void initialize();

	// Find the next cell to explore and returns true if successful.
	// The position and possible values of the given cell are 
	// 	returned as arguments.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values);
	Pos getNextCell();

   protected:
	// Arrays for storing possible values for all empty cells.
	vector<Pos> emptyCells;
	vector< vector<unsigned char> > possibleValues;
};

// Derived class of the sudoku class (method 2).
// 2, Find all possible numbers for all empty cells from the initial
// 	puzzle, and sort them by the number of possible numbers in an
// 	ascending order. Get the next one using this order.
class Sudoku2: public Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be size^2.
	Sudoku2(short sizeBox=3): Sudoku(sizeBox) {};

   protected:
   	// Initialize the information for the possibilities of all empty cells.
	virtual void initialize();

	// Find the next cell to explore and returns true if successful.
	// The position and possible values of the given cell are 
	// 	returned as arguments.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values);
	Pos getNextCell();

   protected:
	// Arrays for storing possible values for all empty cells.
	vector<Pos> emptyCells;
	vector< vector<unsigned char> > possibleValues;
};

// Derived class of the sudoku class (method 3).
// 3, Find all possible numbers for all empty cells at each step, and
// 	choose the cell with the smallest number of possibilities
// 	as the next cell to explore.
class Sudoku3: public Sudoku {
   public:
	// Constructor. 
	// 	sizeBox: size of the box in the board (default: 3).
	// 		the size of the total board will be size^2.
	Sudoku3(short sizeBox=3): Sudoku(sizeBox) {};

   protected:
   	// Nothing to initialize for this method.
	virtual void initialize() {};

	// Find the next cell to explore and returns true if successful.
	// The position and possible values of the given cell are 
	// 	returned as arguments.
	// In this case, it will choose the cell with the smallest number 
	// of possibilities by searching all remaining empty cells.
	virtual bool getNextCell(short &r, short &c, vector<unsigned char> &values);
	Pos getNextCell();

   protected:
	// Nothing extra to store for this method.
};
