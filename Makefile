build: 
	./build.sh

win:
	./build-win64.sh

debug: 
	./build.sh && ./dist/game

run:
	./dist/game

raylib:
	cd ./raylib-5.0/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

