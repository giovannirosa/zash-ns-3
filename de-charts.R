# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

deList5 = c()

deList10 = c()

deList20 = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  de = grep("Device Extensibility", readLines(files[i]), value = TRUE)
  print(de)
  
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
  
  deVals = as.numeric(unlist(regmatches(de,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",de))
  )      )
  
  if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
    deList5 <- append(deList5, deVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 10) {
    deList10 <- append(deList10, deVals)
  } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 20) {
    deList20 <- append(deList20, deVals)
  }
}

d <- data.frame(deList5, deList10, deList20)

d <- data.frame(x = unlist(d), 
                grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


d[d == 'a'] <- 'DE (5)'
d[d == 'b'] <- 'DE (10)'
d[d == 'c'] <- 'DE (20)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")

df_mean <- d %>% 
  group_by(grp) %>% 
  summarize(average = mean(x)) %>%
  ungroup()

legend_ord <- levels(with(d, reorder(grp, x)))


ggplot(d, aes(x=reorder(grp, x), y=x, fill=grp)) + 
  geom_boxplot(alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  geom_jitter(color="black", size=0.4, alpha=0.9) +
  theme_ipsum_rc() +
  labs(x="",
       y="Difference of number of devices",
       title = "Device Extensibility (DE)",
       fill = "Scenarios") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  geom_point(data = df_mean, 
             mapping = aes(x = grp, y = average),
             color="red") +
  geom_line(data = df_mean, 
            mapping = aes(x = grp, y = average, group=1)) + scale_fill_discrete(breaks=legend_ord)
