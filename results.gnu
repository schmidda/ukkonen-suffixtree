set terminal postscript enhanced monochrome dashed 
#set terminal png
set output "results.ps"
set style data lines
set style line 1 linetype 1 lw 2
set title "Ukkonen's suffix tree algorithm"
set ylabel "microseconds"
set xlabel "bytes"
set key inside right center vertical
# common margins
set lmargin 10
set rmargin 2
# common y-range
plot "results.txt" using 2:3 smooth csplines ls 1 title "Time vs size"
