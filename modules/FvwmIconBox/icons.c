/***********************************************************************
 * icons.c
 * 	Based on icons.c of GoodStuff:
 *		Copyright 1993, Robert Nation.
 ***********************************************************************/

/* Copyright 1993, Robert Nation. No guarantees or warantees or anything
 * are provided or implied in any way whatsoever. Use this program at your
 * own risk. Permission to use this program for any purpose is given,
 * as long as the copyright is kept intact. */


/***********************************************************************
 *
 * Derived from fvwm icon code
 *
 ***********************************************************************/

#include <FVWMconfig.h>

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>

#include "../../fvwm/module.h"

#ifdef NeXT
#include <fcntl.h>
#endif

#include "FvwmIconBox.h"

#ifdef XPM
#include <X11/xpm.h>
#endif /* XPM */
#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif /* SHAPE */

#define ICON_EVENTS (ExposureMask |\
ButtonReleaseMask | ButtonPressMask | EnterWindowMask | LeaveWindowMask)

/****************************************************************************
 *
 * Creates an Icon Window
 *
 ****************************************************************************/
void CreateIconWindow(struct icon_info *item)
{
  unsigned long valuemask;		/* mask for create windows */
  XSetWindowAttributes attributes;	/* attributes for create windows */

  attributes.background_pixel = icon_back_pix;
  attributes.event_mask = ExposureMask;
  valuemask =  CWEventMask | CWBackPixel;

  /* pixmap */
  if (max_icon_height != 0){
  item->icon_pixmap_w =
    XCreateWindow(dpy, icon_win, 0, 0,
		  max(max_icon_width,item->icon_w),
		  max(max_icon_height,item->icon_h),
		  0,CopyFromParent,CopyFromParent,
		  CopyFromParent,valuemask,&attributes);
  XSelectInput(dpy, item->icon_pixmap_w, ICON_EVENTS);
  }

  /* label */
  item->IconWin =
    XCreateWindow(dpy, icon_win, 0, 0,
		  max_icon_width,
		  max_icon_height + 10,
		  0,CopyFromParent,CopyFromParent,
		  CopyFromParent,valuemask,&attributes);
  XSelectInput(dpy, item->IconWin, ICON_EVENTS);
}

/****************************************************************************
 *
 * Loads an icon file and combines icon shape masks after a resize
 *
 ****************************************************************************/
void ConfigureIconWindow(struct icon_info *item)
{
  Pixmap temp;
  int hr = icon_relief/2;

  XSelectInput(dpy, item->id, PropertyChangeMask);
  item->wmhints = XGetWMHints(dpy, item->id);

   if (max_icon_height == 0)
     return;

  if (item->icon_file != NULL && (!(item->extra_flags & DEFAULTICON) || !(item->wmhints &&
					      item->wmhints->flags &
					      (IconPixmapHint|IconWindowHint)))){
    /* monochrome bitmap */
    GetBitmapFile(item);

    /* color pixmap */
    if((item->icon_w == 0)&&(item->icon_h == 0))
      GetXPMFile(item);
  }

  /* special thanks to Rich Neitzel <thor@thor.atd.ucar.edu>
     for his patch to handle icon windows        */
  if((item->icon_h == 0)&&(item->icon_w == 0)&&
     (item->wmhints) && (item->wmhints->flags & IconWindowHint))
    GetIconWindow(item);

  /* icon bitmap from the application */
  if((item->icon_h == 0)&&(item->icon_w == 0)&&
     (item->wmhints)&&(item->wmhints->flags & IconPixmapHint))
    GetIconBitmap(item);

#ifdef XPM
#ifdef SHAPE
  if (item->icon_maskPixmap != None)
    {
      XShapeCombineMask(dpy, item->icon_pixmap_w, ShapeBounding,
			hr, hr, item->icon_maskPixmap, ShapeSet);
    }
#endif
#endif

  if(item->icon_depth == -1)
    {
      temp = item->iconPixmap;
      item->iconPixmap =
	XCreatePixmap(dpy, Root, item->icon_w,
		      item->icon_h,d_depth);
      XCopyPlane(dpy,temp,item->iconPixmap,NormalGC,
		 0,0,item->icon_w,item->icon_h,0,0,1);
    }
}

