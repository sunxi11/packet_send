# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zju/sunxi/turbomon/packet_send

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20

# Include any dependencies generated for this target.
include CMakeFiles/packet_recv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/packet_recv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packet_recv.dir/flags.make

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o: CMakeFiles/packet_recv.dir/flags.make
CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o: ../main_pkt_recv.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o -c /home/zju/sunxi/turbomon/packet_send/main_pkt_recv.cpp

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zju/sunxi/turbomon/packet_send/main_pkt_recv.cpp > CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.i

CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zju/sunxi/turbomon/packet_send/main_pkt_recv.cpp -o CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.s

# Object files for target packet_recv
packet_recv_OBJECTS = \
"CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o"

# External object files for target packet_recv
packet_recv_EXTERNAL_OBJECTS =

packet_recv: CMakeFiles/packet_recv.dir/main_pkt_recv.cpp.o
packet_recv: CMakeFiles/packet_recv.dir/build.make
packet_recv: CMakeFiles/packet_recv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable packet_recv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packet_recv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packet_recv.dir/build: packet_recv

.PHONY : CMakeFiles/packet_recv.dir/build

CMakeFiles/packet_recv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packet_recv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packet_recv.dir/clean

CMakeFiles/packet_recv.dir/depend:
	cd /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20 /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20 /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20/CMakeFiles/packet_recv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/packet_recv.dir/depend

