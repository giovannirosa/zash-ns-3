# Libraries
library(ggplot2)
library(dplyr)
library(hrbrthemes)
library(viridis)

# The dataset is provided in the gapminder library
library(gapminder)
data <- gapminder %>% filter(year=="2007") %>% dplyr::select(-year)

print(data)

auList <- c(1, 2, 3, 4)

cdList <- c(1, 2, 4, 6, 8)

auCol <- c()
cdCol <- c()
prCol <- c()
stCol <- c()

for (au in auList) {
  for (cd in cdList) {
    pr <- au*cd
    st <- ifelse(pr<=2, "low", ifelse(pr>2 & pr<8, "med", "hig"))
    
    auCol <- append(auCol, au)
    cdCol <- append(cdCol, cd)
    prCol <- append(prCol, pr)
    stCol <- append(stCol, st)
  }
}

prData <- data.frame(auCol, cdCol, prCol, stCol)

print(auList)

pr <- c(au=1,)



# Most basic bubble plot
prData %>%
  arrange(desc(pop)) %>%
  mutate(country = factor(country, country)) %>%
  ggplot(aes(x=gdpPercap, y=lifeExp, size=pop, fill=continent)) +
  geom_point(alpha=0.5, shape=21, color="black") +
  #scale_size(range = c(.1, 24), name="Population (M)") +
  scale_fill_viridis(discrete=TRUE, guide=FALSE, option="A") +
  theme_ipsum() +
  theme(legend.position="bottom") +
  ylab("No. of Admin Users (AU)") +
  xlab("No. of Critical Devices (CD)") +
  theme(legend.position = "none")


ggplot(prData, aes(x=auCol, y=cdCol, size=prCol, fill=prCol)) + 
  geom_point(alpha=0.5, shape=21, color="black") +
  scale_fill_viridis(discrete=FALSE, option="D", direction=-1) +
  theme_ipsum_rc() +
  labs(size="PR", 
       fill="PR", 
       x="No. of Admin Users (AU)", 
       y="No. of Critical Devices (CD)",
       title = "Privacy Risk (PR)")
