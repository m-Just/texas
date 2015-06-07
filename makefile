
GAME = game_new
GN = game

objects = $(GAME).o opponent.o conversion.o rate.o Card.o socket.o

$(GN) : $(objects)
	g++ -o $(GN) $(objects)
$(GAME).o : opponent.h conversion.h Card.h socket.h constant.h
	g++ -g -c $(GAME).cpp
opponent.o : opponent.h conversion.h constant.h
	g++ -g -c opponent.cpp
socket.o : socket.h conversion.h constant.h
	g++ -g -c socket.cpp
conversion.o : conversion.h Card.h
	g++ -g -c conversion.cpp
rate.o : Card.h constant.h
	g++ -g -c rate.cpp
Card.o : Card.h constant.h
	g++ -g -c Card.cpp
   .PHONY : clean
   clean :
	rm $(GN) $(objects)

