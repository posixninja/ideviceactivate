all:
	make -C src

install:
	cp src/ideviceactivate /usr/local/bin/ideviceactivate

clean:
	rm src/ideviceactivate
