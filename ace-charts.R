# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

racList = c()
hacList = c()
pacList = c()
niList = c()
itList = c()
aceList = c()

for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  rac = grep("Number of requests of requested access way", readLines(files[i]), value = TRUE)
  hac = grep("Number of requests of home access way", readLines(files[i]), value = TRUE)
  pac = grep("Number of requests of personal access way", readLines(files[i]), value = TRUE)
  ni = grep("Number of requests without intermediaries", readLines(files[i]), value = TRUE)
  it = grep("Number of requests with intermediaries", readLines(files[i]), value = TRUE)
  ace = grep("Access Control Enforcement", readLines(files[i]), value = TRUE)
  
  configFile = grep("Enums config file is", readLines(scenarioFile), value = TRUE)
  genAttacks = grep("Generated attacks are", readLines(scenarioFile), value = TRUE)
  genAlter = grep("Generated alterations are", readLines(scenarioFile), value = TRUE)
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
  
  racVals = as.numeric(unlist(regmatches(rac,
                                        gregexpr("[[:digit:]]+\\.*[[:digit:]]*",rac))
  )      )
  hacVals = as.numeric(unlist(regmatches(hac,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",hac))
  )      )
  pacVals = as.numeric(unlist(regmatches(pac,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",pac))
  )      )
  niVals = as.numeric(unlist(regmatches(ni,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",ni))
  )      )
  itVals = as.numeric(unlist(regmatches(it,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",it))
  )      )
  aceVals = as.numeric(unlist(regmatches(ace,
                                         gregexpr("[[:digit:]]+\\.*[[:digit:]]*",ace))
  )      )
  
  if (isHard & datarate == 100 & attacks == 10 & alterations == 5) {
    racList <- append(racList, racVals)
    hacList <- append(hacList, hacVals)
    pacList <- append(pacList, pacVals)
    niList <- append(niList, niVals)
    itList <- append(itList, itVals)
    aceList <- append(aceList, aceVals)}
}

d <- data.frame(racList, hacList, pacList, niList, itList)

d <- data.frame(x = unlist(d), 
                grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


d[d == 'a'] <- '|RAW|'
d[d == 'b'] <- '|HAW|'
d[d == 'c'] <- '|PAW|'
d[d == 'd'] <- '|NI|'
d[d == 'e'] <- '|I|'

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


ggplot(my_sum) +
  geom_bar( aes(x=factor(grp, levels = c('|PAW|','|HAW|','|RAW|','|NI|','|I|')), y=mean, fill=grp), colour="black", stat="identity", alpha=0.5) +
  geom_errorbar( aes(x=grp, ymin=mean-sd, ymax=mean+sd), width=0.4, colour="orange", alpha=0.9, size=1.3) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  theme_ipsum_rc() +
  labs(x="",
       y="Number of requests",
       title = "Access Control Enforcement (ACE)",
       fill = "Scenarios") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5)),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1))
