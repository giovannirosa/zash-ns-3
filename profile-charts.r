# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

reqList10 = c()
grantList10 = c()
denList10 = c()
ontoList10 = c()
contList10 = c()
actList10 = c()
valList10 = c()
invalList10 = c()
blockList10 = c()

reqList25 = c()
grantList25 = c()
denList25 = c()
ontoList25 = c()
contList25 = c()
actList25 = c()
valList25 = c()
invalList25 = c()
blockList25 = c()

reqList50 = c()
grantList50 = c()
denList50 = c()
ontoList50 = c()
contList50 = c()
actList50 = c()
valList50 = c()
invalList50 = c()
blockList50 = c()

reqListP = c()
grantListP = c()
denListP = c()
ontoListP = c()
contListP = c()
actListP = c()
valListP = c()
invalListP = c()
blockListP = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  req = grep("REQUESTS NUMBER", readLines(files[i]), value = TRUE)
  reqG = grep("REQUESTS GRANTED", readLines(files[i]), value = TRUE)
  reqD = grep("REQUESTS DENIED", readLines(files[i]), value = TRUE)
  onto = grep("ONTOLOGY FAILS", readLines(files[i]), value = TRUE)
  cont = grep("CONTEXT FAILS", readLines(files[i]), value = TRUE)
  act = grep("ACTIVITY FAILS", readLines(files[i]), value = TRUE)
  valid = grep("VALID PROOFS", readLines(files[i]), value = TRUE)
  inval = grep("INVALID PROOFS", readLines(files[i]), value = TRUE)
  block = grep("BLOCKS", readLines(files[i]), value = TRUE)
  
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
  
  reqVals = as.numeric(unlist(regmatches(req,
                                      gregexpr("[[:digit:]]+\\.*[[:digit:]]*",req))
  )      )
  reqGVals = as.numeric(unlist(regmatches(reqG,
                                     gregexpr("[[:digit:]]+\\.*[[:digit:]]*",reqG))
  )      )
  reqDVals = as.numeric(unlist(regmatches(reqD,
                                      gregexpr("[[:digit:]]+\\.*[[:digit:]]*",reqD))
  )      )
  ontoVals = as.numeric(unlist(regmatches(onto,
                                      gregexpr("[[:digit:]]+\\.*[[:digit:]]*",onto))
  )      )
  contVals = as.numeric(unlist(regmatches(cont,
                                          gregexpr("[[:digit:]]+\\.*[[:digit:]]*",cont))
  )      )
  actVals = as.numeric(unlist(regmatches(act,
                                          gregexpr("[[:digit:]]+\\.*[[:digit:]]*",act))
  )      )
  validVals = as.numeric(unlist(regmatches(valid,
                                          gregexpr("[[:digit:]]+\\.*[[:digit:]]*",valid))
  )      )
  invalVals = as.numeric(unlist(regmatches(inval,
                                          gregexpr("[[:digit:]]+\\.*[[:digit:]]*",inval))
  )      )
  blockVals = as.numeric(unlist(regmatches(block,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",block))
  )      )
  
  print(reqVals)
  print(reqGVals)
  print(reqDVals)
  print(ontoVals[1])
  print(contVals[1])
  print(actVals[1])
  print(validVals[1])
  print(invalVals[1])
  print(blockVals)
  
  if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
    reqList10 <- append(reqList10, reqVals)
    grantList10 <- append(grantList10, reqGVals)
    denList10 <- append(denList10, reqDVals)
    ontoList10 <- append(ontoList10, ontoVals[1])
    contList10 <- append(contList10, contVals[1])
    actList10 <- append(actList10, actVals[1])
    valList10 <- append(valList10, validVals[1])
    invalList10 <- append(invalList10, invalVals[1])
    blockList10 <- append(blockList10, blockVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 10) {
    reqList25 <- append(reqList25, reqVals)
    grantList25 <- append(grantList25, reqGVals)
    denList25 <- append(denList25, reqDVals)
    ontoList25 <- append(ontoList25, ontoVals[1])
    contList25 <- append(contList25, contVals[1])
    actList25 <- append(actList25, actVals[1])
    valList25 <- append(valList25, validVals[1])
    invalList25 <- append(invalList25, invalVals[1])
    blockList25 <- append(blockList25, blockVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 20) {
    reqList50 <- append(reqList50, reqVals)
    grantList50 <- append(grantList50, reqGVals)
    denList50 <- append(denList50, reqDVals)
    ontoList50 <- append(ontoList50, ontoVals[1])
    contList50 <- append(contList50, contVals[1])
    actList50 <- append(actList50, actVals[1])
    valList50 <- append(valList50, validVals[1])
    invalList50 <- append(invalList50, invalVals[1])
    blockList50 <- append(blockList50, blockVals)
  } else if (isHard & length(datarate) > 0 & attacks == 10 & alterations == 5) {
    reqListP <- append(reqListP, reqVals)
    grantListP <- append(grantListP, reqGVals)
    denListP <- append(denListP, reqDVals)
    ontoListP <- append(ontoListP, ontoVals[1])
    contListP <- append(contListP, contVals[1])
    actListP <- append(actListP, actVals[1])
    valListP <- append(valListP, validVals[1])
    invalListP <- append(invalListP, invalVals[1])
    blockListP <- append(blockListP, blockVals)
  }
  # 
  # if (attacks == 50 & alterations == 5 & length(datarate) == 0) {
  #   vals = as.numeric(unlist(regmatches(acrt,
  #                                       gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acrt))
  #   )      )
  #   
  #   if (isHard) {
  #     acrtList <- append(acrtList, vals[1])
  #     acrtpList <- append(acrtpList, vals[2])
  #     acrtnpList <- append(acrtnpList, vals[3])
  #     acrtbList <- append(acrtbList, vals[4])
  #   } else {
  #     acrtListS <- append(acrtListS, vals[1])
  #     acrtpListS <- append(acrtpListS, vals[2])
  #     acrtnpListS <- append(acrtnpListS, vals[3])
  #     acrtbListS <- append(acrtbListS, vals[4])
  #   }
  # }
}

data10 <- data.frame(reqList10, grantList10, denList10, ontoList10, contList10, actList10, valList10, invalList10, blockList10)
data25 <- data.frame(reqList25, grantList25, denList25, ontoList25, contList25, actList25, valList25, invalList25, blockList25)
data50 <- data.frame(reqList50, grantList50, denList50, ontoList50, contList50, actList50, valList50, invalList50, blockList50)
dataP <- data.frame(reqListP, grantListP, denListP, ontoListP, contListP, actListP, valListP, invalListP, blockListP)

d <- data.frame(x = unlist(data50), 
                grp = rep(letters[1:length(data50)],times = sapply(data50,length)), stringsAsFactors = FALSE)


# d[d == 'a'] <- 'ACRT (H)'
# d[d == 'b'] <- 'ACRT (S)'
# d[d == 'c'] <- 'ACRTP (H)'
# d[d == 'd'] <- 'ACRTP (S)'
# d[d == 'e'] <- 'ACRTNP (H)'
# d[d == 'f'] <- 'ACRTNP (S)'
# d[d == 'g'] <- 'ACRTB (H)'
# d[d == 'h'] <- 'ACRTB (S)'

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
        axis.ticks.y = element_line(color="black", size = 0.1))


boxplot(reqList10)$stats[3]

# create dummy data
data <- data.frame(
  name=letters[1:5],
  value=sample(seq(4,15),5),
  sd=c(1,0.2,3,2,4)
)

data <- iris %>% select(Species, Sepal.Length) 

my_sum <- d %>%
  group_by(grp) %>%
  summarise( 
    n=n(),
    mean=mean(x),
    sd=sd(x)
  ) %>%
  mutate( se=sd/sqrt(n))  %>%
  mutate( ic=se * qt((1-0.05)/2 + .5, n-1))


ggplot(my_sum) +
  geom_bar( aes(x=grp, y=mean), stat="identity", fill="forestgreen", alpha=0.5) +
  geom_errorbar( aes(x=grp, ymin=mean-sd, ymax=mean+sd), width=0.4, colour="orange", alpha=0.9, size=1.5) +
  ggtitle("using standard deviation")
