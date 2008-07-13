check_PROGRAMS = run_unittests
TESTS = run_unittests

INCLUDES = -I$(top_srcdir)/third_party/gtest/include

run_unittests_SOURCES = gtest_main.cc
run_unittests_LDADD = $(top_builddir)/third_party/gtest/lib/libgtest.la

AM_CFLAGS = -Wall
