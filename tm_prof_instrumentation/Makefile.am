lib_LTLIBRARIES = libtm_prof_instumentation.la
libtm_prof_instumentation_la_SOURCES = plugin.cpp

plugin_path = $(shell g++ -print-file-name=plugin)

libtm_prof_instumentation_la_CXXFLAGS = -Wall -fPIC -shared -fno-rtti -I$(plugin_path)/include
