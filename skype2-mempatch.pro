TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += warn_on

SOURCES += skype2-mempatch.c

# scanmem-0.13
SOURCES += \
    scanmem-0.13/commands.c \
    scanmem-0.13/handlers.c \
    scanmem-0.13/list.c \
    scanmem-0.13/main.c \
    scanmem-0.13/maps.c \
    scanmem-0.13/menu.c \
    scanmem-0.13/ptrace.c \
    scanmem-0.13/scanroutines.c \
    scanmem-0.13/show_message.c \
    scanmem-0.13/target_memory_info_array.c \
    scanmem-0.13/value.c

HEADERS += \
    config.h \
    scanmem-0.13/commands.h \
    scanmem-0.13/handlers.h \
    scanmem-0.13/interrupt.h \
    scanmem-0.13/licence.h \
    scanmem-0.13/list.h \
    scanmem-0.13/maps.h \
    scanmem-0.13/scanmem.h \
    scanmem-0.13/scanroutines.h \
    scanmem-0.13/show_message.h \
    scanmem-0.13/target_memory_info_array.h \
    scanmem-0.13/value.h

INCLUDEPATH += .
LIBS += -lreadline

