goal: sqr maxarray bubble 

sqr: sqr.s    
	gcc -static -o sqr sqr.s 

maxarray: maxarray.s maxarray.c
	gcc -static -o maxarray maxarray.c maxarray.s

bubble: bubble.s    
	gcc -static -o bubble bubble.c bubble.s

clean:
	rm -f sqr maxarray bubble
