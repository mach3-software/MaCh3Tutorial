// MaCh3 spline includes
#include "Utils/Comparison.h"

bool CompareTwoFiles(const std::string& File1, const std::string& File2) {
  std::ifstream file1(File1);
  std::ifstream file2(File2);

  // Check if both files opened successfully
  if (!file1.is_open()) {
    std::cerr << "Error opening file: " << File1 << std::endl;
    return false;
  }
  if (!file2.is_open()) {
    std::cerr << "Error opening file: " << File2 << std::endl;
    return false;
  }

  std::string line1, line2;
  bool sameFiles = true;
  int lineNumber = 1;

  // Read and compare line by line
  while (std::getline(file1, line1) && std::getline(file2, line2)) {
    if (line1 != line2) {
      std::cout << "Difference found on line " << lineNumber << ":" << std::endl;
      std::cout << "File1: " << line1 << std::endl;
      std::cout << "File2: " << line2 << std::endl;
      sameFiles = false;
    }
    ++lineNumber;
  }

  // Check if one file has extra lines
  while (std::getline(file1, line1)) {
    std::cout << "Extra line in " << File1 << " on line " << lineNumber << ": " << line1 << std::endl;
    sameFiles = false;
    ++lineNumber;
  }
  while (std::getline(file2, line2)) {
    std::cout << "Extra line in " << File2 << " on line " << lineNumber << ": " << line2 << std::endl;
    sameFiles = false;
    ++lineNumber;
  }

  // Close files
  file1.close();
  file2.close();

  return sameFiles;
}
