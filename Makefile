.PHONY: bild visualize score

PROBLEM := ConstrainedPermutation
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}

build: a.out
a.out: tester/main.cpp ${PROBLEM}.cpp
	${CXX} ${CXXFLAGS} -g -I. -DLOCAL $<
a.out.visualize: tester/main.cpp ${PROBLEM}.cpp
	${CXX} ${CXXFLAGS} -g -I. -DLOCAL $< -o $@ -DVISUALIZE
a.out.debug: tester/main.cpp ${PROBLEM}.cpp
	${CXX} -std=c++11 -Wall -g -I. -DLOCAL -fsanitize=undefined -D_GLIBCXX_DEBUG $< -o $@ -DVISUALIZE

tester/${PROBLEM}Vis.class: tester/${PROBLEM}Vis.java
	cd tester ; javac ${PROBLEM}Vis.java
visualize: a.out.visualize tester/${PROBLEM}Vis.class
	cd tester ; java ${PROBLEM}Vis -exec ../a.out.visualize -seed ${SEED} -debug
score: a.out tester/${PROBLEM}Vis.class
	cd tester ; for seed in `seq 10` ; do java ${PROBLEM}Vis -exec ../a.out -seed $$seed | grep 'Score = ' ; done

WEBDRIVER := /usr/lib/chromium-browser/chromedriver
submit/example:
	oj --webdriver ${WEBDRIVER} s -y 'https://community.topcoder.com/tc?module=MatchDetails&rd=16997' -l C++ ${PROBLEM}.cpp --open
submit/full:
	oj --webdriver ${WEBDRIVER} s -y 'https://community.topcoder.com/tc?module=MatchDetails&rd=16997' -l C++ ${PROBLEM}.cpp --open --full-submission
