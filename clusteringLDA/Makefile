#############################################################################
# Makefile for building: clusteringLDA
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2
CXXFLAGS      = -pipe -O2
LINK          = g++
AR            = ar cq
COPY          = cp -f
SED           = sed
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = $(COPY_FILE)
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		news.cpp \
		clustering.cpp \
		newscluster.cpp \
		lda/utils.cpp \
		lda/strtokenizer.cpp \
		lda/model.cpp \
		lda/dataset.cpp \
		filter.cpp \
		filemanager.cpp \
		dictionary.cpp \
		newsmodel.cpp 
OBJECTS       = main.o \
		news.o \
		clustering.o \
		newscluster.o \
		utils.o \
		strtokenizer.o \
		model.o \
		dataset.o \
		filter.o \
		filemanager.o \
		dictionary.o \
		newsmodel.o
TARGET        = clusteringLDA

####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) -o "$@" "$<"

####### Build rules

all: $(TARGET) clean_obj

$(TARGET):  $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS) $(OBJCOMP)

####### Compile

main.o: main.cpp news.h \
		newscluster.h \
		filter.h \
		dictionary.h \
		filemanager.h \
		clustering.h \
		conversion.h \
		newsmodel.h \
		lda/model.h \
		lda/constants.h \
		lda/dataset.h
	$(CXX) -c $(CXXFLAGS) -o main.o main.cpp

news.o: news.cpp conversion.h \
		news.h
	$(CXX) -c $(CXXFLAGS) -o news.o news.cpp

clustering.o: clustering.cpp clustering.h \
		newscluster.h \
		news.h \
		conversion.h
	$(CXX) -c $(CXXFLAGS) -o clustering.o clustering.cpp

newscluster.o: newscluster.cpp newscluster.h \
		conversion.h
	$(CXX) -c $(CXXFLAGS) -o newscluster.o newscluster.cpp

utils.o: lda/utils.cpp lda/strtokenizer.h \
		lda/utils.h \
		lda/model.h \
		lda/constants.h \
		lda/dataset.h
	$(CXX) -c $(CXXFLAGS) -o utils.o lda/utils.cpp

strtokenizer.o: lda/strtokenizer.cpp lda/strtokenizer.h
	$(CXX) -c $(CXXFLAGS) -o strtokenizer.o lda/strtokenizer.cpp

model.o: lda/model.cpp lda/constants.h \
		lda/strtokenizer.h \
		lda/utils.h \
		lda/dataset.h \
		lda/model.h
	$(CXX) -c $(CXXFLAGS) -o model.o lda/model.cpp

dataset.o: lda/dataset.cpp lda/constants.h \
		lda/strtokenizer.h \
		lda/dataset.h
	$(CXX) -c $(CXXFLAGS) -o dataset.o lda/dataset.cpp

filter.o: filter.cpp filter.h \
		dictionary.h \
		news.h
	$(CXX) -c $(CXXFLAGS) -o filter.o filter.cpp

filemanager.o: filemanager.cpp filemanager.h \
		news.h \
		filter.h \
		dictionary.h \
		newscluster.h \
		conversion.h
	$(CXX) -c $(CXXFLAGS) -o filemanager.o filemanager.cpp

dictionary.o: dictionary.cpp dictionary.h \
		news.h
	$(CXX) -c $(CXXFLAGS) -o dictionary.o dictionary.cpp

newsmodel.o: newsmodel.cpp newsmodel.h
	$(CXX) -c $(CXXFLAGS) -o newsmodel.o newsmodel.cpp

####### PHONY
.PHONY: clean
clean: clean_all

.PHONY: clean_all
clean_all: clean_obj
		-$(DEL_FILE) $(TARGET)
        
.PHONY: clean_obj
clean_obj:
		-$(DEL_FILE) $(OBJECTS)

