
graphColor <- "orange"


postscript("memhistory.eps",width=7, height=5, horizontal=F)

table <- read.table("memhistory.txt")
plot(table,type="l")

dev.off(2)
