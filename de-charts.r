# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)

hrbrthemes::import_roboto_condensed()


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

grep("a", readLines(files[0]), value = TRUE)

print(files)

acrtList = c()
acrtpList = c()
acrtnpList = c()

for (i in 1:length(files)) {
  print(files[i])
  
  acrt = grep("(ACRT)", readLines(files[i]), value = TRUE)
  
  vals = as.numeric(unlist(regmatches(acrt,
                               gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acrt))
  )      )
  
  acrtList <- append(acrtList, vals[1])
  acrtpList <- append(acrtpList, vals[2])
  acrtnpList <- append(acrtnpList, vals[3])
  
  
  
}

acrtData <- data.frame(acrtList, acrtpList, acrtnpList)

t_acrtData <- transpose(acrtData)

d <- data.frame(x = unlist(acrtData), 
                grp = rep(letters[1:length(acrtData)],times = sapply(acrtData,length)))

boxplot(acrtList,acrtpList,acrtnpList)


ggplot(d, aes(x=grp, y=x, fill=grp)) + 
  geom_boxplot(alpha=0.1) +
  scale_fill_viridis(discrete=TRUE, option="A") +
  geom_jitter(color="black", size=0.4, alpha=0.9) +
  theme_ipsum_rc() +
  labs(x="",
       y="Milliseconds",
       title = "ACRT") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1))
