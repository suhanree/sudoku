//
//	File: sudoku.C
//	Date: October 2014
//	Compiler: g++ (GCC) 4.4.7 20120313 (Red Hat 4.4.7-4)
//
//	Programmed by Suhan Ree for Insight Engineering Fellows Code Challenge.
//
//	The program solve the puzzle Sudoku.
//
//	Everything is put into one file to make it easier to read and compile.
//


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "sudoku.h"

// initialize the board (get the puzzle)(returns false if unsuccessful)
// from a csv file.
void Board::read(string filename) {
	std::ifstream inFile(filename.c_str(), std::ios::in);
	if (!inFile) throw NoInputFile();
	string temp;
	vector<string> lines;
	// Read all lines first and store them in an array,
	while (getline(inFile, temp)) lines.push_back(temp);
	if (lines.size() != size) throw BadInput(); // Wrong row counts.

	for (int i = 0; i < size; i++) {
		std::istringstream ss(lines[i]);
		vector<short> values;
		while (getline(ss, temp, ',')) 
			values.push_back(std::atoi(temp.c_str()));
		// Check for wrong col counts.
		if (values.size() != size) throw BadInput(); 
		for (int j = 0; j < size; j++) set(i, j, values[j]);
	};
	inFile.close();
	if (!ifValid()) throw InvalidPuzzle();
};

// Write the contents of the board to a csv file.
void Board::write(string filename) {
	std::ofstream outFile(filename.c_str(), std::ios::out);
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			outFile << (int) board[i][j] << (j < size-1 ? ',' : '\n');
};

// Set the value of a cell (r,c) to v.
void Board::set(short r, short c, unsigned char v) {
	if (v >= 1 && v <= size) { // For valid values only.
		if (ifEmpty(r,c)) { // for empty cells.
			empty[r][c] = false;
			nEmpty--;
		};
		board[r][c] = v;
	}
	else if (v == 0) makeEmpty(r, c); // v=0 means empty.
	else throw BadValue(); // v is not valid.
}; 


// Check if a given board has any conflict for all non-empty cells.
// (returns true if there is no conflict, and it has valid values)
// 	(We still don't know if the puzzle is solvable or not.
// 	It can have either no solution or multiple solution.)
bool Board::ifValid() const {
	for (short r = 0; r < size; r++)
		for (short c = 0; c < size; c++)
			if (!ifEmpty(r,c)) { // For all non-empty cells
				unsigned char v = board[r][c];
				// Check values and any conflict.
				if (v < 1 || v > size || \
					ifAnyConflict(r,c,v))
					return false;
			};
	return true; // No problem found.
};

// Solve function for the sudoku class (basic algorithm is implemented here).
bool Sudoku::solve() {
	// If there is no empty cells, a solution is found.
	// (Note that there can be more solutions.)
	// Also get the next cell to explore and increase the depth by 1.
	short r, c;
	vector<unsigned char> values;
	if (!getNextCell(r, c, values)) 
		return true; // No more empty cell.
	depth++;

	// For all possible values for the given cell.
	for (int i = 0; i < values.size(); i++) {
		// If there is no conflict with the given value.
		if (!board.ifAnyConflict(r, c, values[i])) {
			// Set the cell's value temporarily.
			board.set(r, c, values[i]);
			// Go to the next cell, and if true
			if (solve()) return true;
			// Reset the cell as empty.
			board.makeEmpty(r, c);
			depth--; // decrease the depth by 1.
		};
	};
	return false; // Triggering the backtracking because nothing works.
};

// Initialize the information for the possibilities of all empty cells
// using two arrays for positions and possibilities.
// The order the empty cells are stored are spatial, which means that,
// starting at the top-left corner, it goes from left to right, 
// and top to bottom.
void Sudoku1::initialize() {
	short size = board.getSize();
	for (int i = 0; i < size; i++) // Top to bottom.
		for (int j = 0; j < size; j++) // Left to right.
			if (board.ifEmpty(i, j)) { // For empty cells,
				// store positions.
				emptyCells.push_back(Pos(i, j));
				vector<unsigned char> values;
				for (unsigned char v = 1; v <= size;v++)
					if (!board.ifAnyConflict(i, j, v))
						values.push_back(v);
				// If there is no possible value, invalid.
				if (values.size() == 0) throw InvalidPuzzle();
				// store possible values.
				possibleValues.push_back(values);
			};
};

// Find the next cell to explore and returns true if successful.
// The position and possible values of the given cell are 
// 	returned as arguments.
bool Sudoku1::getNextCell(short &r, short &c, vector<unsigned char> &values) {
	// depth starts from 0 to (number of empty cells)-1.
	if (depth < emptyCells.size()) {
		r = emptyCells[depth].r;
		c = emptyCells[depth].c;
		values = possibleValues[depth];
		return true; // Empty cell found, and its info returned.
	}
	else return false; // No more empty cell.
};
	
