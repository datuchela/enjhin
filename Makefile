build: 
	./scripts/build.sh

win:
	./scripts/build.sh win64

release:
	./scripts/build.sh && ./scripts/release.sh

release-win:
	./scripts/build.sh win64 && ./scripts/release.sh win64

debug: 
	./scripts/build.sh && ./game

run:
	./game

raylib:
	cd ./raylib-5.0/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

