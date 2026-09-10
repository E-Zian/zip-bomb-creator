#include "LocalFileHeader.h"
#include "Helper.h"
#include "CentralDirectory.h"
#include <iostream>

int main() {
    LocalFileHeader lfh { LocalFileHeader::createStored("a.txt","Hello")};
    CentralDirectoryHeader cdh{CentralDirectoryHeader::createBasic(lfh)};

    auto lfhBuffer = lfh.serialize();
    helper::displayBytes(lfhBuffer);

    std::cout << "\n\n" << "Central Directory Header :\n" << std::endl;
    auto cdhBuffer = cdh.serialize();
    helper::displayBytes(cdhBuffer);


}