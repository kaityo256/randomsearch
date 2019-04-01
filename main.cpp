#include "cmdline.h"
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

//グローバルフラグ
bool calc_sc125f = true;     // sc125fを計算する
bool calc_gsfq2 = true;      // gsf's q2を計算する
bool use_fullsearch = false; //途中から全探索に切り替える
bool use_uasets = false;     // 解の一意性判定に不可避集合を用いる

// プログラム開始時からの経過時間(ms)
long long myclock() {
  static const auto s = std::chrono::system_clock::now();
  const auto e = std::chrono::system_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
  return elapsed;
}

// 得られた数独の問題の難易度と取得時刻を返す
void show(std::string &str) {
  static long long q2max = 0;
  static int scmax = 0;
  if (calc_gsfq2) {
    auto r = rater.rate(str.c_str());
    if (q2max < r) {
      q2max = r;
      ofs_q2 << str << " ";
      ofs_q2 << r << " ";
      ofs_q2 << myclock() << std::endl;
    }
  }
  if (calc_sc125f) {
    int sc = sc125f::calc(str.c_str());
    if (scmax < sc) {
      scmax = sc;
      ofs_sc << str << " ";
      ofs_sc << sc << " ";
      ofs_sc << myclock() << std::endl;
    }
  }
}

// 不可避集合にヒットしたか?
bool hits_uasets(const mbit &m) {
  for (auto m2 : uasets) {
    if (!(m & m2)) return false;
  }
  return true;
}

bool is_unique(const mbit &m, const std::string &answer) {
  // 不可避集合とantaさんの一意性判定ルーチンを利用
  if (use_uasets) {
    if (!hits_uasets(m)) return false;
    uint64_t *n = (uint64_t *)(&m);
    const Mask128 mm(n[0], n[1]);
    return uc.isSolutionUnique(mm);
  } else {
    // 自前の一意性判定ルーチンを利用
    std::string str = mbit2str(m, answer);
    return Grid::is_unique(str);
  }
}

// 途中までランダムに掘った数独から、掘れるだけ掘って探す
void fullsearch(mbit s1, mbit s2, const std::string &answer) {
  if (s2 == 0) {
    if (!is_unique(s1, answer)) return;
    std::string str = mbit2str(s1, answer);
    show(str);
    return;
  }
  mbit lsb = (s2 & -s2);
  s2 ^= lsb;
  fullsearch(s1, s2, answer);
  mbit s3 = s1 ^ lsb;
  if (!is_unique(s3, answer)) return;
  fullsearch(s3, s2, answer);
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
    if (!is_unique(m2, answer)) {
      if (!use_fullsearch) {
        std::string str = mbit2str(m, answer);
        show(str);
      }
      return;
    }
    hints--;
    m = m2;
    if (use_fullsearch) {
      if (hints == 30) {
        fullsearch(m, m, answer);
        return;
      }
    }
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

// 不可避集合のビットパターンリストアップ
void setup_uasets(const std::string &answer) {
  uc.setSolution(answer.c_str());
  int max_uasize = 8;
  uafinder.findAll(answer.c_str(), max_uasize);
  for (int i = 0; i <= max_uasize; i++) {
    auto ualist = uafinder.getList(i);
    for (const auto &mask : uafinder.getList(i)) {
      uasets.push_back(mask81tombit(mask));
    }
  }
}

void test(const int seed = 0) {
  std::mt19937 mt;
  AnsMaker am(seed);
  mt.seed(seed);
  auto ans = am.make();
  if (use_uasets) {
    setup_uasets(ans);
  }
  while (true) {
    dig(ans, mt);
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  cmdline::parser a;
  a.add<std::string>("sc125f", 's', "Calculate sc125f", false, "yes", cmdline::oneof<std::string>("yes", "no"));
  a.add<std::string>("gsfq2", 'q', "Calculate gsf's q2", false, "yes", cmdline::oneof<std::string>("yes", "no"));
  a.add<std::string>("fullsearch", 'f', "Use Fullsearch", false, "no", cmdline::oneof<std::string>("yes", "no"));
  a.add<std::string>("uasets", 'u', "Use UA Sets", false, "no", cmdline::oneof<std::string>("yes", "no"));
  a.parse_check(argc, argv);
  calc_sc125f = a.get<std::string>("sc125f") == "yes";
  calc_gsfq2 = a.get<std::string>("gsfq2") == "yes";
  use_fullsearch = a.get<std::string>("fullsearch") == "yes";
  use_uasets = a.get<std::string>("uasets") == "yes";
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    std::cout << "Calculate gsf's q2: " << a.get<std::string>("gsfq2") << std::endl;
    std::cout << "Calculate sc125f  : " << a.get<std::string>("sc125f") << std::endl;
    std::cout << "Use Fullsearch    : " << a.get<std::string>("fullsearch") << std::endl;
    std::cout << "Use UA Sets       : " << a.get<std::string>("uasets") << std::endl;
  }
  // File Open
  if (calc_sc125f) {
    char filename[256];
    sprintf(filename, "sc_%04d.dat", rank);
    ofs_sc.open(filename);
  }
  if (calc_gsfq2) {
    char filename[256];
    sprintf(filename, "q2_%04d.dat", rank);
    ofs_q2.open(filename);
  }

  test(rank);
  MPI_Finalize();
}
