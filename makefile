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

SRCS=ioutils.cpp request.cpp connection.cpp server.cpp
TEST=epoll_example.cpp epoll_server.cpp http_server.cpp

SRCS_FILES=$(addprefix $(SRCDIR)/, $(SRCS))
TEST_FILES=$(addprefix $(TESTDIR)/, $(TEST))

SRCS_TARGET=$(SRCS_FILES:.cpp=.o)
TEST_TARGET=$(TEST_FILES:.cpp=.bin)


all:$(SRCS_TARGET) $(TEST_TARGET)
	cp -f $(TESTDIR)/http_server.bin $(BINDIR)/http_server

show:
	@echo $(SRCS_TARGET) $(TEST_TARGET)

$(SRCDIR)/ioutils.o:$(SRCDIR)/ioutils.cpp $(INLCUDEDIR)/ioutils.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/ioutils.cpp -o $@

$(SRCDIR)/request.o:$(SRCDIR)/request.cpp $(INLCUDEDIR)/request.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/request.cpp -o $@

$(SRCDIR)/connection.o:$(SRCDIR)/connection.cpp $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/connection.cpp -o $@

$(SRCDIR)/server.o:$(SRCDIR)/server.cpp $(INLCUDEDIR)/server.h $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h $(INLCUDEDIR)/ioutils.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/server.cpp -o $@

$(TEST_TARGET):$(TESTDIR)/%.bin:$(TESTDIR)/%.cpp $(SRCS_TARGET)
	$(CXX) $(CXXFLAGS) $< $(SRCS_TARGET)  -o $@


clean:
	rm $(SRCDIR)/*.o -rf
	rm $(TEST_TARGET) -rf
	rm $(BINDIR)/http_server -rf
