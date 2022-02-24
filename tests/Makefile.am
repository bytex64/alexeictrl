AM_CFLAGS = -I../libalexeictrl
AM_LDFLAGS = -L../libalexeictrl -lalexeictrl

TESTS = botstate_test

noinst_PROGRAMS = botstate_test network_test parport_test parport_ascii

botstate_test_SOURCES = botstate_test.c

network_test_SOURCES = network_test.c
network_test_LDADD = -lpthread

parport_test_SOURCES = parport_test.c

parport_ascii_SOURCES = parport_ascii.c
