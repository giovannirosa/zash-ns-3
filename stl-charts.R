# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

stlList10 = c()

stlList25 = c()

stlList50 = c()

stlListP = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  stl = grep("(STL)", readLines(files[i]), value = TRUE)
  
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
  
  stlVals = as.numeric(unlist(regmatches(stl,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",stl))
  )      )
  
  if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
    stlList10 <- append(stlList10, stlVals)
  } else if (isHard & length(datarate) == 0 & attacks == 25 & alterations == 5) {
    stlList25 <- append(stlList25, stlVals)
  } else if (isHard & length(datarate) == 0 & attacks == 50 & alterations == 5) {
    stlList50 <- append(stlList50, stlVals)
  } else if (isHard & length(datarate) > 0 & attacks == 10 & alterations == 5) {
    stlListP <- append(stlListP, stlVals)
  }
}

d <- data.frame(stlList10, stlList25, stlList50)

d <- data.frame(x = unlist(d), 
                grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


d[d == 'a'] <- 'STL (10)'
d[d == 'b'] <- 'STL (25)'
d[d == 'c'] <- 'STL (50)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")

df_mean <- d %>% 
  group_by(grp) %>% 
  summarize(average = mean(x)) %>%
  ungroup()


ggplot(d, aes(x=grp, y=x, fill=grp)) + 
  geom_boxplot(alpha=0.5) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  geom_jitter(color="black", size=0.4, alpha=0.9) +
  theme_ipsum_rc() +
  labs(x="",
       y="Milliseconds",
       title = "Spatial and Temporal Locality (STL)",
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
            mapping = aes(x = grp, y = average, group=1))
