
GAME = play_mate_always_average

objects = $(GAME).o opponent.o conversion.o rate.o Card.o socket.o

avg : $(objects)
	g++ -o avg $(objects)
$(GAME).o : opponent.h conversion.h Card.h socket.h constant.h
	g++ -g -c $(GAME).cpp
opponent.o : opponent.h conversion.h constant.h
	g++ -g -c opponent.cpp
socket.o : socket.h conversion.h
conversion.o : conversion.h Card.h
Card.o : Card.h
   .PHONY : clean
   clean :
	rm avg $(objects)

