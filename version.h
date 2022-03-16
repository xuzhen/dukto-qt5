#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 6
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#if 0   // Next line will be read by dukto.pro, DON'T REMOVE
VERSION=6.0.0
#endif

#define ___xstr(s) ___str(s)
#define ___str(s) #s

#if VERSION_PATCH == 0
    #define VERSION_TEXT ___xstr(VERSION_MAJOR) "." ___xstr(VERSION_MINOR)
#else
    #define VERSION_TEXT ___xstr(VERSION_MAJOR) "." ___xstr(VERSION_MINOR) "." ___xstr(VERSION_PATCH)
#endif

#endif // VERSION_H
