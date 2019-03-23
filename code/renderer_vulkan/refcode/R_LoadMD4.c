#include "tr_local.h"
#include "tr_model.h"
#include "../renderercommon/ref_import.h"

#define	LL(x) x=LittleLong(x)


qboolean R_LoadMD4( model_t *mod, void *buffer, const char *mod_name )
{
	int					i, j, k, lodindex;
	md4Header_t			*pinmodel, *md4;
    md4Frame_t			*frame;
	md4LOD_t			*lod;
	md4Surface_t		*surf;
	md4Triangle_t		*tri;
	md4Vertex_t			*v;
	int					version;
	int					size;
	shader_t			*sh;
	int					frameSize;

	pinmodel = (md4Header_t *)buffer;

	version = LittleLong (pinmodel->version);
	if (version != MD4_VERSION) {
		ri.Printf( PRINT_WARNING, "R_LoadMD4: %s has wrong version (%i should be %i)\n",
				 mod_name, version, MD4_VERSION);
		return qfalse;
	}

	mod->type = MOD_MD4;
	size = LittleLong(pinmodel->ofsEnd);
	mod->dataSize += size;
	md4 = mod->md4 = (md4Header_t*) ri.Hunk_Alloc( size, h_low );

	memcpy( md4, buffer, LittleLong(pinmodel->ofsEnd) );

    LL(md4->ident);
    LL(md4->version);
    LL(md4->numFrames);
    LL(md4->numBones);
    LL(md4->numLODs);
    LL(md4->ofsFrames);
    LL(md4->ofsLODs);
    LL(md4->ofsEnd);

	if ( md4->numFrames < 1 ) {
		ri.Printf( PRINT_WARNING, "R_LoadMD4: %s has no frames\n", mod_name );
		return qfalse;
	}

    // we don't need to swap tags in the renderer, they aren't used
    
	// swap all the frames
	frameSize = (int)(intptr_t)( &((md4Frame_t *)0)->bones[ md4->numBones ] );
    for ( i = 0 ; i < md4->numFrames ; i++, frame++) {
	    frame = (md4Frame_t *) ( (byte *)md4 + md4->ofsFrames + i * frameSize );
    	frame->radius = LittleFloat( frame->radius );
        for ( j = 0 ; j < 3 ; j++ ) {
            frame->bounds[0][j] = LittleFloat( frame->bounds[0][j] );
            frame->bounds[1][j] = LittleFloat( frame->bounds[1][j] );
	    	frame->localOrigin[j] = LittleFloat( frame->localOrigin[j] );
        }
		for ( j = 0 ; j < md4->numBones * sizeof( md4Bone_t ) / 4 ; j++ ) {
			((float *)frame->bones)[j] = LittleFloat( ((float *)frame->bones)[j] );
		}
	}

	// swap all the LOD's
	lod = (md4LOD_t *) ( (byte *)md4 + md4->ofsLODs );
	for ( lodindex = 0 ; lodindex < md4->numLODs ; lodindex++ ) {

		// swap all the surfaces
		surf = (md4Surface_t *) ( (byte *)lod + lod->ofsSurfaces );
		for ( i = 0 ; i < lod->numSurfaces ; i++) {
			LL(surf->ident);
			LL(surf->numTriangles);
			LL(surf->ofsTriangles);
			LL(surf->numVerts);
			LL(surf->ofsVerts);
			LL(surf->ofsEnd);
			
			if ( surf->numVerts > SHADER_MAX_VERTEXES ) {
				ri.Error (ERR_DROP, "R_LoadMD3: %s has more than %i verts on a surface (%i)",
					mod_name, SHADER_MAX_VERTEXES, surf->numVerts );
			}
			if ( surf->numTriangles*3 > SHADER_MAX_INDEXES ) {
				ri.Error (ERR_DROP, "R_LoadMD3: %s has more than %i triangles on a surface (%i)",
					mod_name, SHADER_MAX_INDEXES / 3, surf->numTriangles );
			}

			// change to surface identifier
			surf->ident = SF_MD4;

			// lowercase the surface name so skin compares are faster
			Q_strlwr( surf->name );
		
			// register the shaders
			sh = R_FindShader( surf->shader, LIGHTMAP_NONE, qtrue );
			if ( sh->defaultShader ) {
				surf->shaderIndex = 0;
			} else {
				surf->shaderIndex = sh->index;
			}

			// swap all the triangles
			tri = (md4Triangle_t *) ( (byte *)surf + surf->ofsTriangles );
			for ( j = 0 ; j < surf->numTriangles ; j++, tri++ ) {
				LL(tri->indexes[0]);
				LL(tri->indexes[1]);
				LL(tri->indexes[2]);
			}

			// swap all the vertexes
			// FIXME
			// This makes TFC's skeletons work.  Shouldn't be necessary anymore, but left
			// in for reference.
			//v = (md4Vertex_t *) ( (byte *)surf + surf->ofsVerts + 12);
			v = (md4Vertex_t *) ( (byte *)surf + surf->ofsVerts);
			for ( j = 0 ; j < surf->numVerts ; j++ ) {
				v->normal[0] = LittleFloat( v->normal[0] );
				v->normal[1] = LittleFloat( v->normal[1] );
				v->normal[2] = LittleFloat( v->normal[2] );

				v->texCoords[0] = LittleFloat( v->texCoords[0] );
				v->texCoords[1] = LittleFloat( v->texCoords[1] );

				v->numWeights = LittleLong( v->numWeights );

				for ( k = 0 ; k < v->numWeights ; k++ ) {
					v->weights[k].boneIndex = LittleLong( v->weights[k].boneIndex );
					v->weights[k].boneWeight = LittleFloat( v->weights[k].boneWeight );
				   v->weights[k].offset[0] = LittleFloat( v->weights[k].offset[0] );
				   v->weights[k].offset[1] = LittleFloat( v->weights[k].offset[1] );
				   v->weights[k].offset[2] = LittleFloat( v->weights[k].offset[2] );
				}
				// FIXME
				// This makes TFC's skeletons work.  Shouldn't be necessary anymore, but left
				// in for reference.
				//v = (md4Vertex_t *)( ( byte * )&v->weights[v->numWeights] + 12 );
				v = (md4Vertex_t *)( ( byte * )&v->weights[v->numWeights]);
			}

			// find the next surface
			surf = (md4Surface_t *)( (byte *)surf + surf->ofsEnd );
		}

		// find the next LOD
		lod = (md4LOD_t *)( (byte *)lod + lod->ofsEnd );
	}

	return qtrue;
}



