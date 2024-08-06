//
// Created by Sameer Raj on 05/08/24.
//

#ifndef BGWRITER_H
#define BGWRITER_H

#include "storage/buffer.h"
#include "storage/page.h"

namespace io {
    void startBgWriter(const BufferDescriptors& bufferDescriptors, BufferId victimBuffer,
        const std::vector<Page>& bufferPool);
}
#endif //BGWRITER_H
