# Load ggplot2
library(ggplot2)
library(data.table)
library(hrbrthemes)
library(viridis)
library(dplyr)
library(stringr)


files <- list.files(path="/home/grosa/Dev/ns-allinone-3.36.1/ns-3.36.1/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

# files <- list.files(path="/home/grosa/Dev/zash-ns-3/zash_traces", pattern="zash_simulation_metrics_*", full.names=TRUE, recursive=TRUE)

#print(files)

uslList = list()
uslListNames = list()
uslListCat = list()

actList = list()
actListNames = list()
actListCat = list()

dcsList = list()
dcsListNames = list()
dcsListCat = list()

acwList = list()
acwListNames = list()
acwListCat = list()

locList = list()
locListNames = list()
locListCat = list()

timList = list()
timListNames = list()
timListCat = list()

ageList = list()
ageListNames = list()
ageListCat = list()

grpList = list()
grpListNames = list()
grpListCat = list()

# index = 1
for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
  usl = grep("User Level", readLines(files[i]), value = TRUE)
  act = grep("Action", readLines(files[i]), value = TRUE)
  dcs = grep("Device Class", readLines(files[i]), value = TRUE)
  tim = grep("Time:", readLines(files[i]), value = TRUE)
  loc = grep("Localization", readLines(files[i]), value = TRUE)
  age = grep("Age", readLines(files[i]), value = TRUE)
  grp = grep("Group", readLines(files[i]), value = TRUE)
  acw = grep("Access Way", readLines(files[i]), value = TRUE)
  dvc = grep("Device:", readLines(files[i]), value = TRUE)
  usr = grep("User:", readLines(files[i]), value = TRUE)
  
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
  
  if (isHard & datarate == 100 & attacks == 10 & alterations == 5) {
    
    uslVals = as.numeric(unlist(regmatches(usl,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",usl))
    )      )
    actVals = as.numeric(unlist(regmatches(act,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",act))
    )      )
    dcsVals = as.numeric(unlist(regmatches(dcs,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dcs))
    )      )
    timVals = as.numeric(unlist(regmatches(tim,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",tim))
    )      )
    locVals = as.numeric(unlist(regmatches(loc,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",loc))
    )      )
    ageVals = as.numeric(unlist(regmatches(age,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",age))
    )      )
    grpVals = as.numeric(unlist(regmatches(grp,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",grp))
    )      )
    acwVals = as.numeric(unlist(regmatches(acw,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acw))
    )      )
    dvcVals = as.numeric(unlist(regmatches(dvc,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dvc))
    )      )
    usrVals = as.numeric(unlist(regmatches(usr,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",usr))
    )      )
    
    # print("-------------------------------")
    # for (v in uslVals) {
    #   print(v)
    # }
    
    
    
    names <- str_match(usl, ":\\s*(.*?)\\s*=")[,2]
    
    # tempList = c()
    index = 1
    for (x in 1:length(uslVals)) {
      print(uslVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(uslList)) {
          uslList[[index]] = list()
          uslListNames[[index]] = list()
          uslListCat[[index]] = list()
        }
        uslList[[index]] <- append(uslList[[index]], uslVals[[x]])
        uslListNames[[index]] <- append(uslListNames[[index]], names[[index]])
        uslListCat[[index]] <- append(uslListCat[[index]], "User Level")
        # tempList = append(tempList, uslVals[[x]])
        index = index + 1
      }
      if (index > 4) {
        break;
      }
    }
    
    names <- str_match(act, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(actVals)) {
      print(actVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(actList)) {
          actList[[index]] = list()
          actListNames[[index]] = list()
          actListCat[[index]] = list()
        }
        actList[[index]] <- append(actList[[index]], actVals[[x]])
        actListNames[[index]] <- append(actListNames[[index]], names[[index]])
        actListCat[[index]] <- append(actListCat[[index]], "Action")
        index = index + 1
      }
      if (index > 3) {
        break;
      }
    }
    
    names <- str_match(dcs, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(dcsVals)) {
      print(dcsVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(dcsList)) {
          dcsList[[index]] = list()
          dcsListNames[[index]] = list()
          dcsListCat[[index]] = list()
        }
        dcsList[[index]] <- append(dcsList[[index]], dcsVals[[x]])
        dcsListNames[[index]] <- append(dcsListNames[[index]], names[[index]])
        dcsListCat[[index]] <- append(dcsListCat[[index]], "Device Class")
        index = index + 1
      }
      if (index > 2) {
        break;
      }
    }
    
    names <- str_match(acw, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(acwVals)) {
      print(acwVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(acwList)) {
          acwList[[index]] = list()
          acwListNames[[index]] = list()
          acwListCat[[index]] = list()
        }
        acwList[[index]] <- append(acwList[[index]], acwVals[[x]])
        acwListNames[[index]] <- append(acwListNames[[index]], names[[index]])
        acwListCat[[index]] <- append(acwListCat[[index]], "Access Way")
        index = index + 1
      }
      if (index > 3) {
        break;
      }
    }
    
    names <- str_match(loc, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(locVals)) {
      print(locVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(locList)) {
          locList[[index]] = list()
          locListNames[[index]] = list()
          locListCat[[index]] = list()
        }
        locList[[index]] <- append(locList[[index]], locVals[[x]])
        locListNames[[index]] <- append(locListNames[[index]], names[[index]])
        locListCat[[index]] <- append(locListCat[[index]], "Localization")
        index = index + 1
      }
      if (index > 2) {
        break;
      }
    }
    
    names <- str_match(tim, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(timVals)) {
      print(timVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(timList)) {
          timList[[index]] = list()
          timListNames[[index]] = list()
          timListCat[[index]] = list()
        }
        timList[[index]] <- append(timList[[index]], timVals[[x]])
        timListNames[[index]] <- append(timListNames[[index]], names[[index]])
        timListCat[[index]] <- append(timListCat[[index]], "Time Class")
        index = index + 1
      }
      if (index > 3) {
        break;
      }
    }
    
    names <- str_match(age, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(ageVals)) {
      print(ageVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(ageList)) {
          ageList[[index]] = list()
          ageListNames[[index]] = list()
          ageListCat[[index]] = list()
        }
        ageList[[index]] <- append(ageList[[index]], ageVals[[x]])
        if (names[[index]] == 'ADULT') {
          ageListNames[[index]] <- append(ageListNames[[index]], 'ADULT AGE')
        } else {
          ageListNames[[index]] <- append(ageListNames[[index]], names[[index]])
        }
        ageListCat[[index]] <- append(ageListCat[[index]], "Age")
        index = index + 1
      }
      if (index > 3) {
        break;
      }
    }
    
    names <- str_match(grp, ":\\s*(.*?)\\s*=")[,2]
    
    index = 1
    for (x in 1:length(grpVals)) {
      print(grpVals[[x]])
      col = c()
      if (x %% 2 == 0) {
        if (index > length(grpList)) {
          grpList[[index]] = list()
          grpListNames[[index]] = list()
          grpListCat[[index]] = list()
        }
        grpList[[index]] <- append(grpList[[index]], grpVals[[x]])
        grpListNames[[index]] <- append(grpListNames[[index]], names[[index]])
        grpListCat[[index]] <- append(grpListCat[[index]], "Group")
        index = index + 1
      }
      if (index > 2) {
        break;
      }
    }
  }
  
  
  # uslList[[index]] = tempList
  # index = index + 1
  
  
  
  # if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
  #   dibList10 <- append(dibList10, dibVals)
  # } else if (isHard & length(datarate) == 0 & attacks == 25 & alterations == 5) {
  #   dibList25 <- append(dibList25, dibVals)
  #   dixList25 <- append(dixList25, dixVals)
  #   sibList25 <- append(sibList25, sibVals)
  #   sipList25 <- append(sipList25, sipVals)
  #   aarList25 <- append(aarList25, aarVals)
  # } else if (isHard & length(datarate) == 0 & attacks == 50 & alterations == 5) {
  #   dibList50 <- append(dibList50, dibVals)
  #   dixList50 <- append(dixList50, dixVals)
  #   sibList50 <- append(sibList50, sibVals)
  #   sipList50 <- append(sipList50, sipVals)
  #   aarList50 <- append(aarList50, aarVals)
  # } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 10) {
  #   dibList10A <- append(dibList10A, dibVals)
  #   dixList10A <- append(dixList10A, dixVals)
  #   sibList10A <- append(sibList10A, sibVals)
  #   sipList10A <- append(sipList10A, sipVals)
  #   aarList10A <- append(aarList10A, aarVals)
  # } else if (isHard & length(datarate) == 0 & attacks == 10 & alterations == 20) {
  #   dibList20A <- append(dibList20A, dibVals)
  #   dixList20A <- append(dixList20A, dixVals)
  #   sibList20A <- append(sibList20A, sibVals)
  #   sipList20A <- append(sipList20A, sipVals)
  #   aarList20A <- append(aarList20A, aarVals)
  # } else if (!isHard & length(datarate) == 0 & attacks == 10 & alterations == 5) {
  #   dibList10S <- append(dibList10S, dibVals)
  #   dixList10S <- append(dixList10S, dixVals)
  #   sibList10S <- append(sibList10S, sibVals)
  #   sipList10S <- append(sipList10S, sipVals)
  #   aarList10S <- append(aarList10S, aarVals)
  # } else if (!isHard & length(datarate) == 0 & attacks == 25 & alterations == 5) {
  #   dibList25S <- append(dibList25S, dibVals)
  #   dixList25S <- append(dixList25S, dixVals)
  #   sibList25S <- append(sibList25S, sibVals)
  #   sipList25S <- append(sipList25S, sipVals)
  #   aarList25S <- append(aarList25S, aarVals)
  # } else if (!isHard & length(datarate) == 0 & attacks == 50 & alterations == 5) {
  #   dibList50S <- append(dibList50S, dibVals)
  #   dixList50S <- append(dixList50S, dixVals)
  #   sibList50S <- append(sibList50S, sibVals)
  #   sipList50S <- append(sipList50S, sipVals)
  #   aarList50S <- append(aarList50S, aarVals)
  # }
}

