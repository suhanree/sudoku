//
//	File: sudoku.C
//	Date: November 2014
//	Compiler: g++ (GCC) 4.4.7 20120313 (Red Hat 4.4.7-4)
//
//	Programmed by Suhan Ree for Insight Engineering Fellows Coding Challenge.
//
//	The program solves the puzzle Sudoku.
//
//	Everything is put into two files (header & source) 
//		to make it easier to read and compile.
//

#include "sudoku.h"

// Initializes the board by getting the puzzle from a csv file.
// 0 means empty (an example of a row: 0,0,1,0,0,2,0,0,3)
void Board::read(string filename) {
	std::ifstream inFile(filename.c_str(), std::ios::in);
	// If the file does not exist, throw an error.
	if (!inFile) throw NoInputFile();

	// Temporary strings to read input.
	string temp;
	vector<string> lines;
	// Read all lines first and store them in an array,
	while (getline(inFile, temp)) lines.push_back(temp);
	if (lines.size() != size) throw BadInput(); // Wrong row counts.

	// For each line, extract values using string streams and set values.
	for (short i = 0; i < size; i++) {
		std::istringstream ss(lines[i]);
		vector<unsigned char> values;
		while (getline(ss, temp, ',')) 
			values.push_back(std::atoi(temp.c_str()));
		// Check for wrong col counts.
		if (values.size() != size) throw BadInput(); 
		for (short j = 0; j < size; j++) set(i, j, values[j]);
	};
	inFile.close();
	// Check for the validity of the given puzzle.
	if (!ifValid()) throw InvalidPuzzle();
};

// Writes current contents of the board to a csv file.
// (Uses the same format as input.)
void Board::write(string filename) {
	std::ofstream outFile(filename.c_str(), std::ios::out);
	for (short i = 0; i < size; i++)
		for (short j = 0; j < size; j++)
			outFile << (short) board[i][j] << (j < size-1 ? ',' : '\n');
	outFile.close();
};

// Sets the value of a cell (r,c) to v.
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


// Checks if a given board has any conflict for non-empty cells.
// Returns true if there is no conflict, and it has valid values.
// It will be used to check the validity of the initial puzzle.
// 	(We still don't know if the puzzle is solvable or not.
// 	It can still have no solution or multiple solutions.)
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
	return true; // The puzzle passes the initial test.
};

// Solves function for the sudoku class.
// Basic algorithm is implemented here. Doesn't need to be overloaded.
bool Sudoku::solve() {
	// If there is no empty cells, a solution is found.
	// (Note that there can be more than one solutions.)
	// Also get the next cell to explore and increase the depth by 1.
	short r, c;
	vector<unsigned char> values;
	if (!getNextCell(r, c, values)) // r, c & values are returned here.
		return true; // No more empty cell.
	depth++;

	// For all possible values for the given cell.
	for (short i = 0; i < values.size(); i++) {
		// If there is no conflict with the given value.
		if (!board.ifAnyConflict(r, c, values[i])) {
			// Set the cell's value temporarily.
			board.set(r, c, values[i]);
			// Keep going to the next cell recursively
			// until the given value doesn't work out.
			if (solve()) return true;
			// Comes here if the value didn't work,
			// so reset the cell as empty and try another value.
			board.makeEmpty(r, c);
			depth--; // decrease the depth by 1.
		};
	};
	// If there is no possible value, or all possible values didn't 
	// work out, triggers the backtracking.
	return false; 
};

// Initializes the information for the possibilities of all empty cells
// using two arrays for positions and possibilities.
// The order the empty cells are stored is spatial, which means that,
// starting at the top-left corner, it goes from left to right, 
// and top to bottom.
void Sudoku1::initialize() {
	short size = board.getSize();
	for (short i = 0; i < size; i++) // Top to bottom.
		for (short j = 0; j < size; j++) // Left to right.
			if (board.ifEmpty(i, j)) { // For empty cells,
				// Store positions to a vector.
				emptyCells.push_back(Pos(i, j));
				vector<unsigned char> values;
				for (unsigned char v = 1; v <= size;v++)
					if (!board.ifAnyConflict(i, j, v))
						values.push_back(v);
				// If there is no possible value, invalid.
				if (values.size() == 0) throw InvalidPuzzle();
				// Store possible values to a vector.
				possibleValues.push_back(values);
			};
};

// Finds the next cell to explore (returns true if successful).
// The position and possible values of the given cell are 
// 	returned as arguments.
// The order the empty cells are chosen is spatial, which means that,
// starting at the top-left corner, it goes from left to right, 
// and top to bottom.
// The order is easy to find, because it was predetermined already using
// 'initialize' and it was stored in arrays.
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
	
// Initializes the information for the possibilities of all empty cells
// using two arrays for positions and possibilities.
// The order the empty cells are stored are sorted by the number of possible
// values.
void Sudoku2::initialize() {
	short size = board.getSize();
	// To sort by the number of possible values, we use map containers.
	std::multimap<short, Pos> numPossibles;
	std::map<Pos, vector<unsigned char>, PosLessThan> emptyCellInfo;
	// Find empty cells, and store their info in maps.
	for (short i = 0; i < size; i++) 
		for (short j = 0; j < size; j++) 
			if (board.ifEmpty(i, j)) { // For all empty cells.
				// Find all possible values.
				vector<unsigned char> values;
				for (unsigned char v = 1; v <= size;v++)
					if (!board.ifAnyConflict(i, j, v))
						values.push_back(v);
				// If no possible values, throw an error.
				if (values.size() == 0) throw InvalidPuzzle();
				// Store the info into maps.
				numPossibles.insert(std::pair<short, Pos>( \
					values.size(), Pos(i, j)));
				emptyCellInfo[Pos(i, j)] = values;
			};
	// Using maps, store them into arrays in the sorted order.
	// Arrays will be used in 'solve'.
	for (std::multimap<short, Pos>::const_iterator it = \
		numPossibles.begin(); it != numPossibles.end(); it++) {
		emptyCells.push_back(it->second);
		possibleValues.push_back(emptyCellInfo[it->second]);
	};
};

// Finds the next cell to explore and returns true if successful.
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
	short min = 101; // 4 <= (number of possible values) <= 100.
	// Search all empty cells, find a cell with the minimum number of 
	// possibilities at this moment.
	for (short i = 0; i < size; i++) 
		for (short j = 0; j < size; j++) 
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
	r = rmin; // Setting row & col number for the cell with the minimum 
	c = cmin; // 	number of possibilities.

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
	// Filenames for input and output csv files. 
	// (input: puzzle, output: solution)
	string inFilename(argv[1]), outFilename(argv[2]);
	const short size = 9; // The given size of the board of Sudoku.
	try {
		// Create a (size x size) sodoku puzzle (default: 9x9).
		Sudoku3 puzzle;
		cout << "* Created " << size << 'x' << size \
			<< " Sodoku puzzle." << endl;
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
};

