FILE=out.tex

echo "" > out.tex

cat head.tex >> $FILE
echo "\section{Object statistics}" >> $FILE
echo "\begin{center}" >> $FILE
cat objstat.tex >> $FILE
echo "\\begin{center}\par\includegraphics[width=0.9\\textwidth]{objstat.eps}\\end{center}" >> $FILE
echo "\\begin{center}\par\includegraphics[width=0.9\\textwidth]{slotstat.eps}\\end{center}" >> $FILE
echo "\end{center}" >> $FILE


echo "\section{Memory statistics}" >> $FILE
echo "\begin{center}" >> $FILE

echo "\\begin{center}\par\includegraphics[width=0.9\\textwidth]{memstat.eps}\\end{center}" >> $FILE
cat memstat.tex >> $FILE

echo "\\begin{center}\par\includegraphics[width=0.9\\textwidth]{memhistory.eps}\\end{center}" >> $FILE


echo "\end{center}" >> $FILE



cat tail.tex >> $FILE
latex out.tex
dvipdf out.dvi
