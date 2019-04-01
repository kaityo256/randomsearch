set term png
set out "q2_uasets.png"

set xla "Time [ms]"
set yla "Rating [q2]"

p "use_uasets/q2.dat" w l t "Use UAsets"\
,"fullsearch/q2.dat" w l t "Full Search"

