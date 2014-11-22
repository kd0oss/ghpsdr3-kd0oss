

bin_PROGRAMS = dspserver

noinst_LIBRARIES = libsdrutil.a

dspserver_CFLAGS = -O3 -Wall -I$(top_srcdir)/trunk/src/wdsp -Wno-unused-function $(THREAD_DEBUG_CFLAGS) @SIMD_FLAGS@
dspserver_LDADD = ../wdsp/libwdsp.a libsdrutil.a -lrt -lssl -lcrypto -lfftw3 

dspserver_SOURCES = main.c\
         ozy.c\
         soundcard.c\
         client.c\
         audiostream.c \
	 audiostream.h  \
buffer.h       \
client.h       \
codec2loc.h    \
wdsp.h        \
main.h         \
ozy.h          \
soundcard.h    \
version.h	   \
register.h	   \
register.c     \
rtp.h          \
rtp.c          \
G711A.h        \
G711A.c

libsdrutil_a_SOURCES = util.h         \
                       util.c

