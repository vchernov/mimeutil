#ifndef MIMEWRITER_H_
#define MIMEWRITER_H_

#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>

#include "MimeInfo.h"

namespace mimeutil {

class MimeWriter {
public:
    MimeWriter();
    MimeWriter(std::shared_ptr<MimeInfo> mimeInfo);
    virtual ~MimeWriter();

    std::shared_ptr<MimeInfo> getMimeInfo() const;

    void write(uint8_t* content, size_t len, const std::string& type, FILE* file);

private:
    std::shared_ptr<MimeInfo> info;
};

}

#endif // MIMEWRITER_H_
