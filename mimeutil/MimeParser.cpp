#include "MimeParser.h"

#include <stdexcept>
#include <string.h>

namespace mimeutil {

MimeParser::MimeParser() {
    info = std::make_shared<MimeInfo>();
    init();
}

MimeParser::MimeParser(std::shared_ptr<MimeInfo> mimeInfo) : info(mimeInfo) {
    init();
}

MimeParser::~MimeParser() {
    delete[] content;
}

void MimeParser::init() {
    headerBuffer.str("");
    headerBuffer.clear();

    target = "";
    targetInd = 0;

    contentType = "";
    contentLength = 0;
    contentLoaded = 0;

    state = S_BOUNDARY;

    delete[] content;
    content = nullptr;
    contentSize = 0;
}

void MimeParser::parse(char c) {
    // lazy initialization of the state specific parameters
    if (state == S_BOUNDARY) {
        if (target.empty()) {
            target = info->getOpenDelimiter();
            targetInd = 0;
        }
    }
    if (state == S_HEADER) {
        if (target.empty()) {
            target = MimeInfo::endOfLine;
            targetInd = 0;
        }
    }

    // searching for the target string
    bool targetFound = false;
    if (!target.empty()) {
        if (c == target[targetInd]) {
            targetInd++;
        } else {
            targetInd = 0;
        }
        if (targetInd == target.length()) {
            targetFound = true;
            target = "";
            targetInd = 0;
        }
    }

    switch (state) {
        case S_BOUNDARY:
            if (targetFound) {
                // dash + boundary + end of line
                state = S_HEADER;
            }
            break;
        case S_HEADER:
            headerBuffer << c;

            if (targetFound) {
                // end of line
                std::string line = headerBuffer.str();

                if (line == MimeInfo::endOfLine) {
                    // empty
                    parseHeaders(headers);
                    headers.clear();
                    state = S_CONTENT;
                } else {
                    size_t usefulPart = line.length() - MimeInfo::endOfLine.length();
                    std::string header = line.substr(0, usefulPart);
                    headers.push_back(header);
                }

                headerBuffer.str("");
                headerBuffer.clear();
            }
            break;
        case S_CONTENT:
            if (contentLoaded < contentLength) {
                content[contentLoaded] = (uint8_t)c;
                contentLoaded++;
                if (contentLoaded == contentLength) {
                    state = S_BOUNDARY;
                }
            } else {
                throw std::runtime_error("MIME content length exceeded");
            }
            break;
        default:
            throw std::runtime_error("Unknown state of MimeParser");
    }
}

void MimeParser::parseHeaders(const HeaderContainer& headers) {
    contentLength = 0;
    contentLoaded = 0;
    contentType = "";

    for (auto& header : headers) {
        std::stringstream ss(header);

        std::string descr;
        if (!std::getline(ss, descr, ':')) {
            continue;
        }

        if (descr == info->getTypeHeader()) {
            ss >> contentType;
        }

        if (descr == info->getLengthHeader()) {
            ss >> contentLength;
        }
    }

    if (contentLength > contentSize) {
        delete[] content;
        content = new uint8_t[contentLength];
        contentSize = contentLength;
    }
}

bool MimeParser::readNext(char* buffer, size_t len, size_t& offset) {
    bool loaded = false;
    for (; offset < len; offset++) {
        if (state == S_CONTENT) {
            // speed up content reading
            size_t remLen = contentLength - contentLoaded;
            size_t avail = len - offset;
            if (remLen > avail) {
                remLen = avail;
            }
            memcpy(content + contentLoaded, buffer + offset, remLen);
            contentLoaded += remLen;
            offset += remLen;

            if (contentLoaded == contentLength) {
                state = S_BOUNDARY;
                loaded = true;
                break;
            }
        } else {
            char c = buffer[offset];
            parse(c);
        }
    }
    return loaded;
}

bool MimeParser::readNext(FILE* file) {
    bool loaded = false;
    while (!feof(file)) {
        if (state == S_CONTENT) {
            // speed up content reading
            size_t remLen = contentLength - contentLoaded;
            size_t readLen = fread(content + contentLoaded, 1, remLen, file);
            contentLoaded += readLen;

            if (contentLoaded == contentLength) {
                state = S_BOUNDARY;
                loaded = true;
            }
            break;
        } else {
            char c = (char)fgetc(file);
            parse(c);
        }
    }
    return loaded;
}

std::shared_ptr<MimeInfo> MimeParser::getMimeInfo() const {
    return info;
}

uint8_t* MimeParser::getContent() const {
    return isContentReady() ? content : nullptr;
}

bool MimeParser::isContentReady() const {
    return contentLoaded == contentLength;
}

size_t MimeParser::getContentLength() const {
    return contentLength;
}

const std::string& MimeParser::getContentType() const {
    return contentType;
}

}
