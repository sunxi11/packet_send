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
include CMakeFiles/sketch_operations.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sketch_operations.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sketch_operations.dir/flags.make

CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o: CMakeFiles/sketch_operations.dir/flags.make
CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o: ../Sketch_operations.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o -c /home/zju/sunxi/turbomon/packet_send/Sketch_operations.cpp

CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zju/sunxi/turbomon/packet_send/Sketch_operations.cpp > CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.i

CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zju/sunxi/turbomon/packet_send/Sketch_operations.cpp -o CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.s

# Object files for target sketch_operations
sketch_operations_OBJECTS = \
"CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o"

# External object files for target sketch_operations
sketch_operations_EXTERNAL_OBJECTS =

libsketch_operations.a: CMakeFiles/sketch_operations.dir/Sketch_operations.cpp.o
libsketch_operations.a: CMakeFiles/sketch_operations.dir/build.make
libsketch_operations.a: CMakeFiles/sketch_operations.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libsketch_operations.a"
	$(CMAKE_COMMAND) -P CMakeFiles/sketch_operations.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sketch_operations.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sketch_operations.dir/build: libsketch_operations.a

.PHONY : CMakeFiles/sketch_operations.dir/build

CMakeFiles/sketch_operations.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sketch_operations.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sketch_operations.dir/clean

CMakeFiles/sketch_operations.dir/depend:
	cd /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root /home/zju/sunxi/turbomon/packet_send/cmake-build-zjlab20-root/CMakeFiles/sketch_operations.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sketch_operations.dir/depend

