//
// Created by Sameer Raj on 05/08/24.
//

#include "../../headers/io/bgwriter.h"
#include "../../headers/io/io.h"
#include "../../headers/configs/constants.h"

#include <iostream>
#include <thread>
#include <chrono>

namespace bgwriter {

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
                lock::getReadLock(bufferDesc->contentLock);

                if (bufferDesc->isDirty.load()) {
                    try {
                        io::writePageToDisk(bufferPool[currentBuffer],
                            bufferDesc->tag.blockNumber,
                            bufferDesc->tag.fileNumber);

                        bufferDesc->isDirty.store(false);

                    } catch (const std::exception& e) {
                        std::cerr << "Error for:" << bufferDesc->tag.blockNumber << " " << bufferDesc->tag.fileNumber;
                        std::cerr << std::endl;
                        std::cerr << "Error writing page to disk: " << e.what() << std::endl;
                    }
                    ++flushedPages;
                }

                lock::releaseReadLock(bufferDesc->contentLock);
            }

            buffer::unpin(bufferDesc);

            currentBuffer = (currentBuffer + 1) % BUFFER_SLOTS;
        }
    }

    void bgWriterThread(const BufferDescriptors& bufferDescriptors,
        const BufferId victimBuffer,
        const std::vector<Page>& bufferPool) {
        while (true) {
            flushDirtyPages(bufferDescriptors, victimBuffer, bufferPool);
            std::this_thread::sleep_for(std::chrono::milliseconds(BG_WRITER_INTERVAL));
        }
    }

    void startBgWriter(const BufferDescriptors& bufferDescriptors,
        BufferId victimBuffer,
        const std::vector<Page>& bufferPool) {
        std::thread bgWriter(bgWriterThread, bufferDescriptors, victimBuffer, bufferPool);
        bgWriter.detach();
    }

}