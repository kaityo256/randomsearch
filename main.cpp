#include "smr/rater.hpp"
#include "sudoku_lib/ansmaker.hpp"
#include "sudoku_lib/grid.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

Rater rater(100000);

// このマクロが有効な場合、途中から全探索に切り替える
//#define FULLSEARCH

// プログラム開始時からの経過時間(ms)
auto myclock() {
  static const auto s = std::chrono::system_clock::now();
  const auto e = std::chrono::system_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
  return elapsed;
}

// 得られた数独の問題の難易度と取得時刻を返す
void show(std::string &str) {
  static long long q2max = 0;
  auto r = rater.rate(str.c_str());
  if(q2max >= r){
    return;
  }
  q2max = r;
  std::cout << str << " ";
  std::cout << r << " ";
  std::cout << myclock() << std::endl;
}

// 途中までランダムに掘った数独から、掘れるだけ掘って探す
void search(mbit s1, mbit s2, const std::string &answer) {
  if (s2 == 0) {
    std::string str = mbit2str(s1, answer);
    if (!Grid::is_unique(str)) return;
    show(str);
    //std::cout << str << std::endl;
    return;
  }
  mbit lsb = (s2 & -s2);
  s2 ^= lsb;
  search(s1, s2, answer);
  mbit s3 = s1 ^ lsb;
  std::string str2 = mbit2str(s3, answer);
  if (!Grid::is_unique(str2)) return;
  search(s3, s2, answer);
}

mbit random_remove(const mbit &s, std::mt19937 &mt) {
  mbit t = s;
  std::uniform_int_distribution<> ud(0, popcnt_u128(s) - 1);
  const unsigned int n = ud(mt);
  for (unsigned int i = 0; i < n; i++) {
    t = t ^ (t & -t);
  }
  return (s ^ (t & -t));
}

void dig(const std::string &answer, std::mt19937 &mt) {
  mbit m = mask81;
  int hints = 81;
  for (int i = 0; i < 100; i++) {
    mbit m2 = random_remove(m, mt);
    std::string s = mbit2str(m2, answer);
    if (!Grid::is_unique(s)) {
#ifndef FULLSEARCH
      std::string str = mbit2str(m, answer);
      show(str);
      //std::cout << str << std::endl;
#endif
      return;
    }
    hints--;
    m = m2;
#ifdef FULLSEARCH
    if (hints == 30) {
      //std::cout << s << std::endl;
      search(m, m, answer);
      exit(1);
      return;
    }
#endif
  }
}

void test(const int seed = 0) {
  std::mt19937 mt;
  AnsMaker am(seed);
  mt.seed(seed);
  auto ans = am.make();
  while (true) {
    dig(ans, mt);
  }
}

int main() {
  //mbit m = mbit(243);
  //search(m, m);
  test();
}