qhandle_t R_RegisterMD4(const char *name, model_t *mod)
{
	qboolean	loaded;
	int numLoaded = 0;
	int lod = 0;
	for ( lod = MD3_MAX_LODS - 1 ; lod >= 0 ; lod-- )
    {
		char filename[1024];

		strcpy( filename, name );

		if ( lod != 0 )
        {
			char namebuf[80];

			if ( strrchr( filename, '.' ) ) {
				*strrchr( filename, '.' ) = 0;
			}
			sprintf( namebuf, "_%d.md3", lod );
			strcat( filename, namebuf );
		}

        char* buf = NULL;
		ri.R_ReadFile( filename, &buf );
		if ( !buf ) {
			continue;
		}
		
		int ident = LittleLong(*(unsigned *)buf);
		if ( ident == MD4_IDENT ) {
			loaded = R_LoadMD4( mod, buf, name );
		} 
        /*
        else {
			if ( ident != MD3_IDENT ) {
				ri.Printf (PRINT_WARNING,"RE_RegisterModel: unknown fileid for %s\n", name);
				goto fail;
			}

			loaded = R_LoadMD4( mod, lod, buf, name );
		}
		*/
		ri.FS_FreeFile (buf);

		if ( !loaded ) {
			if ( lod == 0 ) {
				goto fail;
			} else {
				break;
			}
		} else {
			mod->numLods++;
			numLoaded++;
			// if we have a valid model and are biased
			// so that we won't see any higher detail ones,
			// stop loading them
//			if ( lod <= r_lodbias->integer ) {
//				break;
//			}
		}
	}

	if ( numLoaded ) {
		// duplicate into higher lod spots that weren't
		// loaded, in case the user changes r_lodbias on the fly
		for ( lod-- ; lod >= 0 ; lod-- ) {
			mod->numLods++;
			mod->md3[lod] = mod->md3[lod+1];
		}

		return mod->index;
	}

fail:
	// we still keep the model_t around, so if the model name is asked for
	// again, we won't bother scanning the filesystem
	mod->type = MOD_BAD;
	return 0;
}
