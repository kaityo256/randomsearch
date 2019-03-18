set term png
set out "q2.png"

set xla "Time [ms]"
set yla "Rating [q2]"

p "simpledig/q2.dat" w l t "Simple Dig"\
,"fullsearch/q2.dat" w l t "Full Search"

set out "sc.png"

set xla "Time [ms]"
set yla "Rating [sc]"

p "simpledig/sc.dat" w l t "Simple Dig"\
,"fullsearch/sc.dat" w l t "Full Search"


