#include <FVWMconfig.h>

#if HAVE_UNAME
/* define mygethostname() by using uname() */

#include <sys/utsname.h>

int mygethostname(char *client, int length)
{
  struct utsname sysname;
  
  uname(&sysname);
  strncpy(client,sysname.nodename,length);
}
#else 
#if HAVE_GETHOSTNAME
/* define mygethostname() by using gethostname() :-) */

int mygethostname(char *client, int length)
{
  gethostname(client, length);
}
#else
int mygethostname(char *client, int length)
{
  *client = 0;
}
#endif
#endif
