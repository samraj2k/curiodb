//
// Created by Sameer Raj on 15/07/24.
//

#include "../../headers/storage/buffer.h"
#include "../../headers/storage/page.h"
#include <vector>


static BufferMap bufferMap;
static std::vector<BufferId> freeList;
static std::vector<BufferDescriptor*> bufferDescriptors(BUFFER_SLOTS);
static std::vector<Page> bufferPool(BUFFER_SLOTS);
static MonitorRWLock bufferMapLock;

namespace buffer {
     void initBufferMap(const int size = BUFFER_SLOTS) {

        lock::getReadLock(bufferMapLock);
        for(int i = 0; i < size; i++) {
            freeList.push_back(i);
        }
        lock::releaseReadLock(bufferMapLock);
     }

    BufferId readBuffer(const BufferTag &tag) {
        lock::getReadLock(bufferMapLock);
        auto it = bufferMap.find(tag);
        if(it != bufferMap.end()) {
            BufferId bufferId = it->second;
            // atomic operation
            ++bufferDescriptors[bufferId]->pinCount;
            lock::releaseReadLock(bufferMapLock);
            return bufferId;
        } else {
            lock::releaseReadLock(bufferMapLock);
            
            lock::getWriteLock(bufferMapLock);
            
            it = bufferMap.find(tag);
            if (it != bufferMap.end()) {
                BufferId bufferId = it->second;
                ++bufferDescriptors[bufferId]->pinCount;
                lock::releaseWriteLock(bufferMapLock);
                return bufferId;
            }
            
            if (freeList.empty()) {
                lock::releaseWriteLock(bufferMapLock);
                // implement this later
                throw std::runtime_error("No free buffer slots available");
            }
            
            BufferId newBufferId = freeList.back();
            freeList.pop_back();
            
            // Initialize the new buffer descriptor
            bufferDescriptors[newBufferId] = new BufferDescriptor{
                tag,
                newBufferId,
                1,  // Initial pin count
                false,  // Not dirty initially
                MonitorRWLock{}
            };
            
            bufferMap[tag] = newBufferId;
            
            lock::releaseWriteLock(bufferMapLock);
            return newBufferId;
        }
    }



}