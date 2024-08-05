//
// Created by Sameer Raj on 15/07/24.
//

#include "../../headers/storage/buffer.h"
#include "../../headers/io/io.h"
#include "../../headers/io/bgwriter.h"

#include <vector>
#include <cassert>

static BufferMap bufferMap;
static BufferDescriptors bufferDescriptors(BUFFER_SLOTS);
static std::vector<Page> bufferPool(BUFFER_SLOTS);
static ReadWriteLock bufferMapLock;
static BufferId victimBuffer = 0;

namespace buffer {

    void initBufferMap() {
        lock::getWriteLock(bufferMapLock);
        bufferMap.reserve(BUFFER_SLOTS);
        lock::releaseWriteLock(bufferMapLock);
        // start bgwriter thread which will flush dirty page
        // this will decrease the load from evict() method
        io::startBgWriter(bufferDescriptors, victimBuffer, bufferPool);
    }

    void dirtyBufferFlushIO(BufferDescriptor* bufferDescriptor) {
        io::writePageToDisk(bufferPool[victimBuffer],
                        bufferDescriptors[victimBuffer]->tag.blockNumber,
                        bufferDescriptors[victimBuffer]->tag.fileNumber);
        bufferDescriptor->isDirty.store(false);
    }

    BufferId evictBuffer() {
        // for now this is called by taking lock on the map
        // but very inefficient, once the program is up and running, TODO: refactor
        const BufferId currentVictim = victimBuffer;
        while(true) {
            if(bufferDescriptors[victimBuffer] == nullptr) {
                // empty buffer, return directly
                break;
            }
            if(!bufferDescriptors[victimBuffer]->pinCount.load()) {
                if(bufferDescriptors[victimBuffer]->isDirty.load()) {
                    dirtyBufferFlushIO(bufferDescriptors[victimBuffer]);
                    
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
        delete bufferPool[victimBuffer];
        delete bufferDescriptors[victimBuffer];
        return currentVictim;
    }

    bool bufferLoadIO(const BufferDescriptor* bufferDesc) {
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

    BufferDescriptor* retrieveBuffer(const BufferTag &tag) {
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

        // TODO: poor design, need to refactor
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
        bufferDesc = retrieveBuffer(tag);
        pin(bufferDesc);
        // take lock on content since we will do IO
        lock::getWriteLock(bufferDesc->contentLock);
        // release write lock on bufferMap, no longer required
        // the incoming reads on this buffer will be blocked due to above taken write lock
        lock::releaseWriteLock(bufferMapLock);

        if(!bufferLoadIO(bufferDesc)) {
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

