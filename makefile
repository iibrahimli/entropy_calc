entropy : entropy.c
	gcc entropy.c -o entropy -lm -g

clean : 
	rm *.o entropy

