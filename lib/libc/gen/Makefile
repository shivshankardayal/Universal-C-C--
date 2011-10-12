
D2= 
D3=-D_BSD_VA_LIST_="char *"
														

CC=igcc  ${D2} ${D3}  ${INCLUDES} -O2  -c 
AR=iar -qc 


.c.o:
	${CC} $*.c

OBJ =\
_err.o \
_errno.o \
_errx.o \
_sys_errlist.o \
_sys_nerr.o \
_sys_siglist.o \
_verr.o \
_verrx.o \
_vwarn.o \
_vwarnx.o \
_warn.o \
_warnx.o \
alarm.o \
assert.o \
basename.o \
bswap16.o \
bswap32.o \
bswap64.o \
chflags.o \
clock.o \
crypt.o \
ctermid.o \
ctype_.o \
daemon.o \
devname.o \
dirname.o \
err.o \
errlist.o \
errno.o \
errx.o \
fnmatch.o \
fstab.o \
ftok.o \
fts.o \
getbsize.o \
getcap.o \
icvmpath.o \
isctype.o \
lockf.o \
nice.o \
pause.o \
popen.o \
psignal.o \
pw_scan.o \
raise.o \
setrgid.o \
setruid.o \
siginterrupt.o \
siglist.o \
signal.o \
signame.o \
stringlist.o \
time.o \
times.o \
timezone.o \
tolower_.o \
toupper_.o \
ttyname.o \
ttyslot.o \
ualarm.o \
unvis.o \
usleep.o \
utime.o \
sysctl.o \
valloc.o \
verr.o \
verrx.o \
vis.o \
vwarn.o \
vwarnx.o \
wait.o \
wait3.o \
waitpid.o \
warn.o \
warnx.o \
getgroup.o \
getuser.o \
sysconf.o \
setmode.o \
execl.o \
execle.o \
execlp.o \
execv.o \
execvp.o \
syslog.o \
getnetgrent.o \
initgroups.o \
readv.o \
preadv.o \
pread.o \
writev.o \
pwritev.o \
pwrite.o \
getgrouplist.o \
glob.o \
getpass.o \
uname.o \
getttyent.o \
getusershell.o \
sleep.o \
scandir.o 

#Lint_flt_rounds.o \
#Lint_fpgetmask.o \
#Lint_fpgetround.o \
#Lint_fpgetsticky.o \
#Lint_fpsetmask.o \
#Lint_fpsetround.o \
#Lint_fpsetsticky.o \
#Lint_bswap16.o \
#Lint_bswap32.o \
#Lint_fabs.o \
#getgrent.o \
#getpwent.o \
#sigsetops.o \
#pwcache.o \
#__errlist14.o \
#__siglist14.o \
#__signame14.o \
#__sigsetops14.o \
#Lint_alloca.o 
#getdomainname.o \
#difftime.o \ obsolete
#ctime.o \ obsolete 
#confstr.o \
#Lint___setjmp14.o \
#Lint___sigsetjmp14.o \
#Lint__setjmp.o \
#longjmp.o \
#disklabel.o \
#gethostname.o \
#getloadavg.o \
#getlogin.o \
#getmntinfo.o \
#getpagesize.o \
#getsubopt.o \
#isatty.o \
#exec.o \
#nlist.o \
#nlist_aout.o \
#nlist_ecoff.o \
#nlist_elf32.o \
#nlist_elf64.o \
#setdomainname.o \
#sethostname.o \
#setjmp.o \
#setjmperr.o \
#setproctitle.o \
#opendir.o \
#rewinddir.o \
#seekdir.o \
#telldir.o \
#readdir.o \
#closedir.o \
#getcwd.o \
#errlst.o \
#Lint_bswap64.o \
#__glob13.o \
#__fts13.o \
#__glob13.o \
#Lint_modf.o \

all: ${OBJ} gen.a

gen.a: ${OBJ}
	touch gen.a
	rm gen.a
	${AR} gen.a ${OBJ}


clean:
	rm *.o *.a

ctime.o : ctime.c
	${CC} -fno-strength-reduce ctime.c
	 