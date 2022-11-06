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

dvcList = list()
dvcListNames = list()
dvcListCat = list()

usrList = list()
usrListNames = list()
usrListCat = list()

# index = 1
for (i in 1:length(files)) {
  #print(files[i])
  scenarioFile = paste(substr(files[i], 0, nchar(files[i])-44), "zash_simulation_scenario", substr(files[i], nchar(files[i])-20, nchar(files[i])), sep="")
  
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
  
  if (isHard & datarate == 100 & attacks == 10 & alterations == 5 & users == 2) {
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
    
    uslVals = as.numeric(unlist(regmatches(usl,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",usl))
    )      )
    uslVals = split(uslVals, ceiling(seq_along(uslVals)/(length(uslVals)/2)))[[2]]
    actVals = as.numeric(unlist(regmatches(act,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",act))
    )      )
    
    actVals = split(actVals, ceiling(seq_along(actVals)/(length(actVals)/2)))[[2]]
    dcsVals = as.numeric(unlist(regmatches(dcs,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dcs))
    )      )
    dcsVals = split(dcsVals, ceiling(seq_along(dcsVals)/(length(dcsVals)/2)))[[2]]
    timVals = as.numeric(unlist(regmatches(tim,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",tim))
    )      )
    timVals = split(timVals, ceiling(seq_along(timVals)/(length(timVals)/2)))[[2]]
    locVals = as.numeric(unlist(regmatches(loc,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",loc))
    )      )
    locVals = split(locVals, ceiling(seq_along(locVals)/(length(locVals)/2)))[[2]]
    ageVals = as.numeric(unlist(regmatches(age,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",age))
    )      )
    ageVals = split(ageVals, ceiling(seq_along(ageVals)/(length(ageVals)/2)))[[2]]
    grpVals = as.numeric(unlist(regmatches(grp,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",grp))
    )      )
    grpVals = split(grpVals, ceiling(seq_along(grpVals)/(length(grpVals)/2)))[[2]]
    acwVals = as.numeric(unlist(regmatches(acw,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",acw))
    )      )
    acwVals = split(acwVals, ceiling(seq_along(acwVals)/(length(acwVals)/2)))[[2]]
    dvcVals = as.numeric(unlist(regmatches(dvc,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",dvc))
    )      )
    dvcVals = split(dvcVals, ceiling(seq_along(dvcVals)/(length(dvcVals)/2)))[[2]]
    usrVals = as.numeric(unlist(regmatches(usr,
                                           gregexpr("[[:digit:]]+\\.*[[:digit:]]*",usr))
    )      )
    usrVals = split(usrVals, ceiling(seq_along(usrVals)/(length(usrVals)/2)))[[2]]
    
    # print("-------------------------------")
    # for (v in uslVals) {
    #   print(v)
    # }
    
    
    
    names <- str_match(usl, ":\\s*(.*?)\\s*=")[,2]
    
    # tempList = c()
    index = 1
    for (x in 1:length(uslVals)) {
      # print(uslVals[[x]])
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
      # print(actVals[[x]])
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
      # print(dcsVals[[x]])
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
      # print(acwVals[[x]])
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
      # print(locVals[[x]])
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
      # print(timVals[[x]])
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
      # print(ageVals[[x]])
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
      # print(grpVals[[x]])
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
    
    index = 1
    count = 0
    for (x in 1:length(dvcVals)) {
      # print(dvcVals[[x]])
      if (x == 1 || count == 3) {
        if (index > length(dvcListNames)) {
          dvcListNames[[index]] = list()
        }
        dvcListNames[[index]] = append(dvcListNames[[index]], dvcVals[[x]])
        count = 0
      } else if (x %% 3 == 0) {
        if (index > length(dvcList)) {
          dvcList[[index]] = list()
          dvcListCat[[index]] = list()
        }
        dvcList[[index]] <- append(dvcList[[index]], dvcVals[[x]])
        dvcListCat[[index]] <- append(dvcListCat[[index]], "Device")
        index = index + 1
      }
      if (index > 29) {
        break;
      }
      count = count + 1
    }
  }
}

d <- data.frame(uslList, actList, dcsList, timList, locList, ageList, grpList, acwList)
dN <- data.frame(uslListNames, actListNames, dcsListNames, timListNames, locListNames, ageListNames, grpListNames, acwListNames)
dC <- data.frame(uslListCat, actListCat, dcsListCat, timListCat, locListCat, ageListCat, grpListCat, acwListCat)

# d <- data.frame(dvcList)
# dN <- data.frame(dvcListNames)
# dC <- data.frame(dvcListCat)


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
       title = "Profile of Successful Attacks",
       fill = "Properties") +
  theme(axis.title.x = element_text(hjust = 0.5, size = 14), 
        axis.title.y = element_text(hjust = 0.5, size = 14), 
        text = element_text(size = 14),
        axis.text.x = element_text(size = 14, margin = margin(t = 5), angle = 90, vjust = 0.5, hjust=1),
        axis.text.y = element_text(size = 14, margin = margin(r = 5)),
        axis.line = element_line(color="black", size = 0.1, arrow = arrow(type='open', length = unit(8,'pt'))),
        axis.ticks.x = element_line(color="black", size = 0.1),
        axis.ticks.y = element_line(color="black", size = 0.1)) +
  #scale_y_continuous(expand = c(0, 0), limits = c(0,105)) +
  geom_bar(colour="black", stat="identity", alpha=0.5) +
  geom_text(data=subset(my_sum, mean>5),aes(y=mean, label=grp, fontface = "bold"),position=position_stack(vjust=0.5),color = "black",
            show.legend = FALSE, size=3) +
  guides(fill=guide_legend(ncol=2))
