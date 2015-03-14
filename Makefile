default: build

build:
	/opt/gbdk/bin/lcc main.c -o dist.gb

emulate: build
	vba dist.gb

setup:
	#TODO install gbdk and vba
