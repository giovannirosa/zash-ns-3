# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

acrtList = c()
acrtpList = c()
acrtnpList = c()
acrtbList = c()
acrtListS = c()
acrtpListS = c()
acrtnpListS = c()
acrtbListS = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  acrt = grep("(ACRT)", readLines(files[i]), value = TRUE)
  
  configFile = grep("Enums config file is", readLines(scenarioFile), value = TRUE)
  genAttacks = grep("Generated attacks are", readLines(scenarioFile), value = TRUE)
  genAlter = grep("Generated alterations are", readLines(scenarioFile), value = TRUE)
  datarate = grep("Datarate", readLines(scenarioFile), value = TRUE)
  
  isHard = grepl("hard", configFile, fixed = TRUE)
  attacks = as.numeric(unlist(regmatches(genAttacks,
                                      gregexpr("[[:digit:]]+\\.*[[:digit:]]*",genAttacks))
  )      )
  alterations = as.numeric(unlist(regmatches(genAlter,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",genAlter))
  )      )
  
  if (attacks == 25 & alterations == 5 & length(datarate) == 0) {
    vals = as.numeric(unlist(regmatches(acrt,
                                        gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acrt))
    )      )
    
    if (isHard) {
      acrtList <- append(acrtList, vals[1])
      acrtpList <- append(acrtpList, vals[2])
      acrtnpList <- append(acrtnpList, vals[3])
      acrtbList <- append(acrtbList, vals[4])
    } else {
      acrtListS <- append(acrtListS, vals[1])
      acrtpListS <- append(acrtpListS, vals[2])
      acrtnpListS <- append(acrtnpListS, vals[3])
      acrtbListS <- append(acrtbListS, vals[4])
    }
  }
}

acrtData <- data.frame(acrtList, acrtListS, acrtpList, acrtpListS, acrtnpList, acrtnpListS, acrtbList, acrtbListS)

d <- data.frame(x = unlist(acrtData), 
                grp = rep(letters[1:length(acrtData)],times = sapply(acrtData,length)), stringsAsFactors = FALSE)


d[d == 'a'] <- 'ACRT (H)'
d[d == 'b'] <- 'ACRT (S)'
d[d == 'c'] <- 'ACRTP (H)'
d[d == 'd'] <- 'ACRTP (S)'
d[d == 'e'] <- 'ACRTNP (H)'
d[d == 'f'] <- 'ACRTNP (S)'
d[d == 'g'] <- 'ACRTB (H)'
d[d == 'h'] <- 'ACRTB (S)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")


ggplot(d, aes(x=grp, y=x, fill=grp)) + 
  geom_boxplot(alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  geom_jitter(color="black", size=0.4, alpha=0.9) +
  theme_ipsum_rc() +
  labs(x="",
       y="Milliseconds",
       title = "ACRT - 25 Attacks") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  scale_y_continuous(limits = c(0, 8.5))
