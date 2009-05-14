/***********************************************************************
options.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#if 0
#define EST_LIBEDL
#endif

#ifndef EST_LIBEDL
#define SANS_LIBEDL
#endif /* EST_LIBEDL */

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#define STATIC
#else
#define STATIC static
#endif /* defined AVEC_LIBEDL || defined EST_LIBEDL */

#endif /* OPTIONS_H */