// Initialize the information for the possibilities of all empty cells
// using two arrays for positions and possibilities.
// The order the empty cells are stored are sorted by the number of possible
// values.
void Sudoku2::initialize() {
	short size = board.getSize();
	// To sort by the number of possible values, we use map containers.
	std::multimap<short, Pos> numPossibles;
	std::map<Pos, vector<unsigned char>, PosLessThan> emptyCellInfo;
	// Find empty cells, and store their info in maps.
	for (int i = 0; i < size; i++) 
		for (int j = 0; j < size; j++) 
			if (board.ifEmpty(i, j)) {
				vector<unsigned char> values;
				for (unsigned char v = 1; v <= size;v++)
					if (!board.ifAnyConflict(i, j, v))
						values.push_back(v);
				if (values.size() == 0) throw InvalidPuzzle();
				numPossibles.insert(std::pair<short, Pos>( \
					values.size(), Pos(i, j)));
				emptyCellInfo[Pos(i, j)] = values;
			};
	// From maps, store them into arrays (sorted, to be used in 'solve()').
	for (std::multimap<short, Pos>::const_iterator it = \
		numPossibles.begin(); it != numPossibles.end(); it++) {
		emptyCells.push_back(it->second);
		possibleValues.push_back(emptyCellInfo[it->second]);
	};
};

// Find the next cell to explore and returns true if successful.
// The position and possible values of the given cell are 
// 	returned as arguments.
bool Sudoku2::getNextCell(short &r, short &c, vector<unsigned char> &values) {
	if (depth < emptyCells.size()) {
		r = emptyCells[depth].r;
		c = emptyCells[depth].c;
		values = possibleValues[depth];
		return true;
	}
	else return false;
};
	
// Find the next cell to explore and returns true if successful.
// The position and possible values of the given cell are 
// 	returned as arguments.
// In this case, it will choose the cell with the smallest number 
// of possibilities by searching all remaining empty cells.
bool Sudoku3::getNextCell(short &r, short &c, vector<unsigned char> &values) {
	// If there is no empty cell left, return false;
	if (board.getNEmpty() == 0) return false;

	// Temporary row and col numbers for the cell with minimum number
	// of possilities.
	short rmin = 0, cmin = 0;  

	short size = board.getSize(); // default: 9.
	short min = 100;
	// Search all empty cells, find a cell with the minimum number of 
	// possibilities at this moment.
	for (int i = 0; i < size; i++) 
		for (int j = 0; j < size; j++) 
			if (board.ifEmpty(i,j)) { // For all empty cells
				short count = 0;
				for (unsigned char v = 1; v <= size; v++)
					if (!board.ifAnyConflict(i, j, v))
						count++;
				if (count < min) {
					min = count;
					rmin = i; cmin = j;
				};
			};
	// Find possible values for this cell and store it to values.
	values.clear();
	for (unsigned char v = 1; v <= size; v++)
		if (!board.ifAnyConflict(rmin, cmin, v))
			values.push_back(v);
	return true;
};
	



// Main function (driver).
int main(int argc, char **argv) {
	if (argc !=3) {
		std::cerr << "# Usage: <execution filename> <input filename> ";
		std::cerr << "<output filename> \n";
		std::cerr << "   Example: $ a.out in.csv out.csv\n";
		std::exit(1);
	};
	string inFilename(argv[1]), outFilename(argv[2]);
	try {
		// Create a 9x9 sodoku puzzle.
		Sudoku1 puzzle;
		cout << "* Created 9x9 Sodoku puzzle." << endl;
		// Reading the file.
		cout << "* Reading the input file, " << inFilename << endl;
		puzzle.read(inFilename);
		cout << "* Solving..." << endl;
		if (puzzle.solve()) {
			cout << "* Solved, and writing to the output file, " \
			<< outFilename << endl;
			puzzle.write(outFilename);
		}
		else
			cout << "* Unsolvable puzzle. Nothing is written." << endl;
	}
	catch(BadSize e) {
		std::cerr << "* Bad size of the board (grid) is given.\n";
		std::exit(1);
	}
	catch(BadInput e) {
		std::cerr << "* Bad input is given.\n";
		std::exit(1);
	}
	catch(BadValue e) {
		std::cerr << "* Bad value is given in the input.\n";
		std::exit(1);
	}
	catch(NoInputFile e) {
		std::cerr << "* File, " << inFilename << ", does not exist.\n";
		std::exit(1);
	}
	catch(InvalidPuzzle e) {
		std::cerr << "* Given puzzle is invalid.\n";
		std::cerr << "  Causes: (1) Any given value is not following" \
			<< " the rule, or (2) any empty cell has no possible" \
			<< " value.\n";
		std::exit(1);
	};
	return 0;
}

