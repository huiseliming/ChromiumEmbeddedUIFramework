
#ifndef CEUIF_API_H
#define CEUIF_API_H

#ifdef CEUIF_STATIC_DEFINE
#  define CEUIF_API
#  define CEUIF_NO_EXPORT
#else
#  ifndef CEUIF_API
#    ifdef CeUIf_EXPORTS
        /* We are building this library */
#      define CEUIF_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define CEUIF_API __declspec(dllimport)
#    endif
#  endif

#  ifndef CEUIF_NO_EXPORT
#    define CEUIF_NO_EXPORT 
#  endif
#endif

#ifndef CEUIF_DEPRECATED
#  define CEUIF_DEPRECATED __declspec(deprecated)
#endif

#ifndef CEUIF_DEPRECATED_EXPORT
#  define CEUIF_DEPRECATED_EXPORT CEUIF_API CEUIF_DEPRECATED
#endif

#ifndef CEUIF_DEPRECATED_NO_EXPORT
#  define CEUIF_DEPRECATED_NO_EXPORT CEUIF_NO_EXPORT CEUIF_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CEUIF_NO_DEPRECATED
#    define CEUIF_NO_DEPRECATED
#  endif
#endif

#endif /* CEUIF_API_H */
