# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

acrtList5 = c()

acrtList10 = c()

acrtList20 = c()

acrtList40 = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  acrt = grep("(ACRT)", readLines(files[i]), value = TRUE)
  
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
  
  vals = as.numeric(unlist(regmatches(acrt,
                                      gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acrt))
  )      )
  
  if (isHard & datarate == 100 & attacks == 10 & alterations == 5 & users == 5) {
    acrtList5 <- append(acrtList5, vals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 10 & users == 5) {
    acrtList10 <- append(acrtList10, vals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 20 & users == 5) {
    acrtList20 <- append(acrtList20, vals)
  } else if (isHard & datarate == 100 & attacks == 10 & alterations == 40 & users == 5) {
    acrtList40 <- append(acrtList40, vals)
  }
}

acrtData <- data.frame(acrtList5, acrtList10, acrtList20, acrtList40)

d <- data.frame(x = unlist(acrtData), 
                grp = rep(letters[1:length(acrtData)],times = sapply(acrtData,length)), stringsAsFactors = FALSE)


d[d == 'a'] <- 'ACRT (5)'
d[d == 'b'] <- 'ACRT (10)'
d[d == 'c'] <- 'ACRT (20)'
d[d == 'd'] <- 'ACRT (40)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")

df_mean <- d %>% 
  group_by(grp) %>% 
  summarize(average = mean(x)) %>%
  ungroup()


d$grp = factor(d$grp, levels = c('ACRT (5)','ACRT (10)','ACRT (20)','ACRT (40)'))


ggplot(d, aes(x=grp, y=x, fill=grp)) + 
  geom_boxplot(alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  geom_jitter(color="black", size=0.4, alpha=0.9) +
  theme_ipsum_rc() +
  labs(x="",
       y="Milliseconds",
       title = "Access Control Response Time (ACRT) - Alterations Impact",
       fill = "Scenarios") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  scale_y_continuous(limits = c(0, 8.5)) +
  geom_point(data = df_mean, 
             mapping = aes(x = grp, y = average),
             color="red") +
  geom_line(data = df_mean, 
            mapping = aes(x = grp, y = average, group=1))
