#include "RB_DrawTris.h"
#include "tr_globals.h"
#include "vk_shade_geometry.h"
#include "vk_pipelines.h"
#include "tr_backend.h"
/*
================
Draws triangle outlines for debugging
================
*/
void RB_DrawTris (shaderCommands_t * pInput)
{
	updateCurDescriptor( tr.whiteImage->descriptor_set, 0);

	// VULKAN

    memset(pInput->svars.colors, 255, pInput->numVertexes * 4 );
    VkPipeline pipeline = backEnd.viewParms.isMirror ? g_stdPipelines.tris_mirror_debug_pipeline : g_stdPipelines.tris_debug_pipeline;
    vk_shade_geometry(pipeline, VK_FALSE, DEPTH_RANGE_ZERO, VK_TRUE);
}
