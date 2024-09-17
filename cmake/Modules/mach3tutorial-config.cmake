# Set the creation date
string(TIMESTAMP CREATION_DATE "%d-%m-%Y")

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Templates/mach3tutorial-config.in
  "${PROJECT_BINARY_DIR}/mach3tutorial-config" @ONLY)
install(PROGRAMS
  "${PROJECT_BINARY_DIR}/mach3tutorial-config" DESTINATION
  bin)
