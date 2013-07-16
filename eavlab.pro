CONFIG += debug

QT       += core gui opengl

TARGET = eavlab
TEMPLATE = app

QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
QMAKE_CXXFLAGS_X86_64 += -mmacosx-version-min=10.7

SOURCES += main.cpp\
    ELAttributeControl.cpp \
    ELMainWindow.cpp \
    ELWindowManager.cpp \
    ELEmptyWindow.cpp \
    ELWindowFrame.cpp \
    EL1DWindow.cpp \
    EL2DWindow.cpp \
    EL3DWindow.cpp \
    ELPolarWindow.cpp \
    ELBasicInfoWindow.cpp \
    ELPipelineBuilder.cpp \
    ELSources.cpp \
    Attribute.cpp \
    Pipeline.cpp \
    XMLTools.cpp

EAVLROOT = /home/js9/eavl/2013-07-11_work/EAVL
#EAVLROOT = $$(EAVL)
isEmpty(EAVLROOT) {
  warning("Expected an EAVL environment varible to be set that points")
  warning("to a configured/built EAVL checkout.  One does not exist.")
  warning("Instead, assuming that EAVL was a peer checkout to EAVLab.")
  warning("I.e., assuming the EAVLROOT variable was set to ../EAVL/.")
  EAVLROOT="../EAVL"
}


## We're using a wildcard to glob for EAVL header
## files because it won't check them for
## dependencies otherwise.
HEADERS  += $$files(*.h) \
    $$files($$EAVLROOT/src/*/*.h)

FORMS    +=

DEPENDPATH += $$EAVLROOT/config $$EAVLROOT/src/common $$EAVLROOT/src/fonts $$EAVLROOT/src/importers $$EAVLROOT/src/filters $$EAVLROOT/src/exporters $$EAVLROOT/src/math $$EAVLROOT/src/rendering
INCLUDEPATH += $$EAVLROOT/config $$EAVLROOT/src/common $$EAVLROOT/src/fonts $$EAVLROOT/src/importers $$EAVLROOT/src/filters $$EAVLROOT/src/exporters $$EAVLROOT/src/math $$EAVLROOT/src/rendering

win32 {
  LIBS += -L$$EAVLROOT/Debug/lib -L$$EAVLROOT/../eavl-build-desktop/debug/lib -leavl
  #POST_TARGETDEPS += $$EAVLROOT/Debug/lib/libeavl.a
}
unix {
  LIBS += -L$$EAVLROOT/lib -leavl
  POST_TARGETDEPS += $$EAVLROOT/lib/libeavl.a
}

!include($$EAVLROOT/config/make-dependencies)
{
  INCLUDEPATH += $$EAVLROOT/config-simple
}

HOST = $$system(hostname)
SYS = $$system(uname -s)

!equals(BOOST, no) {
  INCLUDEPATH += $$BOOST/include
  LIBS += $$BOOST_LDFLAGS $$BOOST_LIBS
}

!equals(MPI, no) {
  QMAKE_CXXFLAGS += $$MPI_CPPFLAGS
  LIBS += $$MPI_LDFLAGS $$MPI_LIBS
}

!equals(NETCDF, no) {
  INCLUDEPATH += $$NETCDF/include
  LIBS += $$NETCDF_LDFLAGS $$NETCDF_LIBS
}

!equals(HDF5, no) {
  INCLUDEPATH += $$HDF5/include
  LIBS += $$HDF5_LDFLAGS $$HDF5_LIBS
}

!equals(CUDA, no) {
  INCLUDEPATH += $$CUDA/include
  LIBS += $$CUDA_LDFLAGS $$CUDA_LIBS
}

!equals(SILO, no) {
  INCLUDEPATH += $$SILO/include
  LIBS += $$SILO_LDFLAGS $$SILO_LIBS
}

!equals(ADIOS, no) {
  INCLUDEPATH += $$ADIOS/include
  LIBS += $$ADIOS_LDFLAGS $$ADIOS_LIBS
}

!equals(SZIP, no) {
  INCLUDEPATH += $$SZIP/include
  LIBS += $$SZIP_LDFLAGS $$SZIP_LIBS
}

!equals(ZLIB, no) {
  INCLUDEPATH += $$ZLIB/include
  LIBS += $$ZLIB_LDFLAGS $$ZLIB_LIBS
}



##
## Check errors for lens.
## (This is as much a hint for devs who won't know
## the location of a sufficiently new Qt.)
##
hostcheck=$$find(HOST, lens)
!isEmpty(hostcheck) {
  !contains(QMAKE_QMAKE, "/sw/sources/visit/analysis-x64/thirdparty/visit/qt/4.6.1/linux-x86_64_gcc-4.4/bin/qmake") {
     message(ERROR: Please use /sw/sources/visit/analysis-x64/thirdparty/visit/qt/4.6.1/linux-x86_64_gcc-4.4/bin/qmake)
  }
}
