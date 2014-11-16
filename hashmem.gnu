set terminal postscript enhanced monochrome dashed 
#set terminal png
set output "hashmem.ps"
set multiplot
set style data lines
set format y "%6.0f"
set style line 1 linetype 1 lw 2
set style line 2 linetype 2 lw 2
set title "Memory usage of hashtable vs list"
set ylabel "Real memory (bytes)"
set xlabel "File size (bytes)"
set key inside right center vertical
# common margins
set lmargin 10
set rmargin 2
# common y-range
plot "hashmem.txt" using 1:2 smooth csplines ls 2 title "list", "hashmem.txt" using 1:3 smooth csplines ls 1 title "hashtable (6+)"
