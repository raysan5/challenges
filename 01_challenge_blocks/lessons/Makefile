CFLAGS = -Wall -std=c99 -O3 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt

all:	link

01:	01_blocks_game_intro.c
	gcc -c $(CFLAGS) $(LDLIBS) 01_blocks_game_intro.c
	gcc -o game $(CFLAGS) 01_blocks_game_intro.o $(LDLIBS)

02:	02_blocks_game_drawing.c
	gcc -c $(CFLAGS) $(LDLIBS) 02_blocks_game_drawing.c
	gcc -o game $(CFLAGS) 02_blocks_game_drawing.o $(LDLIBS)

03:	03_blocks_game_inputs.c
	gcc -c $(CFLAGS) $(LDLIBS) 03_blocks_game_inputs.c
	gcc -o game $(CFLAGS) 03_blocks_game_inputs.o $(LDLIBS)

04:	04_blocks_game_collisions.c
	gcc -c $(CFLAGS) $(LDLIBS) 04_blocks_game_collisions.c
	gcc -o game $(CFLAGS) 04_blocks_game_collisions.o $(LDLIBS)

05:	05_blocks_game_textures.c
	gcc -c $(CFLAGS) $(LDLIBS) 05_blocks_game_textures.c
	gcc -o game $(CFLAGS) 05_blocks_game_textures.o $(LDLIBS) 	

06:	06_blocks_game_text.c
	gcc -c $(CFLAGS) $(LDLIBS) 06_blocks_game_text.c
	gcc -o game $(CFLAGS) 06_blocks_game_text.o $(LDLIBS)

07:	07_blocks_game_audio.c
	gcc -c $(CFLAGS) $(LDLIBS) 07_blocks_game_audio.c
	gcc -o game $(CFLAGS) 07_blocks_game_audio.o $(LDLIBS) 	

clean:
	rm -f *~ *# *.o game


