CC=arm-apple-darwin-cc
LD=$(CC)
LDFLAGS=-lobjc -framework CoreFoundation -framework Foundation -framework UIKit -framework LayerKit -framework CoreGraphics -larmfp -framework GraphicsServices

all:	NESApp	

NESApp:	main.o NESApp.o MainView.o FileBrowser.o EmulationView.o ControllerView.o ScreenView.o InfoNES/K6502.o InfoNES/InfoNES.o InfoNES/InfoNES_Mapper.o InfoNES/InfoNES_pAPU.o InfoNES_iPhone.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o InfoNES/*.o NESApp