void AdjustIconWindow(struct icon_info *item, int n)
{
  int x=0,y=0,w,h,h2,h3,w3;

  w3 = w = max_icon_width + icon_relief;
  h3 = h2 = max_icon_height + icon_relief;
  h = h2 + 6 + font->ascent + font->descent;

  switch (primary){
  case LEFT:
  case RIGHT:
    if (secondary == BOTTOM)
      y = icon_win_height  - (n / Lines + 1)*(h + interval);
    else if (secondary == TOP)
      y = (n / Lines)*(h + interval) + interval;

    if (primary == LEFT)
      x = (n % Lines)*(w + interval) + interval;
    else
      x = icon_win_width - (n % Lines + 1)*(w + interval);
    break;
  case TOP:
  case BOTTOM:
    if (secondary == RIGHT)
      x = icon_win_width - (n / Lines + 1)*(w + interval);
    else if (secondary == LEFT)
      x = (n / Lines)*(w + interval) + interval;

    if (primary == TOP)
      y = (n % Lines)*(h + interval) + interval;
    else
      y = icon_win_height - (n % Lines + 1)*(h + interval);
    break;
  default:
    break;
  }

  item->x = x;
  item->y = y;

  if (item->icon_w > 0 && item->icon_h > 0){
    w3 = min(max_icon_width, item->icon_w) + icon_relief;
    h3 = min(max_icon_height, item->icon_h) + icon_relief;
  }
  if (max_icon_height != 0)
  XMoveResizeWindow(dpy, item->icon_pixmap_w, x + (w - w3)/2,
		    y + (h2 - h3)/2,w3,h3);
  XMoveResizeWindow(dpy, item->IconWin, x,y + h2,w,h - h2);
}

/***************************************************************************
 *
 * Looks for a monochrome icon bitmap file
 *
 **************************************************************************/
void GetBitmapFile(struct icon_info *item)
{
  char *path = NULL;
  int HotX,HotY;

  path = findIconFile(item->icon_file, iconPath,R_OK);
  if(path == NULL)return;

  if(XReadBitmapFile (dpy, Root,path,(unsigned int *)&item->icon_w,
		      (unsigned int *)&item->icon_h,
		      &item->iconPixmap,
		      (int *)&HotX,
		      (int *)&HotY) != BitmapSuccess)
    {
      item->icon_w = 0;
      item->icon_h = 0;
    }
  else
      item->icon_depth = 1;

  item->icon_w = min(max_icon_width, item->icon_w);
  item->icon_h = min(max_icon_height, item->icon_h);
  item->icon_maskPixmap = None;
  free(path);
}


/****************************************************************************
 *
 * Looks for a color XPM icon file
 *
 ****************************************************************************/
void GetXPMFile(struct icon_info *item)
{
#ifdef XPM
  XWindowAttributes root_attr;
  XpmAttributes xpm_attributes;
  char *path = NULL;

  path = findIconFile(item->icon_file, pixmapPath,R_OK);
  if(path == NULL)return;

  XGetWindowAttributes(dpy,Root,&root_attr);
  xpm_attributes.colormap = root_attr.colormap;
  xpm_attributes.closeness = 20000;    /* tight? */
  xpm_attributes.valuemask = XpmSize|XpmReturnPixels|XpmColormap|XpmCloseness;
  if(XpmReadFileToPixmap(dpy, Root, path,
			 &item->iconPixmap,
			 &item->icon_maskPixmap,
			 &xpm_attributes) == XpmSuccess)
    {
      item->icon_w = min((unsigned)max_icon_width, xpm_attributes.width);
      item->icon_h = min((unsigned)max_icon_height, xpm_attributes.height);
      item->icon_depth = d_depth;
    }
  free(path);
#endif /* XPM */
}

