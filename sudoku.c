#include "sudoku.h"
#include "array.h"
#include "sort.h"

const int DIM = 9;

static const int EMPTY = 0;
static const char BLANK = '-';
static const int MIN = 1;
static const int MAX = 9;

static const int SUCCESS = 0;
static const int ERROR = -1;
static const int ERASE_EMPTY_CELL = -2;
static const int ERASE_FILLED_CELL = -3;
static const int ERROR_NEXT_CELL = -4;

struct sudoku {
  int puzzle[DIM * DIM];
  int sol[DIM * DIM];
};

// You need to implement the following four functions.

// order(a, len) gets an array and its length as integer len
// orders in a way that all items gets placed to indexes one
// smaller than the numverical value of the i.e 
// (2, 0, 3, 0 ,4, 6 ,1 ,0)
// gets ordered like that (1, 2, 3, 4, 0, 6, 0, 0)
// effects: mutates array a
// time: O(n) where n is len
static void order(int a[], int len){
  int b[len];
  for(int i = 0; i < len; ++i){
   b[i] = 0;
  }
  for(int i = 0; i < len; i++){
    if(a[i] != 0){
      b[a[i] - 1] = a[i];
    }
  }
  for(int i = 0; i < len; i++){ 
    a[i] = b[i];
  }
}

// fill_cell(s,row,col,num) tries to fill num in the cell (row,col).  
// It returns 0 if doing so does not violate any of the row, column, 
// and box constraints.  Otherwise, it returns a negative integer.
// requires: row and col are valid indices. 
//           num is an integer between 1 and 9.
// effects:  mutates *s
// time: O(1) 
int fill_cell(struct sudoku *s, int row, int col, int num) {
  if(num > MAX || num < MIN){
    return ERROR;
  }
  
  int j = row * MAX;
  
  if(s->sol[j + col] != EMPTY){
    return ERROR;
  }
  for(int i = 0; i < DIM; ++i){
    int a;
    int b;
    a = s->sol[i * (MAX) + col];
    b = s->sol[j];
    
    if(a == num || b == num){
      return ERROR;
    }
    j++;
  }
  s->sol[row * MAX + col] = num;
  return SUCCESS;
  
}

// choices_cell(s,row,col,choices,num_choices) determines all the 
// possible values for the cell (row,col) that do not violate
// any of the row, column, and box constraints.  It mutates choices 
// to contain the possible values and mutates *num_choices to be
// the number of possible values.
// requires: row and col are valid indices.
//           choices is a valid array with length DIM
// effects:  mutates choices and *num_choices
// time: O(1)
void choices_cell(const struct sudoku *s, int row, int col, 
                 int choices[], int *num_choices) {
  // creates arrays with a length of maximum allowed
  int row_a[DIM];
  int col_a[DIM];
  int j = row * MAX;
  // if the position is not empty there is no possible choice
  if(s->sol[j + col] != EMPTY){
    *num_choices = 0;
  }else{
    //fills the corresponding row and column of the position to the arrays
    for(int i = 0; i < DIM; ++i){
      col_a[i] = s->sol[i * MAX + col];
      row_a[i] = s->sol[j];
      j++;
    }
    // sorts the row and column puts 0 to empty indexes
    order(row_a, DIM);
    order(col_a, DIM);
    int counter = 0;
    // if both row and column have same numbers missing adds
    // to the choices and increments the total occurence
    for(int i = 0; i < DIM; ++i){
      if(row_a[i] == 0 && col_a[i] == 0){
        choices[counter] = i + 1;
        counter++;
      }
    }
    *num_choices = counter;
  }
}

// distinct(a, len) gets an array and checks wether all
// the elements are distinct or not
// time: O(n) where n is the len
static bool distinct(int a[], int len){
  merge_sort(a, len);
  for(int i = 0; i < len - 1; ++i){
    if(a[i] == a[i+1]){
      return false;
    }
  }
  return true;
}

