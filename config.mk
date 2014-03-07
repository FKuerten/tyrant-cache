# Configuration section
MODULE_NAME=cache
DEPENDENCIES=basics core
LIBS:=
CXXFLAGS+= --std=c++11
ARFLAGS:=crvs

# Automatic section
INCLUDEDIRS:=${MODULEDIR}/target/generated
iftrue ${DEPENDENCIES}
	INCLUDEDIRS+=${MODULEDIR}/../${DEPENDENCIES}/src/main/c++
endif
CPPFLAGS+=-I${INCLUDEDIRS}
