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
adrList10 = c()

dibList25 = c()
dixList25 = c()
sibList25 = c()
sipList25 = c()
adrList25 = c()

dibList50 = c()
dixList50 = c()
sibList50 = c()
sipList50 = c()
adrList50 = c()

dibList10S = c()
dixList10S = c()
sibList10S = c()
sipList10S = c()
adrList10S = c()

dibList25S = c()
dixList25S = c()
sibList25S = c()
sipList25S = c()
adrList25S = c()

dibList50S = c()
dixList50S = c()
sibList50S = c()
sipList50S = c()
adrList50S = c()

dibList10A = c()
dixList10A = c()
sibList10A = c()
sipList10A = c()
adrList10A = c()

dibList20A = c()
dixList20A = c()
sibList20A = c()
sipList20A = c()
adrList20A = c()

dibList40A = c()
dixList40A = c()
sibList40A = c()
sipList40A = c()
adrList40A = c()

dibListP = c()
dixListP = c()
sibListP = c()
sipListP = c()
adrListP = c()

dibListU = c()
dixListU = c()
sibListU = c()
sipListU = c()
adrListU = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  dib = grep("Denied Impersonations Building", readLines(files[i]), value = TRUE)
  dix = grep("Denied Impersonations Blocked", readLines(files[i]), value = TRUE)
  sib = grep("Successful Impersonations Building", readLines(files[i]), value = TRUE)
  sip = grep("Successful Impersonations Proof", readLines(files[i]), value = TRUE)
  adr = grep("Attacks Avoided Rate", readLines(files[i]), value = TRUE)
  
  configFile = grep("Enums config file is", readLines(scenarioFile), value = TRUE)
  genAttacks = grep("Generated attacks are", readLines(scenarioFile), value = TRUE)
  genAlter = grep("Generated alterations are", readLines(scenarioFile), value = TRUE)
  users = grep("Users of simulation are", readLines(scenarioFile), value = TRUE)
  users = as.numeric(unlist(regmatches(users,
                                       gregexpr("[[:digit:]]+\\.*[[:digit:]]*",users))
  )      )
  datarate = grep("Datarate", readLines(scenarioFile), value = TRUE)
  datarate = as.numeric(unlist(regmatches(datarate,
                                              gregexpr("[[:digit:]]+\\.*[[:digit:]]*",datarate))
  )      )
  
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
  adrVals = as.numeric(unlist(regmatches(adr,
                                        gregexpr("[[:digit:]]+\\.*[[:digit:]]*",adr))
  )      )
  
  if (isHard & datarate == 100 & attacks == 10 & alterations == 5 & users == 5) {
    dibList10 <- append(dibList10, dibVals)
    dixList10 <- append(dixList10, dixVals)
    sibList10 <- append(sibList10, sibVals)
    sipList10 <- append(sipList10, sipVals)
    adrList10 <- append(adrList10, adrVals)
  } else if (isHard & datarate == 100 & attacks == 25 & alterations == 5 & users == 5) {
    dibList25 <- append(dibList25, dibVals)
    dixList25 <- append(dixList25, dixVals)
    sibList25 <- append(sibList25, sibVals)
    sipList25 <- append(sipList25, sipVals)
    adrList25 <- append(adrList25, adrVals)
  } else if (isHard & datarate == 100 & attacks == 50 & alterations == 5 & users == 5) {
    dibList50 <- append(dibList50, dibVals)
    dixList50 <- append(dixList50, dixVals)
    sibList50 <- append(sibList50, sibVals)
    sipList50 <- append(sipList50, sipVals)
    adrList50 <- append(adrList50, adrVals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 10 & users == 5) {
    dibList10A <- append(dibList10A, dibVals)
    dixList10A <- append(dixList10A, dixVals)
    sibList10A <- append(sibList10A, sibVals)
    sipList10A <- append(sipList10A, sipVals)
    adrList10A <- append(adrList10A, adrVals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 20 & users == 5) {
    dibList20A <- append(dibList20A, dibVals)
    dixList20A <- append(dixList20A, dixVals)
    sibList20A <- append(sibList20A, sibVals)
    sipList20A <- append(sipList20A, sipVals)
    adrList20A <- append(adrList20A, adrVals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 40 & users == 5) {
    dibList40A <- append(dibList40A, dibVals)
    dixList40A <- append(dixList40A, dixVals)
    sibList40A <- append(sibList40A, sibVals)
    sipList40A <- append(sipList40A, sipVals)
    adrList40A <- append(adrList40A, adrVals)
  } else if (!isHard & datarate == 100 & attacks == 10 & alterations == 5 & users == 5) {
    dibList10S <- append(dibList10S, dibVals)
    dixList10S <- append(dixList10S, dixVals)
    sibList10S <- append(sibList10S, sibVals)
    sipList10S <- append(sipList10S, sipVals)
    adrList10S <- append(adrList10S, adrVals)
  } else if (!isHard & datarate == 100 & attacks == 25 & alterations == 5 & users == 5) {
    dibList25S <- append(dibList25S, dibVals)
    dixList25S <- append(dixList25S, dixVals)
    sibList25S <- append(sibList25S, sibVals)
    sipList25S <- append(sipList25S, sipVals)
    adrList25S <- append(adrList25S, adrVals)
  } else if (!isHard & datarate == 100 & attacks == 50 & alterations == 5 & users == 5) {
    dibList50S <- append(dibList50S, dibVals)
    dixList50S <- append(dixList50S, dixVals)
    sibList50S <- append(sibList50S, sibVals)
    sipList50S <- append(sipList50S, sipVals)
    adrList50S <- append(adrList50S, adrVals)
  } else if (isHard & datarate == 10 & attacks == 10 & alterations == 5 & users == 5) {
    dibListP <- append(dibListP, dibVals)
    dixListP <- append(dixListP, dixVals)
    sibListP <- append(sibListP, sibVals)
    sipListP <- append(sipListP, sipVals)
    adrListP <- append(adrListP, adrVals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 5 & users == 2) {
    dibListU <- append(dibListU, dibVals)
    dixListU <- append(dixListU, dixVals)
    sibListU <- append(sibListU, sibVals)
    sipListU <- append(sipListU, sipVals)
    adrListU <- append(adrListU, adrVals)
  }
}

mode = 'USR'

if (mode == 10) {
  title = "Attacks Denied Rate (ADR) - 10 Attacks"
  d <- data.frame(dibList10, dibList10S, dixList10, dixList10S, sibList10, sibList10S, sipList10, sipList10S, adrList10, adrList10S)
} else if (mode == 25) {
  title = "Attacks Denied Rate (ADR) - 25 Attacks"
  d <- data.frame(dibList25, dibList25S, dixList25, dixList25S, sibList25, sibList25S, sipList25, sipList25S, adrList25, adrList25S)
} else if (mode == 50) {
  title = "Attacks Denied Rate (ADR) - 50 Attacks"
  d <- data.frame(dibList50, dibList50S, dixList50, dixList50S, sibList50, sibList50S, sipList50, sipList50S, adrList50, adrList50S)
} else if (mode == 'ALT') {
  title = "Attacks Denied Rate (ADR) - Alterations Impact"
  d <- data.frame(dibList10, dibList10A, dibList20A, dibList40A, dixList10, dixList10A, dixList20A, dixList40A, sibList10, sibList10A, sibList20A, sibList40A, sipList10, sipList10A, sipList20A, sipList40A, adrList10, adrList10A, adrList20A, adrList40A)
} else if (mode == 'USR') {
  title = "Attacks Denied Rate (ADR) - Users Impact"
  d <- data.frame(dibList10, dibListU, dixList10, dixListU, sibList10, sibListU, sipList10, sipListU, adrList10, adrListU)
}

d <- data.frame(x = unlist(d), 
                grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


if (mode == 'ALT') {
  d[d == 'a'] <- 'DIB (5)'
  d[d == 'b'] <- 'DIB (10)'
  d[d == 'c'] <- 'DIB (20)'
  d[d == 'd'] <- 'DIB (40)'
  d[d == 'e'] <- 'DIX (5)'
  d[d == 'f'] <- 'DIX (10)'
  d[d == 'g'] <- 'DIX (20)'
  d[d == 'h'] <- 'DIX (40)'
  d[d == 'i'] <- 'SIB (5)'
  d[d == 'j'] <- 'SIB (10)'
  d[d == 'k'] <- 'SIB (20)'
  d[d == 'l'] <- 'SIB (40)'
  d[d == 'm'] <- 'SIP (5)'
  d[d == 'n'] <- 'SIP (10)'
  d[d == 'o'] <- 'SIP (20)'
  d[d == 'p'] <- 'SIP (40)'
  d[d == 'q'] <- 'ADR (5)'
  d[d == 'r'] <- 'ADR (10)'
  d[d == 's'] <- 'ADR (20)'
  d[d == 't'] <- 'ADR (40)'
} else if (mode == 'USR') {
  d[d == 'a'] <- 'DIB (5)'
  d[d == 'b'] <- 'DIB (2)'
  d[d == 'c'] <- 'DIX (5)'
  d[d == 'd'] <- 'DIX (2)'
  d[d == 'e'] <- 'SIB (5)'
  d[d == 'f'] <- 'SIB (2)'
  d[d == 'g'] <- 'SIP (5)'
  d[d == 'h'] <- 'SIP (2)'
  d[d == 'i'] <- 'ADR (5)'
  d[d == 'j'] <- 'ADR (2)'
} else {
  d[d == 'a'] <- 'DIB (H)'
  d[d == 'b'] <- 'DIB (S)'
  d[d == 'c'] <- 'DIX (H)'
  d[d == 'd'] <- 'DIX (S)'
  d[d == 'e'] <- 'SIB (H)'
  d[d == 'f'] <- 'SIB (S)'
  d[d == 'g'] <- 'SIP (H)'
  d[d == 'h'] <- 'SIP (S)'
  d[d == 'i'] <- 'ADR (H)'
  d[d == 'j'] <- 'ADR (S)'
}



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

# my_sum[[6]][[9]] = 3.333333
# $ic = my_sum$mean


# my_sum$ic <- replace(my_sum$ic, my_sum$ic>my_sum$mean, my_sum$mean)

if (mode == 'ALT') {
  my_sum$grp = factor(my_sum$grp, levels = c('ADR (5)','ADR (10)','ADR (20)','ADR (40)','DIB (5)','DIB (10)','DIB (20)','DIB (40)','DIX (5)','DIX (10)','DIX (20)','DIX (40)','SIB (5)','SIB (10)','SIB (20)','SIB (40)','SIP (5)','SIP (10)','SIP (20)','SIP (40)'))
}


ggplot(my_sum) +
  geom_bar( aes(x=grp, y=mean, fill=grp), colour="black", stat="identity", alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  theme_ipsum_rc() +
  labs(x="",
       y="Percentage (%)",
       title = title,
       fill = "Scenarios") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5), angle = 90),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  scale_y_continuous(expand = c(0, 0), limits = c(0,105)) +
  geom_errorbar( aes(x=grp, ymin=ifelse(mean-ic < 0, 0, mean-ic), ymax=ifelse(mean+ic > 100, 100, mean+ic)), width=0.4, colour="orange", alpha=0.9, size=1.3) +
  guides(fill=guide_legend(ncol=2))
