//
// Created by Sameer Raj on 05/08/24.
//

#include "io/bgwriter.h"
#include "io/io.h"
#include "configs/constants.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>

namespace io {

    void flushDirtyPages(const BufferDescriptors& bufferDescriptors,
                        const BufferId victimBuffer,
                        const std::vector<Page>& bufferPool) {
        BufferId currentBuffer = victimBuffer;
        int flushedPages = 0;

        for (int i = 0; i < BUFFER_SLOTS && flushedPages < MAX_FLUSH_PAGES; ++i) {
            BufferDescriptor* bufferDesc = bufferDescriptors[currentBuffer];
            if(bufferDesc == nullptr) {
                currentBuffer = (currentBuffer + 1) % BUFFER_SLOTS;
                continue;
            }
            // pin buffer, so does not get evicted
            buffer::pin(bufferDesc);
            if (bufferDesc->isDirty.load()) {
                // since read lock taken, no one can write to this buffer
                bufferDesc->contentLock.getReadLock();

                if (bufferDesc->isDirty.load()) {
                    try {
                        io::writePageToDisk(bufferPool[currentBuffer],
                            bufferDesc->tag.blockNumber,
                            bufferDesc->tag.fileNumber);

                        bufferDesc->isDirty.store(false);

                    } catch (const std::exception& e) {
                        spdlog::error("Error writing page to disk for block {} in file {}: {}",
                                      bufferDesc->tag.blockNumber,
                                      bufferDesc->tag.fileNumber,
                                      e.what());
                    }
                    ++flushedPages;
                }

                bufferDesc->contentLock.releaseReadLock();
            }

            buffer::unpin(bufferDesc);

            currentBuffer = (currentBuffer + 1) % BUFFER_SLOTS;
        }
    }

    void startBgWriter(const BufferDescriptors& bufferDescriptors, BufferId victimBuffer,
        const std::vector<Page>& bufferPool) {
        // std::thread bgWriter(bgWriterThread, bufferDescriptors, victimBuffer, bufferPool);
        auto bgWriterLambda = [&]() {
            while (true) {
                flushDirtyPages(bufferDescriptors, victimBuffer, bufferPool);
                std::this_thread::sleep_for(std::chrono::milliseconds(BG_WRITER_INTERVAL));
            }
        };
        std::thread bgWriter(bgWriterLambda);
        bgWriter.detach();
    }

}