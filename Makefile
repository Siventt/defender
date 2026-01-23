
RAYLIB ?= ../raylib/src/

all:	
	cc -o defender src/defender.c -I $(RAYLIB) -I/opt/vc/include -L $(RAYLIB) -L/opt/vc/lib -lraylib -lm -lpthread -lbrcmGLESv2 -lbrcmEGL -lvcos -lvchiq_arm -lbcm_host
	
	# Para Linux x64
	#gcc -o defender src/defender.c -I $(RAYLIB) -L $(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	
	#cc -o defender src/defender.c -I../raylib/src -I/opt/vc/include -L../raylib/src -L/opt/vc/lib -lraylib -lm -lpthread -lbrcmGLESv2 -lbrcmEGL -lvcos -lvchiq_arm -lbcm_host
