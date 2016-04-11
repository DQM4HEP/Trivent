#######################################################
# cmake file for building Trivent package - lcio binding
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

# include directories
INCLUDE_DIRECTORIES( include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library

AUX_SOURCE_DIRECTORY( src LC_SRC_FILES )

ADD_SHARED_LIBRARY( LC${PROJECT_NAME} ${LC_SRC_FILES} )
TARGET_LINK_LIBRARIES( LC${PROJECT_NAME} ${PROJECT_NAME} )

INSTALL(
        TARGETS LC${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

# -------------------------------------------------
# install headers
INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )


