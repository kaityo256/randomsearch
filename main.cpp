#include "sc125f/sc125fa.hpp"
#include "smr/rater.hpp"
#include "smr/uafinder.hpp"
#include "smr/uniquenesschecker.hpp"
#include "sudoku_lib/ansmaker.hpp"
#include "sudoku_lib/grid.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <random>

Rater rater(100000);
UAFinder uafinder;
std::vector<mbit> uasets;
UniquenessChecker uc;

std::ofstream ofs_q2;
std::ofstream ofs_sc;

// このマクロが有効な場合、途中から全探索に切り替える
//#define FULLSEARCH

// プログラム開始時からの経過時間(ms)
const long long myclock() {
  static const auto s = std::chrono::system_clock::now();
  const auto e = std::chrono::system_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
  return elapsed;
}

// 得られた数独の問題の難易度と取得時刻を返す
void show(std::string &str) {
  static long long q2max = 0;
  static int scmax = 0;
  auto r = rater.rate(str.c_str());
  if (q2max < r) {
    q2max = r;
    ofs_q2 << str << " ";
    ofs_q2 << r << " ";
    ofs_q2 << myclock() << std::endl;
  }
  int sc = sc125f::calc(str.c_str());
  if (scmax < sc) {
    scmax = sc;
    ofs_sc << str << " ";
    ofs_sc << sc << " ";
    ofs_sc << myclock() << std::endl;
  }
}

// 不可避集合にヒットしたか?
bool hits_uasets(const mbit &m) {
  for (auto m2 : uasets) {
    if (!(m & m2)) return false;
  }
  return true;
}

bool is_unique(const mbit &m) {
  if (!hits_uasets(m)) return false;
  uint64_t *n = (uint64_t *)(&m);
  const Mask128 mm(n[0], n[1]);
  return uc.isSolutionUnique(mm);
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

//途中までランダムに掘った数独から、掘れるだけ掘って探す(不可避集合利用版)
// TODO: 上とまとめること
void search_withuc(mbit s1, mbit s2, const std::string &answer) {
  if (s2 == 0) {
    if (!is_unique(s1)) return;
    std::string str = mbit2str(s1, answer);
    show(str);
    return;
  }
  mbit lsb = (s2 & -s2);
  s2 ^= lsb;
  search(s1, s2, answer);
  mbit s3 = s1 ^ lsb;
  std::string str2 = mbit2str(s3, answer);
  if (!is_unique(s3)) return;
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
      search(m, m, answer);
      return;
    }
#endif
  }
}

void test(const int seed = 0) {
  std::mt19937 mt;
  AnsMaker am(seed);
  mt.seed(seed);
  char filename[256];
  sprintf(filename, "sc_%04d.dat", seed);
  ofs_sc.open(filename);
  sprintf(filename, "q2_%04d.dat", seed);
  ofs_q2.open(filename);
  auto ans = am.make();
  while (true) {
    dig(ans, mt);
  }
}

void use_uasets(const int seed = 0) {
  std::mt19937 mt;
  AnsMaker am(seed);
  mt.seed(seed);
  char filename[256];
  sprintf(filename, "sc_%04d.dat", seed);
  ofs_sc.open(filename);
  sprintf(filename, "q2_%04d.dat", seed);
  ofs_q2.open(filename);
  auto ans = am.make();
  uasets.clear();
  uafinder.findAll(ans.c_str(), 4);
  while (true) {
    dig(ans, mt);
  }
}

mbit mask81tombit(const Mask81 &mask) {
  mbit m = mask.getData(2);
  m = m << 32;
  m = m | mask.getData(1);
  m = m << 32;
  m = m | mask.getData(0);
  return m;
}

void dig_withuc(const std::string &answer, std::mt19937 &mt) {
  mbit m = mask81;
  int hints = 81;
  for (int i = 0; i < 100; i++) {
    mbit m2 = random_remove(m, mt);
    std::string str = mbit2str(m2, answer);
    if (!is_unique(m2)) {
      return;
    }
    hints--;
    m = m2;
    if (hints == 30) {
      search_withuc(m, m, answer);
      return;
    }
  }
}

void uatest(const int seed = 0) {
  std::mt19937 mt;
  AnsMaker am(seed);
  mt.seed(seed);
  char filename[256];
  sprintf(filename, "sc_%04d.dat", seed);
  ofs_sc.open(filename);
  sprintf(filename, "q2_%04d.dat", seed);
  ofs_q2.open(filename);

  auto ans = am.make();
  uc.setSolution(ans.c_str());
  // 不可避集合のリストアップ
  int max_uasize = 8;
  uafinder.findAll(ans.c_str(), max_uasize);
  for (int i = 0; i <= max_uasize; i++) {
    auto ualist = uafinder.getList(i);
    for (const auto &mask : uafinder.getList(i)) {
      uasets.push_back(mask81tombit(mask));
    }
  }
  while (true) {
    dig_withuc(ans, mt);
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //test(rank);
  uatest(rank);
  MPI_Finalize();
}
