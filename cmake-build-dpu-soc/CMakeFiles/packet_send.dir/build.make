# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/sunxi/packet_send

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc

# Include any dependencies generated for this target.
include CMakeFiles/packet_send.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/packet_send.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/packet_send.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packet_send.dir/flags.make

CMakeFiles/packet_send.dir/main_pkt_send.cpp.o: CMakeFiles/packet_send.dir/flags.make
CMakeFiles/packet_send.dir/main_pkt_send.cpp.o: ../main_pkt_send.cpp
CMakeFiles/packet_send.dir/main_pkt_send.cpp.o: CMakeFiles/packet_send.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/packet_send.dir/main_pkt_send.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/packet_send.dir/main_pkt_send.cpp.o -MF CMakeFiles/packet_send.dir/main_pkt_send.cpp.o.d -o CMakeFiles/packet_send.dir/main_pkt_send.cpp.o -c /home/ubuntu/sunxi/packet_send/main_pkt_send.cpp

CMakeFiles/packet_send.dir/main_pkt_send.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packet_send.dir/main_pkt_send.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/sunxi/packet_send/main_pkt_send.cpp > CMakeFiles/packet_send.dir/main_pkt_send.cpp.i

CMakeFiles/packet_send.dir/main_pkt_send.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packet_send.dir/main_pkt_send.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/sunxi/packet_send/main_pkt_send.cpp -o CMakeFiles/packet_send.dir/main_pkt_send.cpp.s

# Object files for target packet_send
packet_send_OBJECTS = \
"CMakeFiles/packet_send.dir/main_pkt_send.cpp.o"

# External object files for target packet_send
packet_send_EXTERNAL_OBJECTS =

packet_send: CMakeFiles/packet_send.dir/main_pkt_send.cpp.o
packet_send: CMakeFiles/packet_send.dir/build.make
packet_send: CMakeFiles/packet_send.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable packet_send"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packet_send.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packet_send.dir/build: packet_send
.PHONY : CMakeFiles/packet_send.dir/build

CMakeFiles/packet_send.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packet_send.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packet_send.dir/clean

CMakeFiles/packet_send.dir/depend:
	cd /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/sunxi/packet_send /home/ubuntu/sunxi/packet_send /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc/CMakeFiles/packet_send.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/packet_send.dir/depend

