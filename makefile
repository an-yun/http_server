# 定义编译环境
#
ROOT=.#$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
MKFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
CXX=clang++
LINK.cxx=$(CXX) $(CXXFLAGS) $(LDFLAGS)
LDFLAGS=-lpthread
LDDIR=-L$(ROOT)/lib
LDLIBS=$(LDDIR) #-l xxxlib
BUILDDIR=$(ROOT)/build
BINDIR=$(ROOT)/bin
SRCDIR=$(ROOT)/src
TESTDIR=$(ROOT)/test
INLCUDEDIR=$(ROOT)/include
TESTFLAGS=-DTEST
DEBUGFLAG=-std=c++17 -I$(INLCUDEDIR) -Wall -g $(TESTFLAGS)
RELEASEFLAG-std=c++17 -I$(INLCUDEDIR) -O2
CXXFLAGS=$(DEBUGFLAG)
#-stdlib=libc++ -lpthread -O2

CURRENT_VERSION=single_http_server

SRCS=ioutils.cpp fd_transfer.cpp request.cpp response.cpp connection.cpp worker.cpp server.cpp
TEST=epoll_example.cpp epoll_server.cpp single_http_server.cpp test_src.cpp pip_dup.cpp client_test.cpp fifo_rw.cpp

SRCS_FILES=$(addprefix $(SRCDIR)/, $(SRCS))
TEST_FILES=$(addprefix $(TESTDIR)/, $(TEST))

SRCS_TARGET=$(SRCS_FILES:.cpp=.o)
TEST_TARGET=$(TEST_FILES:.cpp=.bin)


all:$(SRCS_TARGET) $(TESTDIR)/$(CURRENT_VERSION).bin $(BINDIR)
	cp -f $(TESTDIR)/$(CURRENT_VERSION).bin $(BINDIR)/http_server


run:all
	$(BINDIR)/http_server web 8081

test:$(TEST_TARGET)
	@echo makefile in $(MKFILE_PATH)

$(SRCDIR)/ioutils.o:$(SRCDIR)/ioutils.cpp $(INLCUDEDIR)/ioutils.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/ioutils.cpp -o $@

$(SRCDIR)/fd_transfer.o:$(SRCDIR)/fd_transfer.cpp $(INLCUDEDIR)/fd_transfer.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/fd_transfer.cpp -o $@

$(SRCDIR)/request.o:$(SRCDIR)/request.cpp $(INLCUDEDIR)/request.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/request.cpp -o $@

$(SRCDIR)/response.o:$(SRCDIR)/response.cpp $(INLCUDEDIR)/response.h $(INLCUDEDIR)/ioutils.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/response.cpp -o $@

$(SRCDIR)/connection.o:$(SRCDIR)/connection.cpp $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h $(INLCUDEDIR)/response.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/connection.cpp -o $@

$(SRCDIR)/worker.o:$(SRCDIR)/worker.cpp $(INLCUDEDIR)/worker.h $(INLCUDEDIR)/connection.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/worker.cpp -o $@

$(SRCDIR)/server.o:$(SRCDIR)/server.cpp $(INLCUDEDIR)/server.h $(INLCUDEDIR)/connection.h $(INLCUDEDIR)/request.h $(INLCUDEDIR)/ioutils.h $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/server.cpp -o $@



$(TESTDIR)/epoll_example.bin:$(TESTDIR)/epoll_example.cpp $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(TESTDIR)/epoll_example.cpp -o $@

$(TESTDIR)/client_test.bin:$(TESTDIR)/client_test.cpp $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(TESTDIR)/client_test.cpp -o $@

$(TESTDIR)/pip_dup.bin:$(TESTDIR)/pip_dup.cpp $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(TESTDIR)/pip_dup.cpp -o $@

$(TESTDIR)/epoll_server.bin:$(TESTDIR)/epoll_server.cpp $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(TESTDIR)/epoll_server.cpp -o $@

$(TESTDIR)/fifo_rw.bin:$(TESTDIR)/fifo_rw.cpp $(SRCDIR)/fd_transfer.o $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(TESTDIR)/fifo_rw.cpp $(SRCDIR)/fd_transfer.o -o $@

$(TESTDIR)/test_src.bin:$(TESTDIR)/test_src.cpp $(SRCS_TARGET) $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TESTDIR)/test_src.cpp $(SRCS_TARGET) -o $@	

$(TESTDIR)/single_http_server.bin:$(TESTDIR)/single_http_server.cpp $(SRCS_TARGET) $(MKFILE_PATH)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(SRCS_TARGET)  -o $@

$(BINDIR):
	mkdir $(BINDIR)

clean:
	rm $(SRCDIR)/*.o -rf
	rm $(TEST_TARGET) -rf
	rm $(BINDIR)/http_server -rf