# d <- data.frame(dibList10, dibList10S, dixList10, dixList10S, aarList10, aarList10S)
# d <- data.frame(dibList25, dibList25S, dixList25, dixList25S, aarList25, aarList25S)
d <- data.frame(uslList, actList, dcsList, timList, locList, ageList, grpList, acwList)
dN <- data.frame(uslListNames, actListNames, dcsListNames, timListNames, locListNames, ageListNames, grpListNames, acwListNames)
dC <- data.frame(uslListCat, actListCat, dcsListCat, timListCat, locListCat, ageListCat, grpListCat, acwListCat)
# d <- data.frame(dibList10, dibList10A, dibList20A, dixList10, dixList10A, dixList20A, aarList10, aarList10A, aarList20A)

# d <- data.frame(x = unlist(d), 
#                 grp = rep(letters[1:length(d)],times = sapply(d,length)), stringsAsFactors = FALSE)


# d[d == 'a'] <- 'DIB (5)'
# d[d == 'b'] <- 'DIB (10)'
# d[d == 'c'] <- 'DIB (20)'
# d[d == 'd'] <- 'DIX (5)'
# d[d == 'e'] <- 'DIX (10)'
# d[d == 'f'] <- 'DIX (20)'
# d[d == 'g'] <- 'AAR (5)'
# d[d == 'h'] <- 'AAR (10)'
# d[d == 'i'] <- 'AAR (20)'

