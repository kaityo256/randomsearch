#include "sudoku_lib/ansmaker.hpp"
#include "sudoku_lib/grid.hpp"
#include <iostream>

void dig(const std::string &answer) {
  mbit m = mbit81mask;
  int hints = 81;
  for (int i = 0; i < 1000; i++) {
    mbit m2 = Mbit::random_remove(m, mt);
    std::string s = mbit2str(m2, answer);
    if (is_unique(m2)) {
      hints--;
      m = m2;
      if (hints == 30) {
        ts.look_around(m, answer, mout);
        return;
      }
    }
  }
}

int main() {
  AnsMaker am;
  dig(am.make());
}
