add_executable (MondLint Main.cpp)
target_link_libraries (MondLint LINK_PUBLIC MondX)
set_target_properties (MondLint PROPERTIES OUTPUT_NAME mondx-lint)
