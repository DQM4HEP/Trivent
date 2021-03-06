#######################################################
# cmake file for building Trivent package
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

AUX_SOURCE_DIRECTORY( src SRC_FILES )

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} )

INSTALL(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

# -------------------------------------------------
# install headers
INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )

FIND_PACKAGE( LCIO )

IF( LCIO_FOUND )
	
	INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} )
	LINK_LIBRARIES( ${LCIO_LIBRARIES} )
	ADD_DEFINITIONS ( ${LCIO_DEFINITIONS} )

	ADD_SUBDIRECTORY( lcio )
	
ENDIF()
