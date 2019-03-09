#include "sudoku_lib/ansmaker.hpp"
#include "sudoku_lib/grid.hpp"
#include <algorithm>
#include <iostream>
#include <random>

std::mt19937 mt;

mbit random_remove(const mbit &s, std::mt19937 &mt) {
  mbit t = s;
  std::uniform_int_distribution<> ud(0, popcnt_u128(s) - 1);
  const unsigned int n = ud(mt);
  for (unsigned int i = 0; i < n; i++) {
    t = t ^ (t & -t);
  }
  return (s ^ (t & -t));
}

void dig(const std::string &answer) {
  mbit m = mask81;
  int hints = 81;
  for (int i = 0; i < 1000; i++) {
    mbit m2 = random_remove(m, mt);
    std::string s = mbit2str(m2, answer);
    hints--;
    m = m2;
    if (hints == 30) {
      std::cout << m << std::endl;
      return;
    }
  }
}

int main() {
  const int seed = 0;
  AnsMaker am(seed);
  mt.seed(seed);
  dig(am.make());
}
