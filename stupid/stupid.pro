SUBDIRS += posix
SUBDIRS += src/pesim
SUBDIRS += src/pesim/app
SUBDIRS += src

TEMPLATE = subdirs
CONFIG += warn_on \
          qt \
          thread 