/***************************************************************************
*
 *
 * Looks for an application supplied icon window
 *
 ***************************************************************************
*/
void GetIconWindow(struct icon_info *item)
{
  int x, y;
  unsigned int bw;
  Window Junkroot;

  if(!XGetGeometry(dpy, item->wmhints->icon_window, &Junkroot,
                  &x, &y, (unsigned int *)&item->icon_w,
		   (unsigned int *)&item->icon_h,
		   &bw, (unsigned int *)&item->icon_depth))
    return;

  XDestroyWindow(dpy, item->icon_pixmap_w);
  item->icon_pixmap_w = item->wmhints->icon_window;

#ifdef SHAPE
  if (item->wmhints->flags & IconMaskHint)
    {
      item->flags |= SHAPED_ICON;
      item->icon_maskPixmap = item->wmhints->icon_mask;
    }
#endif

  item->icon_w = min(max_icon_width + icon_relief,  item->icon_w);
  item->icon_h = min(max_icon_height + icon_relief, item->icon_h);

  XReparentWindow(dpy, item->icon_pixmap_w, icon_win, 0, 0);
  XSetWindowBorderWidth(dpy, item->icon_pixmap_w, 0);
  item->flags &= ~ICON_OURS;
}

/***************************************************************************
*
 *
 * Looks for an application supplied bitmap or pixmap
 *
 ***************************************************************************
*/
void GetIconBitmap(struct icon_info *item)
{
  int x, y;
  unsigned int bw, depth;
  Window Junkroot;
  GC gc;

  if (!XGetGeometry(dpy, item->wmhints->icon_pixmap, &Junkroot, &x, &y,
               (unsigned int *)&item->icon_w,
               (unsigned int *)&item->icon_h, &bw, &depth))
    return;

  item->icon_depth = depth;
  item->icon_file = NULL;
  item->icon_maskPixmap = None;
#ifdef SHAPE
  if (item->wmhints->flags & IconMaskHint)
    {
      item->flags |= SHAPED_ICON;
      item->icon_maskPixmap = item->wmhints->icon_mask;
    }
#endif

  item->icon_w = min(max_icon_width, item->icon_w);
  item->icon_h = min(max_icon_height, item->icon_h);

  item->iconPixmap = XCreatePixmap(dpy, Root, item->icon_w,
                                   item->icon_h, depth);
  gc = XCreateGC(dpy, item->iconPixmap, 0, NULL);
  XCopyArea(dpy, item->wmhints->icon_pixmap, item->iconPixmap,
               gc, 0, 0, item->icon_w, item->icon_h, 0, 0);
  XFreeGC(dpy, gc);
}

Bool GetBackPixmap(void)
{
  XWindowAttributes root_attr;
#ifdef XPM
  XpmAttributes xpm_attributes;
#endif
  char *path = NULL;
  Pixmap tmp_bitmap, maskPixmap;
  int x, y, w=0, h=0;

  if (IconwinPixmapFile == NULL)
    return False;

  if ((path = findIconFile(IconwinPixmapFile, iconPath,R_OK)) != NULL){
    if (XReadBitmapFile(dpy, Root,path,(unsigned int *)&w,
			(unsigned int *)&h, &tmp_bitmap,
			(int *)&x, (int *)&y)!= BitmapSuccess)
      w = h = 0;
    else{
      IconwinPixmap = XCreatePixmap(dpy, Root, w, h, d_depth);
      XCopyPlane(dpy, tmp_bitmap, IconwinPixmap, NormalGC, 0, 0, w, h,
		 0, 0, 1);
      XFreePixmap(dpy, tmp_bitmap);
    }
    free(path);
  }

#ifdef XPM
  if ( w == 0 && h == 0 && (path = findIconFile(IconwinPixmapFile,
						pixmapPath,R_OK)) != NULL)
    {
      XGetWindowAttributes(dpy,Root,&root_attr);
      xpm_attributes.colormap = root_attr.colormap;
      xpm_attributes.valuemask = XpmSize|XpmReturnPixels|XpmColormap;
      if (XpmReadFileToPixmap(dpy, Root, path, &IconwinPixmap,
			      &maskPixmap, &xpm_attributes) ==
	  XpmSuccess){
	w = xpm_attributes.width;
	h = xpm_attributes.height;
      }
      free(path);
    }
#endif
  if (w != 0 && h != 0)
    return True;
  return False;
}
