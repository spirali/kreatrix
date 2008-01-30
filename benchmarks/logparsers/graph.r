
graphColor <- "orange"


##### OBJSTAT ####

postscript("objstat.eps",width=7, height=5, horizontal=F)

table <- read.table("objstat.txt")

data <- as.vector(table[,2])
textes <- as.vector(table[,1])

title <- commandArgs()[5]

names <- 1:length(data)

barplot(data,
		col=graphColor,
		main=paste("Objects extensions in ",title),
		xlab="ObjectExtension",
		ylab="Count of objects",
		names=names
)

legend("right", textes)

dev.off(2)

##### SLOTSTAT ####

postscript("slotstat.eps",width=7, height=5, horizontal=F)

table <- read.table("slotstat.txt")

data <- as.vector(table[,2])
names <- as.vector(table[,1])

title <- commandArgs()[5]

barplot(data,
		col=graphColor,
		main=paste("Count of slots when object is freed",title),
		xlab="Count of slots",
		ylab="Count of objects",
		names=names
)


dev.off(2)


##### MEMSTAT ####

postscript("memstat.eps",width=7, height=5, horizontal=F)


table <- read.table("memstat.txt")

data <- as.vector(table[,2])
names <- as.vector(table[,1])

title <- commandArgs()[5]


barplot(data,
		col=graphColor,
		main=paste("Objects extensions in ",title),
		xlab="Sizes of allocated memory",
		ylab="Count of allocations",
		names=names
);

dev.off(2)
