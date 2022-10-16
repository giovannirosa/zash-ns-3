# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

dibList10 = c()
dixList10 = c()
sibList10 = c()
sipList10 = c()
aarList10 = c()

dibList25 = c()
dixList25 = c()
sibList25 = c()
sipList25 = c()
aarList25 = c()

dibList50 = c()
dixList50 = c()
sibList50 = c()
sipList50 = c()
aarList50 = c()

dibList10S = c()
dixList10S = c()
sibList10S = c()
sipList10S = c()
aarList10S = c()

dibList25S = c()
dixList25S = c()
sibList25S = c()
sipList25S = c()
aarList25S = c()

dibList50S = c()
dixList50S = c()
sibList50S = c()
sipList50S = c()
aarList50S = c()

dibList10A = c()
dixList10A = c()
sibList10A = c()
sipList10A = c()
aarList10A = c()

dibList20A = c()
dixList20A = c()
sibList20A = c()
sipList20A = c()
aarList20A = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  dib = grep("Denied Impersonations Building", readLines(files[i]), value = TRUE)
  dix = grep("Denied Impersonations Blocked", readLines(files[i]), value = TRUE)
  sib = grep("Successful Impersonations Building", readLines(files[i]), value = TRUE)
  sip = grep("Successful Impersonations Proof", readLines(files[i]), value = TRUE)
  aar = grep("Attacks Avoided Rate", readLines(files[i]), value = TRUE)
  
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
  
  dibVals = as.numeric(unlist(regmatches(dib,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dib))
  )      )
  dixVals = as.numeric(unlist(regmatches(dix,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dix))
  )      )
  sibVals = as.numeric(unlist(regmatches(sib,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",sib))
  )      )
  sipVals = as.numeric(unlist(regmatches(sip,
                                        gregexpr("[[:digit:]]+\\.*[[:digit:]]*",sip))
  )      )
  aarVals = as.numeric(unlist(regmatches(aar,
                                        gregexpr("[[:digit:]]+\\.*[[:digit:]]*",aar))
  )      )
  
  if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
    dibList10 <- append(dibList10, dibVals)
    dixList10 <- append(dixList10, dixVals)
    sibList10 <- append(sibList10, sibVals)
    sipList10 <- append(sipList10, sipVals)
    aarList10 <- append(aarList10, aarVals)
  } else if (isHard & length(datarate) == 0 & attacks == 25 & alterations == 5) {
    dibList25 <- append(dibList25, dibVals)
    dixList25 <- append(dixList25, dixVals)
    sibList25 <- append(sibList25, sibVals)
    sipList25 <- append(sipList25, sipVals)
    aarList25 <- append(aarList25, aarVals)
  } else if (isHard & length(datarate) == 0 & attacks == 50 & alterations == 5) {
    dibList50 <- append(dibList50, dibVals)
    dixList50 <- append(dixList50, dixVals)
    sibList50 <- append(sibList50, sibVals)
    sipList50 <- append(sipList50, sipVals)
    aarList50 <- append(aarList50, aarVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 10) {
    dibList10A <- append(dibList10A, dibVals)
    dixList10A <- append(dixList10A, dixVals)
    sibList10A <- append(sibList10A, sibVals)
    sipList10A <- append(sipList10A, sipVals)
    aarList10A <- append(aarList10A, aarVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 20) {
    dibList20A <- append(dibList20A, dibVals)
    dixList20A <- append(dixList20A, dixVals)
    sibList20A <- append(sibList20A, sibVals)
    sipList20A <- append(sipList20A, sipVals)
    aarList20A <- append(aarList20A, aarVals)
  } else if (!isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
    dibList10S <- append(dibList10S, dibVals)
    dixList10S <- append(dixList10S, dixVals)
    sibList10S <- append(sibList10S, sibVals)
    sipList10S <- append(sipList10S, sipVals)
    aarList10S <- append(aarList10S, aarVals)
  } else if (!isHard & length(datarate) == 0 & attacks == 25 & alterations == 5) {
    dibList25S <- append(dibList25S, dibVals)
    dixList25S <- append(dixList25S, dixVals)
    sibList25S <- append(sibList25S, sibVals)
    sipList25S <- append(sipList25S, sipVals)
    aarList25S <- append(aarList25S, aarVals)
  } else if (!isHard & length(datarate) == 0 & attacks == 50 & alterations == 5) {
    dibList50S <- append(dibList50S, dibVals)
    dixList50S <- append(dixList50S, dixVals)
    sibList50S <- append(sibList50S, sibVals)
    sipList50S <- append(sipList50S, sipVals)
    aarList50S <- append(aarList50S, aarVals)
  }
}

# d <- data.frame(dibList10, dibList10S, dixList10, dixList10S, aarList10, aarList10S)
# d <- data.frame(dibList25, dibList25S, dixList25, dixList25S, aarList25, aarList25S)
d <- data.frame(dibList50, dibList50S, dixList50, dixList50S, aarList50, aarList50S)
# d <- data.frame(dibList10, dibList10A, dibList20A, dixList10, dixList10A, dixList20A, aarList10, aarList10A, aarList20A)

d <- data.frame(x = unlist(d), 
                grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


# d[d == 'a'] <- 'DIB (5)'
# d[d == 'b'] <- 'DIB (10)'
# d[d == 'c'] <- 'DIB (20)'
# d[d == 'd'] <- 'DIX (5)'
# d[d == 'e'] <- 'DIX (10)'
# d[d == 'f'] <- 'DIX (20)'
# d[d == 'g'] <- 'AAR (5)'
# d[d == 'h'] <- 'AAR (10)'
# d[d == 'i'] <- 'AAR (20)'

d[d == 'a'] <- 'DIB (H)'
d[d == 'b'] <- 'DIB (S)'
d[d == 'c'] <- 'DIX (H)'
d[d == 'd'] <- 'DIX (S)'
d[d == 'e'] <- 'AAR (H)'
d[d == 'f'] <- 'AAR (S)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")

# df_mean <- d %>% 
#   group_by(grp) %>% 
#   summarize(average = mean(x)) %>%
#   ungroup()
# 
# legend_ord <- levels(with(d, reorder(grp, x)))

my_sum <- d %>%
  group_by(grp) %>%
  summarise( 
    n=n(),
    mean=mean(x),
    sd=sd(x)
  ) %>%
  mutate( se=sd/sqrt(n))  %>%
  mutate( ic=se * qt((1-0.05)/2 + .5, n-1))

#factor(grp, levels = c('AAR (5)','AAR (10)','AAR (20)','DIB (5)','DIB (10)','DIB (20)','DIX (5)','DIX (10)','DIX (20)'))

ggplot(my_sum) +
  geom_bar( aes(x=grp, y=mean, fill=grp), stat="identity", alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  theme_ipsum_rc() +
  labs(x="",
       y="Percentage (%)",
       title = "Attacks Avoided Rate (AAR) - 50 Attacks",
       fill = "Scenarios") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  scale_y_continuous(expand = c(0, 0), limits = c(0,105))
