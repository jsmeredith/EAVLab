CONFIG += debug

QT       += core gui opengl

TARGET = eavlab
TEMPLATE = app


SOURCES += main.cpp\
    ELAttributeControl.cpp \
    ELMainWindow.cpp \
    ELWindowManager.cpp \
    ELEmptyWindow.cpp \
    ELWindowFrame.cpp \
    EL3DWindow.cpp \
    ELBasicInfoWindow.cpp \
    ELPipelineBuilder.cpp \
    ELSources.cpp \
    Attribute.cpp \
    Pipeline.cpp \
    XMLTools.cpp


EAVLROOT = $$(EAVL)
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

DEPENDPATH += $$EAVLROOT/config $$EAVLROOT/src/common $$EAVLROOT/src/importers $$EAVLROOT/src/filters $$EAVLROOT/src/exporters $$EAVLROOT/src/math $$EAVLROOT/src/rendering
INCLUDEPATH += $$EAVLROOT/config $$EAVLROOT/src/common $$EAVLROOT/src/importers $$EAVLROOT/src/filters $$EAVLROOT/src/exporters $$EAVLROOT/src/math $$EAVLROOT/src/rendering

win32 {
  INCLUDEPATH += $$EAVLROOT/config-windows
  LIBS += -L$$EAVLROOT/Debug/lib -leavl
  POST_TARGETDEPS += $$EAVLROOT/Debug/lib/libeavl.a
}
unix {
  LIBS += -L$$EAVLROOT/lib -leavl
  POST_TARGETDEPS += $$EAVLROOT/lib/libeavl.a
}

include($$EAVLROOT/config/make-dependencies)

HOST = $$system(hostname)
SYS = $$system(uname -s)

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
