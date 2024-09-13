.PHONY: all

FLAGS = -Wall -O3 
INC = 
LINK = -lssl -lcrypto 

all: siflow_chat kimi_chat 

kimi_chat: kimi_chat.cpp
	g++ $(FLAGS) -o $@ $< $(INC) $(LINK)

siflow_chat: siflow_chat.cpp
	g++ $(FLAGS) -o $@ $< $(INC) $(LINK)

clean:
	rm -f kimi_chat siflow_chat  
