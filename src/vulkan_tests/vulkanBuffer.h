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

    void* data;
    size_t dataSize;
private:
    void create();
    void allocate();
    void fill();
    void bindMemory();
public:
    vulkanBuffer(vulkanDevice* device, void* data, size_t dataSize, VkBufferUsageFlags usageFlags);
    ~vulkanBuffer();
    void release();
    void update(void* data, size_t dataSize);
    VkDescriptorBufferInfo getBufferInfo();
};
