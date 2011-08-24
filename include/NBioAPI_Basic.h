#ifndef _NITGEN_NBioAPI_BASIC_H
#define _NITGEN_NBioAPI_BASIC_H

#ifdef WIN32
   #define NBioAPI   __stdcall
#else /* (!WIN32) */
   #define NBioAPI
#endif

#ifdef WIN32
   typedef __int8    NBioAPI_SINT8;
   typedef __int16   NBioAPI_SINT16;
   typedef int       NBioAPI_SINT32;

   typedef BYTE      NBioAPI_UINT8;
   typedef WORD      NBioAPI_UINT16;
   typedef DWORD     NBioAPI_UINT32;
   typedef __int64   NBioAPI_UINT64;

   typedef INT_PTR   NBioAPI_SINT;  
   typedef UINT_PTR  NBioAPI_UINT;  

   typedef void*     NBioAPI_VOID_PTR;
   typedef BOOL      NBioAPI_BOOL;

   typedef CHAR      NBioAPI_CHAR;
   typedef LPSTR     NBioAPI_CHAR_PTR;

   #define NBioAPI_FALSE   (0)
   #define NBioAPI_TRUE    (!NBioAPI_FALSE)
   
   #define NBioAPI_NULL    (NULL)

   typedef HWND            NBioAPI_HWND;
#else
   typedef char      NBioAPI_SINT8;
   typedef short     NBioAPI_SINT16;
   typedef int       NBioAPI_SINT32;

   typedef unsigned char      NBioAPI_UINT8;
   typedef unsigned short     NBioAPI_UINT16;
   typedef unsigned int       NBioAPI_UINT32;
   typedef unsigned long int  NBioAPI_UINT64;

   typedef int                NBioAPI_SINT;
   typedef unsigned int       NBioAPI_UINT;

   typedef void*     NBioAPI_VOID_PTR;
   typedef int       NBioAPI_BOOL;

   typedef char      NBioAPI_CHAR;
   typedef char*     NBioAPI_CHAR_PTR;

   #define NBioAPI_FALSE   (0)
   #define NBioAPI_TRUE    (!NBioAPI_FALSE)
   
   #define NBioAPI_NULL    (0)
   #define MAX_PATH        (256)

   #define WINAPI
   typedef const NBioAPI_CHAR_PTR   LPCTSTR;
   typedef NBioAPI_CHAR_PTR         LPSTR;
   typedef NBioAPI_CHAR             CHAR;
   typedef NBioAPI_VOID_PTR         HINSTANCE;
   typedef NBioAPI_UINT32           NBioAPI_HWND;

   #define IDOK                1
   #define IDCANCEL            2
#endif/* WIN32 */

/* Macros for byte-order conversion */
#ifndef LITTLE_ENDIAN   
   #ifndef _UNIX
      #define LITTLE_ENDIAN
   #endif
#endif
   
#ifdef LITTLE_ENDIAN
#define EndianChange4(a)	(a)
#define EndianChange2(a)	(a)
#else
#define EndianChange4(val)\
	(\
      (((val) & 0xff000000) >> 24) | (((val) & 0x00ff0000) >> 8) |   \
	   (((val) & 0x0000ff00) << 8) | (((val) & 0x000000ff) << 24)     \
   )
#define EndianChange2(val)\
   ((((val) & 0xff00) >> 8) | (((val) & 0x00ff) << 8))
#endif

#endif/*_NITGEN_NBioAPI_BASIC_H */
