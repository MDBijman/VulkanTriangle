#pragma once
#include <set>

#include "Modules/Graphics/Data/GPUBuffer.h"
#include "Modules/TemplateUtils/ForEachInTuple.h"

namespace Graphics
{
	namespace Data
	{
		template<class... T>
		class Uniforms : public GPUBuffer
		{
		public:
			Uniforms(Graphics::VulkanContext& context, std::tuple<T...>&& d) : data(std::move(d))
			{
				/*
					Descriptor Set Layout Initialization
				*/

				auto setLayoutDescriptors = getDescriptorLayoutBindings<T...>();
				if(setLayoutDescriptors.size() == 0)
					return;
				setLayoutBindings = new std::vector<vk::DescriptorSetLayoutBinding>(setLayoutDescriptors);

				vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
				setLayoutCreateInfo
					.setBindingCount(static_cast<uint32_t>(setLayoutBindings->size()))
					.setPBindings(setLayoutBindings->data());
				descriptorSetLayout = context.device.createDescriptorSetLayout(setLayoutCreateInfo);

				/*
					Descriptor Pool Initialization
				*/

				auto poolSizeDescriptors = getDescriptorPoolSizes<T...>();
				if(poolSizeDescriptors.size() == 0)
					return;
				poolSizes = new std::vector<vk::DescriptorPoolSize>(poolSizeDescriptors);

				vk::DescriptorPoolCreateInfo poolCreateInfo = vk::DescriptorPoolCreateInfo()
					.setPoolSizeCount(static_cast<uint32_t>(poolSizes->size()))
					.setPPoolSizes(poolSizes->data())
					// Set the max. number of sets that can be requested
					// Requesting descriptors beyond maxSets will result in an error
					.setMaxSets(1);
				descriptorPool = context.device.createDescriptorPool(poolCreateInfo);

				/*
					Descriptor Set Initialization
				*/

				vk::DescriptorSetAllocateInfo allocInfo;
				allocInfo
					.setDescriptorPool(descriptorPool)
					.setDescriptorSetCount(1)
					.setPSetLayouts(&descriptorSetLayout);
				descriptorSet = context.device.allocateDescriptorSets(allocInfo).at(0);
				std::vector<vk::WriteDescriptorSet> setWriters;
				for_each_in_tuple(data, [&](auto& t)
				{
					setWriters.push_back(t.getWriteDescriptorSet());
				});
				for(auto& writer : setWriters)
					writer.dstSet = descriptorSet;

				context.device.updateDescriptorSets(setWriters, nullptr);
			}

			Uniforms(Uniforms&& other) : data(std::move(other.data))
			{
				this->setLayoutBindings = other.setLayoutBindings;
				other.setLayoutBindings = nullptr;

				this->poolSizes = other.poolSizes;
				other.poolSizes = nullptr;

				descriptorSet = other.descriptorSet;
				descriptorSetLayout = other.descriptorSetLayout;
				descriptorPool = other.descriptorPool;
			}
				
			void upload(vk::Device& device, vk::PhysicalDevice& physicalDevice) override
			{
				for_each_in_tuple(data, [&](auto& t)
				{
					t.upload(device, physicalDevice);
				});
			}

			const vk::DescriptorSetLayout& getDescriptorSetLayout()
			{
				return descriptorSetLayout;
			}

			const vk::DescriptorSet& getDescriptorSet()
			{
				return descriptorSet;
			}

		private:
			template<class... A>
			std::vector<vk::DescriptorSetLayoutBinding> getDescriptorLayoutBindings()
			{
				return{
					A::getDescriptorSetLayoutBinding()...
				};
			}

			template<class... A>
			std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes()
			{
				return{
					A::getDescriptorPoolSize()...
				};
			}
			
			std::vector<vk::DescriptorSetLayoutBinding>* setLayoutBindings;
			std::vector<vk::DescriptorPoolSize>* poolSizes;

			vk::DescriptorSetLayout		descriptorSetLayout;
			vk::DescriptorPool          descriptorPool;
			vk::DescriptorSet           descriptorSet;

			std::tuple<T...> data;
		};
	}
}