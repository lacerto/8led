8led: 8led.o gpio.o modes.o
	gcc -o $@ $^ -lwiringPi -lm -lncurses -lmenu

8led.o: 8led.c gpio.h modes.h
	gcc -c $<

gpio.o: gpio.c gpio.h
	gcc -c $<

modes.o: modes.c modes.h gpio.h
	gcc -c $<

.PHONY: clean
clean:
	rm *.o 8led
