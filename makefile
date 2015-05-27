

objects = game_new.o opponent.o conversion.o rate.o Card.o socket.o

game : $(objects)
	g++ -o game $(objects)
game_new.o : opponent.h conversion.h Card.h socket.h constant.h
	g++ -c game_new.cpp
opponent.o : opponent.h conversion.h constant.h
	g++ -c opponent.cpp
conversion.o : conversion.h Card.h
Card.o : Card.h
socket.o : socket.h

   .PHONY : clean
   clean :
	rm game $(objects)

