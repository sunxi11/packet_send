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
CMAKE_BINARY_DIR = /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root

# Include any dependencies generated for this target.
include CMakeFiles/packet_send.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/packet_send.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packet_send.dir/flags.make

CMakeFiles/packet_send.dir/ow_packet_send.cpp.o: CMakeFiles/packet_send.dir/flags.make
CMakeFiles/packet_send.dir/ow_packet_send.cpp.o: ../ow_packet_send.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/packet_send.dir/ow_packet_send.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packet_send.dir/ow_packet_send.cpp.o -c /home/zju/sunxi/turbomon/packet_send/ow_packet_send.cpp

CMakeFiles/packet_send.dir/ow_packet_send.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packet_send.dir/ow_packet_send.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zju/sunxi/turbomon/packet_send/ow_packet_send.cpp > CMakeFiles/packet_send.dir/ow_packet_send.cpp.i

CMakeFiles/packet_send.dir/ow_packet_send.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packet_send.dir/ow_packet_send.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zju/sunxi/turbomon/packet_send/ow_packet_send.cpp -o CMakeFiles/packet_send.dir/ow_packet_send.cpp.s

CMakeFiles/packet_send.dir/rdma_server.cpp.o: CMakeFiles/packet_send.dir/flags.make
CMakeFiles/packet_send.dir/rdma_server.cpp.o: ../rdma_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/packet_send.dir/rdma_server.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/packet_send.dir/rdma_server.cpp.o -c /home/zju/sunxi/turbomon/packet_send/rdma_server.cpp

CMakeFiles/packet_send.dir/rdma_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/packet_send.dir/rdma_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zju/sunxi/turbomon/packet_send/rdma_server.cpp > CMakeFiles/packet_send.dir/rdma_server.cpp.i

CMakeFiles/packet_send.dir/rdma_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/packet_send.dir/rdma_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zju/sunxi/turbomon/packet_send/rdma_server.cpp -o CMakeFiles/packet_send.dir/rdma_server.cpp.s

# Object files for target packet_send
packet_send_OBJECTS = \
"CMakeFiles/packet_send.dir/ow_packet_send.cpp.o" \
"CMakeFiles/packet_send.dir/rdma_server.cpp.o"

# External object files for target packet_send
packet_send_EXTERNAL_OBJECTS =

packet_send: CMakeFiles/packet_send.dir/ow_packet_send.cpp.o
packet_send: CMakeFiles/packet_send.dir/rdma_server.cpp.o
packet_send: CMakeFiles/packet_send.dir/build.make
packet_send: libpacket_utils.a
packet_send: libsketch_operations.a
packet_send: librdmacm_tools.a
packet_send: /home/zju/sunxi/turbomon/sketches/cmake-build-remote-zjlab20/libsketches-lhy.so
packet_send: libpacket_utils.a
packet_send: /usr/lib/x86_64-linux-gnu/libssl.so
packet_send: /usr/lib/x86_64-linux-gnu/libcrypto.so
packet_send: CMakeFiles/packet_send.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable packet_send"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packet_send.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packet_send.dir/build: packet_send

.PHONY : CMakeFiles/packet_send.dir/build

CMakeFiles/packet_send.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packet_send.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packet_send.dir/clean

CMakeFiles/packet_send.dir/depend:
	cd /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles/packet_send.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/packet_send.dir/depend

