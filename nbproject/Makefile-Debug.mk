#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ext_library/Crypto/Crypto.o \
	${OBJECTDIR}/ext_library/Cryptosuite/sha1.o \
	${OBJECTDIR}/ext_library/Print/Print.o \
	${OBJECTDIR}/ext_library/base64_lib/base64_lib.o \
	${OBJECTDIR}/ext_library/libb64/cdecode.o \
	${OBJECTDIR}/ext_library/libb64/cencode.o \
	${OBJECTDIR}/lsp.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libLWSPLibrary.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libLWSPLibrary.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libLWSPLibrary.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/ext_library/Crypto/Crypto.o: ext_library/Crypto/Crypto.cpp 
	${MKDIR} -p ${OBJECTDIR}/ext_library/Crypto
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iext_library/ArduinoJson -Iext_library/base64_arduino -Iext_library/base64_lib -Iext_library/Crypto -Iext_library/Cryptosuite -Iext_library/libb64 -Iext_library/Print -Iext_library/Printable -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/Crypto/Crypto.o ext_library/Crypto/Crypto.cpp

${OBJECTDIR}/ext_library/Cryptosuite/sha1.o: ext_library/Cryptosuite/sha1.cpp 
	${MKDIR} -p ${OBJECTDIR}/ext_library/Cryptosuite
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iext_library/ArduinoJson -Iext_library/base64_arduino -Iext_library/base64_lib -Iext_library/Crypto -Iext_library/Cryptosuite -Iext_library/libb64 -Iext_library/Print -Iext_library/Printable -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/Cryptosuite/sha1.o ext_library/Cryptosuite/sha1.cpp

${OBJECTDIR}/ext_library/Print/Print.o: ext_library/Print/Print.cpp 
	${MKDIR} -p ${OBJECTDIR}/ext_library/Print
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iext_library/ArduinoJson -Iext_library/base64_arduino -Iext_library/base64_lib -Iext_library/Crypto -Iext_library/Cryptosuite -Iext_library/libb64 -Iext_library/Print -Iext_library/Printable -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/Print/Print.o ext_library/Print/Print.cpp

${OBJECTDIR}/ext_library/base64_lib/base64_lib.o: ext_library/base64_lib/base64_lib.cpp 
	${MKDIR} -p ${OBJECTDIR}/ext_library/base64_lib
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iext_library/ArduinoJson -Iext_library/base64_arduino -Iext_library/base64_lib -Iext_library/Crypto -Iext_library/Cryptosuite -Iext_library/libb64 -Iext_library/Print -Iext_library/Printable -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/base64_lib/base64_lib.o ext_library/base64_lib/base64_lib.cpp

${OBJECTDIR}/ext_library/libb64/cdecode.o: ext_library/libb64/cdecode.c 
	${MKDIR} -p ${OBJECTDIR}/ext_library/libb64
	${RM} "$@.d"
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/libb64/cdecode.o ext_library/libb64/cdecode.c

${OBJECTDIR}/ext_library/libb64/cencode.o: ext_library/libb64/cencode.c 
	${MKDIR} -p ${OBJECTDIR}/ext_library/libb64
	${RM} "$@.d"
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ext_library/libb64/cencode.o ext_library/libb64/cencode.c

${OBJECTDIR}/lsp.o: lsp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iext_library/ArduinoJson -Iext_library/base64_arduino -Iext_library/base64_lib -Iext_library/Crypto -Iext_library/Cryptosuite -Iext_library/libb64 -Iext_library/Print -Iext_library/Printable -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lsp.o lsp.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libLWSPLibrary.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
