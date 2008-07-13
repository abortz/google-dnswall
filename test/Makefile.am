check_PROGRAMS = run_unittests
TESTS = run_unittests

INCLUDES = -I$(top_srcdir)/third_party/gtest/include

run_unittests_SOURCES = gtest_main.cc \
                        $(top_srcdir)/src/query_record.c \
                        $(top_srcdir)/src/query_record_unittest.cc \
                        $(top_srcdir)/src/check_record.c \
                        $(top_srcdir)/src/check_record_unittest.cc
                        
run_unittests_LDADD = $(top_builddir)/third_party/gtest/lib/libgtest.la

AM_CFLAGS = --std=c99 -Wall
