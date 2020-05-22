PYTHON := python

all:
	$(PYTHON) uwsgiconfig.py --build $(PROFILE)

clean:
	$(PYTHON) uwsgiconfig.py --clean

check:
	$(PYTHON) uwsgiconfig.py --check

plugin.%:
	$(PYTHON) uwsgiconfig.py --plugin plugins/$* $(PROFILE)

tests:
	$(PYTHON) uwsgiconfig.py --build unittest
	cd check && make && make test

docker-test:
	docker build -t uwsgi-build -f docker/Dockerfile .
	docker run --rm -it --entrypoint /bin/bash uwsgi-build -c 'cd /uwsgi; make tests; python setup.py test'

%:
	$(PYTHON) uwsgiconfig.py --build $@

.PHONY: all clean check tests
