CC:=clang++
OUT:=main
CFLAGS:=-g -Wall -std=c++20 -I./src/include -lglfw -lfreetype -I/usr/include/freetype2
RFLAGS:=-std=c++20 -DNDEBUG -O3 -I./src/include -lglfw -lfreetype -I/usr/include/freetype2
CFILES:=$(wildcard src/*.cpp) src/glad.c
HFILES:=$(wildcard src/*.hpp)
OBJS:=$(patsubst src/%.cpp, obj/%.o, $(CFILES))

debug: $(OBJS)
	$(CC) $(CFLAGS) -o bin/debug/$(OUT) $(OBJS)

release: $(CFILES)
	$(CC) $(RFLAGS) -o bin/release/$(OUT) $(CFILES)

$(OBJS): $(CFILES)
	$(CC) $(CFLAGS) -c $(patsubst obj/%.o, src/%.cpp, $@) -o $@

clean:
	del $(patsubst obj/%, obj\\%, $(OBJS))
