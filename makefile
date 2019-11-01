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
TESTDIR=$(ROOT)/test
INLCUDEDIR=$(ROOT)/include
CXXFLAGS=-std=c++17 -I$(INLCUDEDIR) -Wall -g -D_GNU_SOURCE 

#源代码
SRCSFILES=$(notdir $(wildcard $(SRCDIR)/*.cpp))
TESTFILES=$(notdir $(wildcard $(TESTDIR)/*.cpp))
SRCS=$(addprefix $(SRCDIR)/,$(SRCSFILES))
TEST=$(addprefix $(TESTDIR)/,$(TESTFILES))
OBJS=$(addprefix $(BUILDDIR)/,$(SRCSFILES:.cpp=.o))

$(BINDIR)/http_server:$(BUILDDIR) $(BINDIR) $(OBJS)
	$(LINK.cxx) $(OBJS) -o $(BUILDDIR)/http_server
	cp $(BUILDDIR)/http_server $(BINDIR)/http_server -f

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(BINDIR):
	mkdir $(BINDIR)
		
run:$(BINDIR)/http_server
	@echo ================run================
	@$(BINDIR)/http_server web 8080

$(OBJS):$(BUILDDIR)/%.o:$(SRCDIR)/%.cpp $(INLCUDEDIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm build/* -rf
	rm bin/* -rf
