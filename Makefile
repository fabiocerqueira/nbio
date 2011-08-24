all: 
	python build.py build_ext -i
clean: 
	rm nbio.so
	rm -Rf build/
