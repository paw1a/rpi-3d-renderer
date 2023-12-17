#!/opt/homebrew/bin/gnuplot -persist

reset

set terminal svg size 1280, 720 font "Helvetica,18"
set output 'plot1.svg'
set size ratio 0.56
set pointsize 0.9

set key right bottom
set xlabel "Кол-во полигонов, шт."
set ylabel "Время, мс"
set grid

plot "./data.txt" using 1:2 with linespoints title 'Модифицированный алгоритм Варнока' pt 9,\
     "./data.txt" using 1:3 with linespoints title 'Классический алгоритм Варнока' pt 10
