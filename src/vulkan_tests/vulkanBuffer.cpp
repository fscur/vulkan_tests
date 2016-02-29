#include "vulkanBuffer.h"

#include "vulkanHelper.h"

vulkanBuffer::vulkanBuffer(vulkanDevice* device, void* data, size_t dataSize, VkBufferUsageFlags usageFlags) :
    _device(device),
    _usageFlags(usageFlags),
    data(data),
    dataSize(dataSize)
{
    create();
    allocate();
    fill();
    bindMemory();
}

vulkanBuffer::~vulkanBuffer()
{
    //delete(data); //TODO:fix vulkanBuffer delete
    release();
}

void vulkanBuffer::release()
{
    vkDestroyBuffer(_device->vkDevice, buffer, NULL);
    vkFreeMemory(_device->vkDevice, memory, NULL);
}

void vulkanBuffer::create()
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.usage = _usageFlags;
    createInfo.size = dataSize;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.flags = 0;

    auto result = vkCreateBuffer(_device->vkDevice, &createInfo, NULL, &buffer);
    assert(!result);
}

void vulkanBuffer::allocate()
{
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(_device->vkDevice, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = NULL;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.allocationSize = memoryRequirements.size;

    auto pass = vulkanHelper::memoryTypeFromProperties(
        *_device->memoryProperties,
        memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &allocateInfo.memoryTypeIndex);
    assert(pass);

    auto result = vkAllocateMemory(_device->vkDevice, &allocateInfo, NULL, &memory);
    assert(!result);

    range = memoryRequirements.size;
    offset = 0;
}

void vulkanBuffer::fill()
{
    uint8_t* dataPtr;
    auto result = vkMapMemory(_device->vkDevice, memory, 0, range, 0, (void **)&dataPtr);
    assert(!result);

    if(data != nullptr)
        memcpy(dataPtr, data, dataSize);

    vkUnmapMemory(_device->vkDevice, memory);
}

void vulkanBuffer::bindMemory()
{
    auto result = vkBindBufferMemory(_device->vkDevice, buffer, memory, 0);
    assert(!result);
}

void vulkanBuffer::update(void* data, size_t dataSize)
{
    auto oldDataSize = this->dataSize;

    this->dataSize = dataSize;
    this->data = data;

    if (oldDataSize != dataSize)
    {
        release();
        create();
        allocate();
        fill();
        bindMemory();
    }
    else
    {
        fill();
    }
}

VkDescriptorBufferInfo vulkanBuffer::getBufferInfo()
{
    return VkDescriptorBufferInfo() =
    {
        buffer,
        offset,
        range
    };
}