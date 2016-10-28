#include <cassert>
#include <chrono>
#include <iostream>

#include "../mimeutil/MimeParser.h"

using namespace mimeutil;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " MIME_container_file" << std::endl;
        return 1;
    }

    const char* testFileName = argv[1];
    FILE* testFile = fopen(testFileName, "rb");
    assert(testFile != nullptr);

    auto parser = std::make_unique<MimeParser>();
    size_t frameNum = 0;
    while (true) {
        if (!parser->readNext(testFile)) {
            break;
        }
        frameNum++;

        std::cout << "frame #" << frameNum;
        std::cout << " " << parser->getContentType();
        std::cout << " size: " << parser->getContentLength();
        std::cout << std::endl;
    }

    fclose(testFile);
    return 0;
}
