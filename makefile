
GAME = play_mate_always_average

objects = $(GAME).o opponent.o conversion.o rate.o Card.o socket.o

avg : $(objects)
	g++ -o avg $(objects)
$(GAME).o : opponent.h conversion.h Card.h socket.h constant.h
	g++ -c $(GAME).cpp
opponent.o : opponent.h conversion.h constant.h
	g++ -c opponent.cpp
conversion.o : conversion.h Card.h
Card.o : Card.h
socket.o : socket.h

   .PHONY : clean
   clean :
	rm avg $(objects)

