#include <FVWMconfig.h>
#include <string.h>

#if HAVE_UNAME
/* define mygethostname() by using uname() */

#include <sys/utsname.h>

int mygethostname(char *client, int length)
{
  struct utsname sysname;
  
  uname(&sysname);
  strncpy(client,sysname.nodename,length);
  return 0;
}
#else 
#if HAVE_GETHOSTNAME
/* define mygethostname() by using gethostname() :-) */

int mygethostname(char *client, int length)
{
  return gethostname(client, length);
}
#else
int mygethostname(char *client, int length)
{
  *client = 0;
  return 0;
}
#endif
#endif
