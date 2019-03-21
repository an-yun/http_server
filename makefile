# 定义编译环境
#
ROOT=.#$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CXX=clang++
LINK.cxx=$(CXX) $(CXXFLAGS) $(LDFLAGS)
LDFLAGS=
LDDIR=-L$(ROOT)/lib
LDLIBS=$(LDDIR) #-l xxxlib
BUILDDIR=$(ROOT)/build
BINDIR=$(ROOT)/bin
SRCDIR=$(ROOT)/src
CXXFLAGS=-std=c++17 -I$(ROOT)/include -Wall

#源代码
SRCSFILES=$(notdir $(wildcard $(SRCDIR)/*.cpp))
SRCS=$(addprefix $(SRCDIR)/,$(SRCSFILES))
OBJS=$(addprefix $(BUILDDIR)/,$(SRCSFILES:.cpp=.o))

$(BINDIR)/server:$(OBJS)
	$(LINK.cxx) $(OBJS) -o $(BUILDDIR)/server
	cp $(BUILDDIR)/server $(BINDIR)/server
		
run:$(BINDIR)/server
	@echo ================run================
	@$(BINDIR)/server

$(OBJS):$(BUILDDIR)/%.o:$(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
