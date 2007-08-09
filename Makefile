CC = arm-apple-darwin-cc 
LD = $(CC)
LDFLAGS = -lobjc \
          -framework CoreFoundation \
          -framework Foundation \
          -framework UIKit \
          -framework LayerKit \
          -framework CoreGraphics \
          -framework GraphicsServices \
          -framework CoreSurface \
          -framework CoreAudio \
          -larmfp 

CFLAGS = -DPORTRAIT
#CFLAGS = -DLANDSCAPE

all:	NES package

NES:	src/main.o src/NESApp.o src/MainView.o src/FileBrowser.o src/EmulationView.o src/ScreenView.o src/InfoNES/K6502.o src/InfoNES/InfoNES.o src/InfoNES/InfoNES_Mapper.o src/InfoNES/InfoNES_pAPU.o src/InfoNES_iPhone.o src/ControllerView.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

package:
	rm -rf build
	mkdir build
	cp -r ./src/NES.app ./build
	mv NES ./build/NES.app

clean:
	rm -f src/*.o src/InfoNES/*.o NES
	rm -rf ./build
