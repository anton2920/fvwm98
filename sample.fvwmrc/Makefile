#
SHELL=/bin/sh



top_srcdir=..
srcdir=.
prefix=/usr/local
exec_prefix=${prefix}
bin_dir=${prefix}/bin
fvwm_rc=.fvwm95rc
fvwm_name=fvwm95
fvwm_dir=${prefix}/lib/X11/${fvwm_name}

INSTALL=/usr/bin/install -c
INSTALL_DATA=${INSTALL} -m 644
RM=rm -f

RC=system${fvwm_rc}

all: 

depend:

install: 
	${INSTALL_DATA} ${RC} ${fvwm_dir} 

clean:
	${RM} *~ core *.bak

distclean: clean


#
