#ifndef TriventConfig_H
#define TriventConfig_H

// version macros
#define Trivent_MAJOR_VERSION 1
#define Trivent_MINOR_VERSION 0
#define Trivent_PATCH_LEVEL 0

#define Trivent_VERSION_STR "1.0.0"
// for backward compatibility
#define DQM4HEP_VERSION_STR "1.0.0"

#define Trivent_VERSION_GE( MAJV , MINV , PLEV )  \
     ( (Trivent_MAJOR_VERSION > MAJV) || ( (Trivent_MAJOR_VERSION == MAJV) && (Trivent_MINOR_VERSION > MINV) ) \
  || ( (Trivent_MAJOR_VERSION == MAJV) && (Trivent_MINOR_VERSION == MINV) && (Trivent_PATCH_LEVEL >= PLEV) ) )

// project source dir
// useful for icons
#define Trivent_DIR "/home/rete/soft/Trivent"
// for backward compatibility
#define DQM4HEP_DIR "/home/rete/soft/Trivent"


#ifdef DQM4HEP_USE_ELOG
#define DQM4HEP_ELOG_EXECUTABLE ""
#else
#define DQM4HEP_ELOG_EXECUTABLE ""
#endif


#endif // TriventConfig_H
