1p-ldpreload.so:
	$(CC) $(CFLAGS) -Wall -fPIC -shared -o 1p-ldpreload.so 1p-ldpreload.c $(LDFLAGS) -ldl

build: 1p-ldpreload.so
install: build
	$(INSTALL) -d $(DESTDIR)/opt/1Password
	$(INSTALL) -t $(DESTDIR)/opt/1Password 1p-ldpreload.so

.PHONY: clean
clean:
	rm 1p-ldpreload.so
