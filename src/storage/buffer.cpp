//
// Created by Sameer Raj on 15/07/24.
//

#include "../../headers/storage/buffer.h"
#include "../../headers/storage/page.h"
#include "../../headers/storage/io.h"

#include <vector>
#include <cassert>

static BufferMap bufferMap;
static BufferDescriptors bufferDescriptors(BUFFER_SLOTS);
static std::vector<Page> bufferPool(BUFFER_SLOTS);
static ReadWriteLock bufferMapLock;
static BufferId victimBuffer;

namespace buffer {

    void initBufferMap() {
        lock::getWriteLock(bufferMapLock);
        bufferMap.reserve(BUFFER_SLOTS);
        lock::releaseWriteLock(bufferMapLock);
    }

    BufferId evictBuffer() {
        // TODO: implement freeList
        BufferId currentVictim = victimBuffer;
        while(true) {
            if(bufferDescriptors[victimBuffer] == nullptr) {
                break;
            }
            if(!bufferDescriptors[victimBuffer]->pinCount.load()) {
                if(bufferDescriptors[victimBuffer]->isDirty) {
                    // separate process will write to disk
                    // for now skip this
                } else if (!bufferDescriptors[victimBuffer]->usageCount.load()) {
                    break;
                } else if(bufferDescriptors[victimBuffer]->usageCount) {
                    --bufferDescriptors[victimBuffer]->usageCount;
                }
            }
            victimBuffer = (victimBuffer + 1) % BUFFER_SLOTS;
        }
        // set victim buffer to next one for future
        victimBuffer = (victimBuffer + 1) % BUFFER_SLOTS;
        bufferMap.erase(bufferDescriptors[victimBuffer]->tag);
        delete bufferDescriptors[victimBuffer];
        return currentVictim;
    }

    bool bufferIO(BufferDescriptor* bufferDesc) {
        assert(bufferDesc->contentLock.isWriteLocked.load());
        BufferTag tag = bufferDesc->tag;
        Page page = io::loadPageFromDisk(tag.blockNumber, tag.fileNumber);
        if(page == nullptr) {
            return false;
        }
        bufferPool[bufferDesc->id] = page;
        return true;
    }

    BufferDescriptor* getBufferDesc(const BufferTag &tag) {
        // lock must be taken before entering this method
        auto it = bufferMap.find(tag);
        if(it != bufferMap.end()) {
            return bufferDescriptors[it->second];
        }
        return nullptr;
    }

    BufferDescriptor* loadBuffer(const BufferTag &tag) {
        assert(bufferMapLock.isWriteLocked);
        BufferId evictedBuffer = evictBuffer();
        // this operation is only perfomed when bufferMap is write locked
        // thus thread safe
        bufferDescriptors[evictedBuffer] = new BufferDescriptor{
            tag,
            evictedBuffer
        };
        bufferMap[tag] = evictedBuffer;
        return bufferDescriptors[evictedBuffer];
    }

    BufferId readBuffer(const BufferTag &tag) {
        lock::getReadLock(bufferMapLock);
        BufferDescriptor* bufferDesc = getBufferDesc(tag);
        if(bufferDesc != nullptr) {
            // buffer found
            ++bufferDesc->pinCount;
            ++bufferDesc->usageCount;
            lock::releaseReadLock(bufferMapLock);
            return bufferDesc->id;
        }
        lock::releaseReadLock(bufferMapLock);

        lock::getWriteLock(bufferMapLock);
        // maybe while leaving the lock, the buffer got filled
        bufferDesc = getBufferDesc(tag);
        if(bufferDesc != nullptr) {
            // buffer found
            pin(bufferDesc);
            lock::releaseWriteLock(bufferMapLock);
            return bufferDesc->id;
        }

        // if still not found, then we need to read from disk
        // two scene: 1. buffer is empty 2. buffer is full
        // if buffer is empty, then we can directly read from disk
        // if buffer is full, then we need to evict a buffer, then load from disk
        bufferDesc = loadBuffer(tag);
        pin(bufferDesc);
        // take lock on content since we will do IO
        lock::getWriteLock(bufferDesc->contentLock);
        // release write lock on bufferMap, no longer required
        // the incoming reads on this buffer will be blocked due to above taken write lock
        lock::releaseWriteLock(bufferMapLock);

        if(!bufferIO(bufferDesc)) {
            // handle read error
            return NULL;
        }
        lock::releaseWriteLock(bufferDesc->contentLock);
        return bufferDesc->id;
    }

    void releaseBuffer(BufferId bufferId) {
        // must have been pinned already
        // cannot be evicted
        assert(isValidBuffer(bufferId));
        BufferDescriptor* bufferDesc = bufferDescriptors[bufferId];
        unpin(bufferDesc);
    }

}

