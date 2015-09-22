TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE+= -O2

QMAKE_CXXFLAGS+=-std=c++0x
QMAKE_LFLAGS+=-std=c++0x
QMAKE_CXXFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_CXXFLAGS_X86_64+= -mmacosx-version-min=10.7
QMAKE_LFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_LFLAGS_X86_64-= -mmacosx-version-min=10.5
QMAKE_LFLAGS_X86_64+= -mmacosx-version-min=10.7


# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

SOURCES += \
    cgraphdemo.cpp \
    cgraphwatcher.cpp \

HEADERS += \
    OrderedQueue.h \
    Image.h \
    FlatSE.h \
    Types.h \
    Point.h \
    ImageIterators.h \
    ImageIO.hxx \
    Image.hxx \
    cgraph.h \
    ragraph.h \
    cgraphwatcher.h \
    FlatSE.hxx \
    graphnode.h \
    cgraph.hxx \
    ragraph.hxx \
    ordering.h
