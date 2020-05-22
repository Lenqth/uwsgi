FROM python:3.6

WORKDIR "/opt/uwsgi/"

RUN apt-get update -qq && apt-get install -qqyf software-properties-common

# RUN add-apt-repository ppa:deadsnakes/ppa -y && add-apt-repository ppa:ondrej/php -y

RUN apt-get update -qq && \
    apt-get --fix-missing -qqy install libxml2-dev libpcre3-dev libcap2-dev libargon2-0-dev libsodium-dev liblua5.1-0-dev libjansson-dev libldap2-dev libpq-dev libpam0g-dev libsqlite3-dev libyaml-dev libzmq3-dev libmatheval-dev libperl-dev libonig-dev libdb-dev libqdbm-dev libbz2-dev libwrap0-dev libgeoip-dev libv8-dev libxslt1-dev libboost-thread-dev libboost-filesystem-dev libssl-dev libacl1-dev python-greenlet-dev libgloox-dev gccgo cli-common-dev mono-devel mono-mcs uuid-dev curl check

# php7.2-dev libphp7.2-embed openjdk-8-jdk 
ADD . .

RUN CFLAGS="-I/usr/include/python3.6m" UWSGI_PROFILE="gevent" python uwsgiconfig.py --build
CMD python setup.py test 