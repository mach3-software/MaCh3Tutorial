// MaCh3 spline includes
#include "Utils/Comparison.h"

bool CompareTwoFiles(const std::string& File1, const std::string& File2) {
  std::ifstream file1(File1);
  std::ifstream file2(File2);

  // Check if both files opened successfully
  if (!file1.is_open()) {
    MACH3LOG_ERROR("Error opening file:  {}", File1);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  if (!file2.is_open()) {
    MACH3LOG_ERROR("Error opening file:  {}", File2);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::string line1, line2;
  bool sameFiles = true;
  int lineNumber = 1;

  // Read and compare line by line
  while (std::getline(file1, line1) && std::getline(file2, line2)) {
    if (line1 != line2) {
      MACH3LOG_WARN("Difference found on line {}:", lineNumber);
      MACH3LOG_WARN("File1: {}", line1);
      MACH3LOG_WARN("File2: {}", line2);
      sameFiles = false;
    }
    ++lineNumber;
  }

  // Check if one file has extra lines
  while (std::getline(file1, line1)) {
    MACH3LOG_WARN("Extra line in {} on line {}: {}", File1, lineNumber, line1);
    sameFiles = false;
    ++lineNumber;
  }
  while (std::getline(file2, line2)) {
    MACH3LOG_WARN("Extra line in {} on line {}: {}", File2, lineNumber, line2);
    sameFiles = false;
    ++lineNumber;
  }

  // Close files
  file1.close();
  file2.close();

  return sameFiles;
}
