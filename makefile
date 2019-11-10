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
CXXFLAGS=-std=c++17 -I$(INLCUDEDIR) -Wall -g 

CURRENT_VERSION=single_http_server

SRCS=ioutils.cpp fd_transfer.cpp request.cpp connection.cpp worker.cpp server.cpp
TEST=epoll_example.cpp epoll_server.cpp single_http_server.cpp test_src.cpp pip_dup.cpp client_test.cpp fifo_rw.cpp

SRCS_FILES=$(addprefix $(SRCDIR)/, $(SRCS))
TEST_FILES=$(addprefix $(TESTDIR)/, $(TEST))

SRCS_TARGET=$(SRCS_FILES:.cpp=.o)
TEST_TARGET=$(TEST_FILES:.cpp=.bin)


all:$(SRCS_TARGET) $(TEST_TARGET)

install:$(BINDIR) all
	cp -f $(TESTDIR)/$(CURRENT_VERSION).bin $(BINDIR)/http_server

run:install
	$(BINDIR)/http_server web 8080


test:all
	$(TESTDIR)/test_src.bin

$(SRCDIR)/ioutils.o:$(SRCDIR)/ioutils.cpp $(INLCUDEDIR)/ioutils.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/ioutils.cpp -o $@

$(SRCDIR)/fd_transfer.o:$(SRCDIR)/fd_transfer.cpp $(INLCUDEDIR)/fd_transfer.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/fd_transfer.cpp -o $@

$(SRCDIR)/request.o:$(SRCDIR)/request.cpp $(INLCUDEDIR)/request.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/request.cpp -o $@

$(SRCDIR)/connection.o:$(SRCDIR)/connection.cpp $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/connection.cpp -o $@

$(SRCDIR)/worker.o:$(SRCDIR)/worker.cpp $(INLCUDEDIR)/worker.h $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/worker.cpp -o $@

$(SRCDIR)/server.o:$(SRCDIR)/server.cpp $(INLCUDEDIR)/server.h $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h $(INLCUDEDIR)/ioutils.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/server.cpp -o $@

$(TEST_TARGET):$(TESTDIR)/%.bin:$(TESTDIR)/%.cpp $(SRCS_TARGET)
	$(CXX) $(CXXFLAGS) $< $(SRCS_TARGET)  -o $@

$(BINDIR):
	mkdir $(BINDIR)

clean:
	rm $(SRCDIR)/*.o -rf
	rm $(TEST_TARGET) -rf
	rm $(BINDIR)/http_server -rf
