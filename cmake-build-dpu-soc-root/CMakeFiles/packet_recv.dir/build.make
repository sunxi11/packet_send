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
CMAKE_BINARY_DIR = /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root

# Include any dependencies generated for this target.
include CMakeFiles/packet_recv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/packet_recv.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/packet_recv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packet_recv.dir/flags.make

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o: CMakeFiles/packet_recv.dir/flags.make
CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o: ../main_pkt_recv.cpp
CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o: CMakeFiles/packet_recv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o -MF CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o.d -o CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o -c /home/ubuntu/sunxi/packet_send/main_pkt_recv.cpp

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/sunxi/packet_send/main_pkt_recv.cpp > CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/sunxi/packet_send/main_pkt_recv.cpp -o CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s

# Object files for target packet_recv
packet_recv_OBJECTS = \
"CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o"

# External object files for target packet_recv
packet_recv_EXTERNAL_OBJECTS =

packet_recv: CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o
packet_recv: CMakeFiles/packet_recv.dir/build.make
packet_recv: libpacket_utils.a
packet_recv: libsketch_operations.a
packet_recv: libpacket_utils.a
packet_recv: /usr/lib/aarch64-linux-gnu/libssl.so
packet_recv: /usr/lib/aarch64-linux-gnu/libcrypto.so
packet_recv: CMakeFiles/packet_recv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable packet_recv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packet_recv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packet_recv.dir/build: packet_recv
.PHONY : CMakeFiles/packet_recv.dir/build

CMakeFiles/packet_recv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packet_recv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packet_recv.dir/clean

CMakeFiles/packet_recv.dir/depend:
	cd /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/sunxi/packet_send /home/ubuntu/sunxi/packet_send /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root /home/ubuntu/sunxi/packet_send/cmake-build-dpu-soc-root/CMakeFiles/packet_recv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/packet_recv.dir/depend

