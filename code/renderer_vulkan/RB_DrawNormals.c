#include "tr_backend.h"
#include "vk_shade_geometry.h"
#include "tr_globals.h"
#include "vk_pipelines.h"

/*
================
Draws vertex normals for debugging
================
*/
void RB_DrawNormals (shaderCommands_t *input)
{
	// VULKAN

    vec4_t xyz[SHADER_MAX_VERTEXES];
    memcpy(xyz, tess.xyz, tess.numVertexes * sizeof(vec4_t));
    memset(tess.svars.colors, tr.identityLightByte, SHADER_MAX_VERTEXES * sizeof(color4ub_t));

    int numVertexes = tess.numVertexes;
    int i = 0;
    while (i < numVertexes)
    {
        int count = numVertexes - i;
        if (count >= SHADER_MAX_VERTEXES/2 - 1)
            count = SHADER_MAX_VERTEXES/2 - 1;

        int k;
        for (k = 0; k < count; k++)
        {
            VectorCopy(xyz[i + k], tess.xyz[2*k]);
            VectorMA(xyz[i + k], 2, input->normal[i + k], tess.xyz[2*k + 1]);
        }
        tess.numVertexes = 2 * count;
        tess.numIndexes = 0;

        uploadShadingData();
        updateMVP(backEnd.viewParms.isPortal, backEnd.projection2D, getptr_modelview_matrix());
        vk_shade_geometry(g_stdPipelines.normals_debug_pipeline, VK_FALSE, DEPTH_RANGE_ZERO, VK_TRUE);

        i += count;
    }
}