// solved_puzzle(s) returns true if s has a valid solution to the puzzle, 
// and returns false otherwise.
// time: O(1)
bool solved_puzzle(const struct sudoku *s) {
  int a[DIM * DIM];
  int test[DIM];
  for(int i = 0; i < DIM * DIM; ++i){
    if(s->sol[i] == EMPTY){
      return false;
    }
    a[i] = s->sol[i];
  }
  // tests for each index in a row
  for(int row = 0; row< DIM; ++row){
    for(int col = 0; col< DIM; ++col){
      test[col] = a[row * DIM + col];
    }
    if(!distinct(test, DIM)){
      return false;
    }
  }
  // tests for each index in a column
  for(int col = 0; col < DIM; ++col){
    for(int row = 0; row < DIM; ++row){
      test[row] = a[row * DIM + col];
    }
    if(!distinct(test, DIM)){
      return false;
    }
  }
  return true;
}

// solve(s) solves the Sudoku puzzle by search and backtracking.  
// It mutates the 
// It returns true if s has a valid solution to the puzzle, 
// and returns false otherwise.
// effects: mutates *s
// time: O(1)
bool solve(struct sudoku *s) {

  int choices[DIM];
  int num_choices = 0;

  if(solved_puzzle(s)){
    return true;
  }
  int pos = 0;
  while(pos < DIM * DIM && s->sol[pos] != EMPTY){
    ++pos;
  }
  if(pos == DIM * DIM){
    return false;
  }
  choices_cell(s, pos / DIM, pos % DIM, choices, &num_choices);
  if(0 == num_choices){
    return false;
  }
  for(int j = 0; j < num_choices; ++j){
    if(fill_cell(s, pos / DIM, pos % DIM, choices[j]) == SUCCESS){
      return solve(s);
    }
  }
  s->sol[pos] = EMPTY;
  return false;
}

// There is no need to modify the rest of the functions.

struct sudoku *read_sudoku(void) {
  struct sudoku *s = malloc(sizeof(struct sudoku));
  char c = 0;
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
        scanf(" %c", &c);
      if (c == BLANK) {
        s->puzzle[row * DIM + col] = 0;
      } else {
        s->puzzle[row * DIM + col] = c - '0';
      }
    }
  }

  // copy puzzle to solution
  reset_sol(s);

  return s;
}

void sudoku_destroy(struct sudoku *s) {
  assert(s);
  free(s);
}

void print_sol(const struct sudoku *s) {
  assert(s);

  printf("\n");
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      int num = s->sol[row * DIM + col];
      if (num == EMPTY) {
        printf("%c", BLANK);
      } else {
        printf("%d", num);
      }  
    }
    printf("\n");
  }
  printf("\n");
}

void reset_sol(struct sudoku *s) {
  assert(s);

  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      s->sol[row * DIM + col] = s->puzzle[row * DIM + col];
    }
  }  
}

// cell_empty(board, row, col) returns true
// if cell (row,col) is empty on board.
// requires: board is a valid sudoku puzzle.
static bool cell_empty(const int board[], int row, int col) {
  assert(board);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  return board[row * DIM + col] == EMPTY;
}

int erase_cell(struct sudoku *s, int row, int col) {
  assert(s);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (cell_empty(s->sol, row, col)) {
    return ERASE_EMPTY_CELL;
  }
  if (!cell_empty(s->puzzle, row, col)) {
    return ERASE_FILLED_CELL;
  }
  s->sol[row * DIM + col] = EMPTY;
  return SUCCESS;
}


int next_cell(const struct sudoku *s, int *row, int *col) {
  assert(s);
  assert(row);
  assert(col);

  int choices[DIM];
  int num_choices = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (!cell_empty(s->sol, i, j)) continue;
      choices_cell(s, i, j, choices, &num_choices);
      if (num_choices == 1) {
        *row = i;
        *col = j;
        return SUCCESS;
      }
    }
  }
  return ERROR_NEXT_CELL;
}
