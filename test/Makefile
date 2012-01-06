#Steps to build the TC_bundle.c 
#1.build the bundle code 
#2.make
#3. TC_bundle binary file would be created.
export PKG_CONFIG_PATH=../debian/libbundle-dev/usr/lib/pkgconfig/

all:
	sbs -e gcc -Wall -o TC_bundle TC_bundle.c `pkg-config --libs bundle`
clean:
	rm -rf *.exe
