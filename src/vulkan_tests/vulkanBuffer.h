#pragma once

#include "vulkanDevice.h"

#include <vulkan\vulkan.h>

class vulkanBuffer
{
private:
    vulkanDevice* _device;
    VkBufferUsageFlags _usageFlags;
public:
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize offset;
    VkDeviceSize range;

    uint32_t dataSize;
    void* data;
private:
    void create();
    void allocate();
    void fill();
    void bindMemory();
public:
    vulkanBuffer(vulkanDevice* device, uint32_t dataSize, void* data, VkBufferUsageFlags usageFlags);
    ~vulkanBuffer();
    void release();
    void update(uint32_t dataSize, void* data);
    VkDescriptorBufferInfo getBufferInfo();
    
};
