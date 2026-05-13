SP=server
C=client
CFLAGS=-Wall
all:$(SP) $(C)

$(SP):$(SP).c
	gcc $(CFLAGS) $< -o $@
$(C):$(C).c
	gcc $(CFLAGS) $< -o $@
clean:
	rm -f $(SP) $(C)
