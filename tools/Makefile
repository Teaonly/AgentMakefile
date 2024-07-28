.PHONY: all

FLAGS = -Wall -O3 
INC = 
LINK = -lssl -lcrypto 

all: kimi_chat 

kimi_chat: kimi_chat.cpp
	g++ $(FLAGS) -o $@ $< $(INC) $(LINK)

clean:
	rm -f kimi_chat 
