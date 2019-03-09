#include <iostream>
#include "sudoku_lib/grid.hpp"
#include "sudoku_lib/ansmaker.hpp"

int main(){
  AnsMaker am;
  std::cout << am.make() << std::endl;
}
