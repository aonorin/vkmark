/*
 * Copyright © 2017 Collabora Ltd.
 *
 * This file is part of vkmark.
 *
 * vkmark is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * vkmark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with vkmark. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Alexandros Frantzis <alexandros.frantzis@collabora.com>
 */

#include "render_pass_builder.h"

#include "vulkan_state.h"

vkutil::RenderPassBuilder::RenderPassBuilder(VulkanState& vulkan)
    : vulkan{vulkan},
      format{vk::Format::eUndefined}
{
}

vkutil::RenderPassBuilder& vkutil::RenderPassBuilder::set_format(vk::Format format_)
{
    format = format_;
    return *this;
}

ManagedResource<vk::RenderPass> vkutil::RenderPassBuilder::build()
{
    auto const color_attachment = vk::AttachmentDescription{}
        .setFormat(format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    auto const color_attachment_ref = vk::AttachmentReference{}
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    auto const subpass = vk::SubpassDescription{}
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setPColorAttachments(&color_attachment_ref);

    auto const render_pass_create_info = vk::RenderPassCreateInfo{}
        .setAttachmentCount(1)
        .setPAttachments(&color_attachment)
        .setSubpassCount(1)
        .setPSubpasses(&subpass);

    return ManagedResource<vk::RenderPass>{
        vulkan.device().createRenderPass(render_pass_create_info),
        [vptr=&vulkan] (auto const& rp) { vptr->device().destroyRenderPass(rp); }};
}
