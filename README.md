# 数独ランダム探索コード

## 概要

数独の問題をランダムに作成するコード。
ランダムに「すべて埋まった」問題を作成し、そこからヒントを抜く形で問題を作成していく。

## オプション

* `-q [yes/no]` gsf's q2を計算するかどうか (デフォルト:yes)
* `-s [yes/no]` sc125f を計算するかどうか  (デフォルト:yes)
* `-f [yes/no]` 全探索に切り替えるかどうか (デフォルト: no)
* `-u [yes/no]` 解の一意性判定に不可避集合を使うかどうか (デフォルト: no)

### 全探索について

全探索モードが有効である場合、ヒントをランダムに削っていって30ヒントに到達したら全探索、つまり「その問題からヒントを抜いて到達可能なすべての問題の列挙」に切り替える。そうでない場合は、ヒントをランダムに抜いていき、解が一意でなくなったら、その一ステップ前の問題を報告する。

## 結果

結果は`results`フォルダに入っている。

* simpledig (掘れるだけ掘るモード)
* fullsearch (全探索モード)

## ライセンス

* [cmpdline](https://github.com/tanakh/cmdline) (C) 2009, Hideyuki Tanaka 修正BSDライセンス
* [smr](https://github.com/kaityo256/smr) (C) anta GPLv3
* [sc125f](https://github.com/kaityo256/sc125f) (C) 川合秀実 川合堂ライセンス-01 ver.1.0
* 上記以外 (C) H. Watanabe MITライセンス