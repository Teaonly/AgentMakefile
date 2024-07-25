.PHONY: all

FLAGS = -Wall -O3 
INC = 
LINK = -lssl -lcrypto 

all: chat 

chat: chat.cpp
	g++ $(FLAGS) -o $@ $< $(INC) $(LINK)

clean:
	rm -f chat 
