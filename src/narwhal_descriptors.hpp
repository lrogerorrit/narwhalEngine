#pragma once

#include "narwhal_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>


namespace narwhal {

    class NarwhalDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(NarwhalDevice& narwhalDevice) : narwhalDevice{ narwhalDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<NarwhalDescriptorSetLayout> build() const;

        private:
            NarwhalDevice& narwhalDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        NarwhalDescriptorSetLayout(
            NarwhalDevice& narwhalDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~NarwhalDescriptorSetLayout();
        NarwhalDescriptorSetLayout(const NarwhalDescriptorSetLayout&) = delete;
        NarwhalDescriptorSetLayout& operator=(const NarwhalDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        NarwhalDevice& narwhalDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class NarwhalDescriptorWriter;
    };

    class NarwhalDescriptorPool {
    public:
        class Builder {
        public:
            Builder(NarwhalDevice& narwhalDevice) : narwhalDevice{ narwhalDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<NarwhalDescriptorPool> build() const;

        private:
            NarwhalDevice& narwhalDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        NarwhalDescriptorPool(
            NarwhalDevice& narwhalDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~NarwhalDescriptorPool();
        NarwhalDescriptorPool(const NarwhalDescriptorPool&) = delete;
        NarwhalDescriptorPool& operator=(const NarwhalDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();
        VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

    private:
        NarwhalDevice& narwhalDevice;
        VkDescriptorPool descriptorPool;

        friend class NarwhalDescriptorWriter;
    };

    class NarwhalDescriptorWriter {
    public:
        NarwhalDescriptorWriter(NarwhalDescriptorSetLayout& setLayout, NarwhalDescriptorPool& pool);

        NarwhalDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        NarwhalDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        NarwhalDescriptorSetLayout& setLayout;
        NarwhalDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace narwhal