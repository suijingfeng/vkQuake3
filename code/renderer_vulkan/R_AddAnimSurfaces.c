#include "tr_local.h"
#include "tr_globals.h"
#include "tr_model.h"



void R_AddAnimSurfaces( trRefEntity_t *ent )
{

	md4Header_t* header = tr.currentModel->md4;
	md4LOD_t* lod = (md4LOD_t *)( (unsigned char *)header + header->ofsLODs );

	md4Surface_t* surface = (md4Surface_t *)( (unsigned char *)lod + lod->ofsSurfaces );

    int	i;

    for ( i = 0 ; i < lod->numSurfaces ; i++ )
    {
		shader_t* shader = R_GetShaderByHandle( surface->shaderIndex );
		R_AddDrawSurf( (surfaceType_t*) (void *)surface, shader, 0 /*fogNum*/, qfalse );
		surface = (md4Surface_t *)( (unsigned char *)surface + surface->ofsEnd );
	}
}
