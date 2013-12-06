CC=gcc
CFLAGS=-c -Wall -I ./src/includes/
LDFLAGS=-lm
SOURCES=./src/c/annealPoints/logging.c ./src/c/annealPoints/vector.c \
        ./src/c/annealPoints/sphere.c ./src/c/annealPoints/annealPoints.c \
	./src/c/annealPoints/sa.c
TESTSOURCES=./src/c/annealPoints/vector.c ./src/c/annealPoints/sphere.c \
        ./src/c/test/test.c
OBJECTS=$(SOURCES:.c=.o)
TESTOBJECTS=$(TESTSOURCES:.c=.o)
EXECUTABLE=annealPoints
TESTEXECUTABLE=test
DOCDIR=./doc
VERSION=1.0
DISTRIBUTABLE=sa-sphere-$(VERSION)
DISTDIR=./dist/sa-sphere-$(VERSION)


all: $(EXECUTABLE) $(TESTEXECUTABLE)

.PHONY: $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.PHONY: $(TESTEXECUTABLE)
$(TESTEXECUTABLE): $(TESTOBJECTS)
	$(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@

lint:
	splint -I./src/includes/ -warnposix -exportlocal $(SOURCES)

.PHONY: doc
doc:
	doxygen doxygen.cfg

dist: clean doc
	mkdir -p $(DISTDIR)/src
	cp doc $(DISTDIR)/ -R
	cp src/c $(DISTDIR)/src -R
	cp src/includes $(DISTDIR)/src -R
	cp src/R $(DISTDIR)/src -R
	cp src/doc/report.pdf $(DISTDIR)/doc -R
	cp *.awk $(DISTDIR)/
	cp *.sh $(DISTDIR)/
	cp *.cfg $(DISTDIR)/
	cp README $(DISTDIR)/
	cp Makefile $(DISTDIR)/
	tar -czf dist/$(DISTRIBUTABLE).tar.gz -C ./dist $(DISTRIBUTABLE)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(TESTOBJECTS) $(EXECUTABLE) $(TESTEXECUTABLE) $(DOCDIR) $(DISTDIR)
