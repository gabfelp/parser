#use /usr/lib

GPP=g++ -std=c++11
INCLUDES=/
CPPFILES=*.cpp #./utils/*.cpp
LIBS=/usr/lib #libs
 

main: parser.cpp 
	$(GPP) $(CPPFILES)  -I. -g -o parser -L$(LIBS) -lgumbo

clean:
	rm parser


.PHONY: all clean
