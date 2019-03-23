#include "tr_local.h"
#include "R_LoadMD3.h"
#include "R_LoadMDR.h"
#include "tr_model_iqm.h"

#include "RE_RegisterModel.h"

#define	LL(x) x=LittleLong(x)
model_t	*loadmodel;

/*
====================

Loads in a model for the given name

Zero will be returned if the model fails to load.
An entry will be retained for failed models as an
optimization to prevent disk rescanning if they are
asked for again.
====================
*/
qhandle_t RE_RegisterModel( const char *name )
{
	model_t		*mod;

	int			lod;
	int			ident;
	qboolean	loaded;
	qhandle_t	hModel;
	int			numLoaded;

	if ( !name || !name[0] ) {
		ri.Printf( PRINT_ALL, "RE_RegisterModel: NULL name\n" );
		return 0;
	}

	if ( strlen( name ) >= MAX_QPATH ) {
		ri.Printf( PRINT_ALL, "Model name exceeds MAX_QPATH\n" );
		return 0;
	}

	//
	// search the currently loaded models
	//
	for ( hModel = 1 ; hModel < tr.numModels; hModel++ ) {
		mod = tr.models[hModel];
		if ( !strcmp( mod->name, name ) ) {
			if( mod->type == MOD_BAD ) {
				return 0;
			}
			return hModel;
		}
	}

	// allocate a new model_t

	if ( ( mod = R_AllocModel() ) == NULL ) {
		ri.Printf( PRINT_WARNING, "RE_RegisterModel: R_AllocModel() failed for '%s'\n", name);
		return 0;
	}

	// only set the name after the model has been successfully loaded
	Q_strncpyz( mod->name, name, sizeof( mod->name ) );


	// make sure the render thread is stopped
	R_IssuePendingRenderCommands();

	mod->type = MOD_BAD;
	mod->numLods = 0;

	//
	// load the files
	//
	numLoaded = 0;

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
		
		loadmodel = mod;
		
		ident = LittleLong(*(unsigned *)buf);
		if ( ident == MD4_IDENT ) {
			loaded = R_LoadMD4( mod, buf, name );
		} else {
			if ( ident != MD3_IDENT ) {
				ri.Printf (PRINT_WARNING,"RE_RegisterModel: unknown fileid for %s\n", name);
				goto fail;
			}

			loaded = R_LoadMD3( mod, lod, buf, name );
		}
		
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
#ifndef NDEBUG
	else {
		ri.Printf (PRINT_WARNING,"RE_RegisterModel: couldn't load %s\n", name);
	}
#endif

fail:
	// we still keep the model_t around, so if the model name is asked for
	// again, we won't bother scanning the filesystem
	mod->type = MOD_BAD;
	return 0;
}
}