# d[d == 'a'] <- 'DIB (H)'
# d[d == 'b'] <- 'DIB (S)'
# d[d == 'c'] <- 'DIX (H)'
# d[d == 'd'] <- 'DIX (S)'
# d[d == 'e'] <- 'AAR (H)'
# d[d == 'f'] <- 'AAR (S)'

#d <- d[d$x != 0 & d$grp != 'ACRTB (S)',]

#d[nrow(d) + 1,] = c(0.0,"ACRTB (S)")

# df_mean <- d %>% 
#   group_by(grp) %>% 
#   summarize(average = mean(x)) %>%
#   ungroup()
# 
# legend_ord <- levels(with(d, reorder(grp, x)))



#factor(grp, levels = c('AAR (5)','AAR (10)','AAR (20)','DIB (5)','DIB (10)','DIB (20)','DIX (5)','DIX (10)','DIX (20)'))


t_d <- transpose(d)
colnames(t_d) <- c("x")
t_dN <- transpose(dN)
colnames(t_dN) <- c("grp")
t_dC <- transpose(dC)
colnames(t_dC) <- c("cat")

merged = cbind(t_d,t_dN,t_dC)

# splitD = split(merged,cumsum(1:nrow(merged)%in%41))


my_sum <- merged %>%
  group_by(grp,cat) %>%
  summarise( 
    n=n(),
    mean=mean(x),
    sd=sd(x)
  ) %>%
  mutate( se=sd/sqrt(n))  %>%
  mutate( ic=se * qt((1-0.05)/2 + .5, n-1))

