#ifndef MIMEPARSER_H_
#define MIMEPARSER_H_

#include <cstdio>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "MimeInfo.h"

namespace mimeutil {

/**
 * Parser of MIME containers (Motion JPEG or MJPEG format).
 */
class MimeParser {
public:
    MimeParser();
    MimeParser(std::shared_ptr<MimeInfo> mimeInfo);
    virtual ~MimeParser();

    MimeParser(const MimeParser&) = delete;
    void operator=(const MimeParser&) = delete;

    std::shared_ptr<MimeInfo> getMimeInfo() const;

    /// Gets buffer with fully loaded content (or NULL if the content is not ready).
    uint8_t* getContent() const;

    /// Returns true if all bytes of content have been loaded.
    bool isContentReady() const;

    size_t getContentLength() const;
    const std::string& getContentType() const;

    /// Sets to initial state.
    void init();

    void parse(char c);

    /**
     * Reads next available content from buffer in memory.
     * @param[in]  buffer  The memory buffer with MIME encoded data.
     * @param[in]  len     The length of the buffer.
     * @param[out] offset  The offset in buffer, will be moved after invocation.
     * @return True if the new content has been reed, false otherwise.
     */
    bool readNext(char* buffer, size_t len, size_t& offset);

    /**
     * Reads next available content from MIME file.
     * @param file The file descriptor.
     * @return True if the new content has been reed, false otherwise.
     */
    bool readNext(FILE* file);

protected:
    typedef std::vector<std::string> HeaderContainer;

    virtual void parseHeaders(const HeaderContainer& headers);

private:
    enum State {
        S_BOUNDARY,
        S_HEADER,
        S_CONTENT
    };

    std::shared_ptr<MimeInfo> info;

    State state;

    std::string target; // current search target
    size_t targetInd; // next character index at target to search

    std::stringstream headerBuffer;
    HeaderContainer headers;

    std::string contentType;
    size_t contentLength;
    size_t contentLoaded; // amount of loaded bytes of the content

    uint8_t* content; // buffer with enough size to store the content data
    size_t contentSize; // real size of the content buffer, contentSize >= contentLength
};

}

#endif // MIMEPARSER_H_
