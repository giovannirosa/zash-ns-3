# Libraries
library(ggplot2)
library(dplyr)
library(hrbrthemes)
library(viridis)
library(plotly)
library(fmsb)

ulList <- c(1, 2, 4)

dcList <- c(1, 2, 3)

aList <- c(1, 2, 3)

ulCol <- c()
dcCol <- c()
aCol <- c()
riCol <- c()

for (ul in ulList) {
  for (dc in dcList) {
    for (a in aList) {
      ri <- ul*dc*a
      cat(ul, dc, a, ri, "\n")
      
      ulCol <- append(ulCol, ul)
      dcCol <- append(dcCol, dc)
      aCol <- append(aCol, a)
      riCol <- append(riCol, ri)
    }
  }
}

prData <- data.frame(ulCol, dcCol, aCol)

radarchart(prData)


# plot_ly(x=ulCol, y=dcCol, z=aCol, type="scatter3d", mode="markers", color=riCol)