my_sum = my_sum[order(as.character(my_sum$cat),as.character(my_sum$grp)),]

# found =  my_sum[my_sum$grp=='ADULT' & my_sum$cat=='Age',]
# found$grp = 'ADULT AGE'
# 
# my_sum[my_sum$grp=='ADULT' & my_sum$cat=='Age',] = found


# my_sum$grp  <- with(my_sum, reorder(grp, as.character(cat)))
my_sum$grp = factor(my_sum$grp, levels = c('ADMIN','ADULT','CHILD','VISITOR','MANAGE','CONTROL','VIEW','CRITICAL','NONCRITICAL','COMMON','INDEFINITE','UNCOMMON','EXTERNAL','INTERNAL','ADULT AGE','KID','TEEN','ALONE','TOGETHER','HOUSE','PERSONAL','REQUESTED'))


# ggplot(my_sum) +
#   geom_bar( aes(x=cat, y=mean, fill=grp), colour="black", stat="identity", alpha=0.5) +
#   scale_fill_viridis(discrete=TRUE, option="D") +
#   theme_ipsum_rc() +
#   labs(x="",
#        y="Percentage (%)",
#        title = "Profile of Denied Attacks",
#        fill = "Properties") +
#   theme(axis.title.x = element_text(hjust = 0.5, size = 14),
#         axis.title.y = element_text(hjust = 0.5, size = 14),
#         text = element_text(size = 14),
#         axis.text.x = element_text(size = 14, margin = margin(t = 5), angle = 90, vjust = 0.5, hjust=1),
#         axis.text.y = element_text(size = 14, margin = margin(r = 5)),
#         axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
#         axis.ticks.x = element_line(color="black", size = 0.1),
#         axis.ticks.y = element_line(color="black", size = 0.1)) +
#   scale_y_continuous(expand = c(0, 0), limits = c(0,105)) +
#   geom_col() +
#   geom_text(aes(y=mean, label=grp),position=position_stack(vjust=0.5),color = "black",
#             show.legend = FALSE, size=3)

my_sum <- my_sum %>% filter_at(vars(starts_with("mean")), any_vars(. != 0))

ggplot(my_sum, aes(x=cat, y=mean, fill=grp)) +
  scale_fill_viridis(discrete=TRUE, option="D") +
  theme_ipsum_rc() +
  labs(x="",
       y="Percentage (%)",
       title = "Profile of Denied Attacks - 10 Attacks",
       fill = "Properties") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5), angle = 90, vjust = 0.5, hjust=1),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  scale_y_continuous(expand = c(0, 0), limits = c(0,105)) +
  geom_bar(colour="black", stat="identity", alpha=0.5) +
  geom_text(aes(y=mean, label=grp, fontface = "bold"),position=position_stack(vjust=0.5),color = "black",
            show.legend = FALSE, size=3)
