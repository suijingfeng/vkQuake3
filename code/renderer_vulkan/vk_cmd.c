#include "VKimpl.h"
#include "vk_instance.h"
//  ===============  Command Buffer Lifecycle ====================
// Each command buffer is always in one of the following states:
//
// Initial
//
// When a command buffer is allocated, it is in the initial state. Some commands are able to
// reset a command buffer, or a set of command buffers, back to this state from any of the 
// executable, recording or invalid state. Command buffers in the initial state can only be
// moved to the recording state, or freed.
//
// Recording
//
// vkBeginCommandBuffer changes the state of a command buffer from the initial state to the
// recording state. Once a command buffer is in the recording state, vkCmd* commands can be
// used to record to the command buffer.
//
// Executable
//
// vkEndCommandBuffer ends the recording of a command buffer, and moves it from the recording
// state to the executable state. Executable command buffers can be submitted, reset, or
// recorded to another command buffer.
//
// Pending
//
// Queue submission of a command buffer changes the state of a command buffer from the
// executable state to the pending state. Whilst in the pending state, applications must
// not attempt to modify the command buffer in any way - as the device may be processing
// the commands recorded to it. Once execution of a command buffer completes, the command
// buffer reverts back to either the executable state, or the invalid state if it was
// recorded with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT. A synchronization command
// should be used to detect when this occurs.
//
// Invalid
// 
// Some operations, such as modifying or deleting a resource that was used in a command
// recorded to a command buffer, will transition the state of that command buffer into
// the invalid state. Command buffers in the invalid state can only be reset or freed.
//
// Any given command that operates on a command buffer has its own requirements on what
// state a command buffer must be in, which are detailed in the valid usage constraints
// for that command.
// 
// Resetting a command buffer is an operation that discards any previously recorded 
// commands and puts a command buffer in the initial state. Resetting occurs as a
// result of vkResetCommandBuffer or vkResetCommandPool, or as part of 
// vkBeginCommandBuffer (which additionally puts the command buffer in the recording state).
//
// Secondary command buffers can be recorded to a primary command buffer via
// vkCmdExecuteCommands. This partially ties the lifecycle of the two command
// buffers together - if the primary is submitted to a queue, both the primary 
// and any secondaries recorded to it move to the pending state. Once execution
// of the primary completes, so does any secondary recorded within it, and once all 
// executions of each command buffer complete, they move to the executable state.
// If a secondary moves to any other state whilst it is recorded to another command buffer, 
// the primary moves to the invalid state. A primary moving to any other state does 
// not affect the state of the secondary. Resetting or freeing a primary command buffer 
// removes the linkage to any secondary command buffers that were recorded to it.
//
//

void record_image_layout_transition( 
        VkCommandBuffer cmdBuf,
        VkImage image,
        VkImageAspectFlags image_aspect_flags,
        VkAccessFlags src_access_flags,
        VkImageLayout old_layout,
        VkAccessFlags dst_access_flags,
        VkImageLayout new_layout )
{

	VkImageMemoryBarrier barrier = {0};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = NULL;
	barrier.srcAccessMask = src_access_flags;
	barrier.dstAccessMask = dst_access_flags;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = image_aspect_flags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

// vkCmdPipelineBarrier is a synchronization command that inserts a dependency between
// commands submitted to the same queue, or between commands in the same subpass.
// When vkCmdPipelineBarrier is submitted to a queue, it defines a memory dependency
// between commands that were submitted before it, and those submitted after it.
    
    // cmdBuf is the command buffer into which the command is recorded.
	qvkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,	0, NULL, 0, NULL, 1, &barrier);
}

