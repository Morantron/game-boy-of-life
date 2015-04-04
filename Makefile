default: build

build: clean
	mkdir -p build
	/opt/gbdk/bin/lcc src/main.c -o build/dist.gb

clean:
	rm -rf build

emulate: build
	vba build/dist.gb

setup:
	#TODO install gbdk and vba
