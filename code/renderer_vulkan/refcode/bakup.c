
extern cvar_t	*r_texturebits;			// number of desired texture bits
										// 0 = use framebuffer depth
										// 16 = use 16-bit textures
										// 32 = use 32-bit textures
										// all else = error


// VULKAN
static struct Vk_Image upload_vk_image(const struct Image_Upload_Data* upload_data, VkBool32 isRepTex)
{
	int w = upload_data->base_level_width;
	int h = upload_data->base_level_height;


	unsigned char* buffer = upload_data->buffer;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	int bytes_per_pixel = 4;

	if (r_texturebits->integer <= 16)
    {

    	VkBool32 has_alpha = qfalse;
	    for (int i = 0; i < w * h; i++) {
		if (upload_data->buffer[i*4 + 3] != 255)  {
			has_alpha = qtrue;
			break;
		}
	    }

		buffer = (unsigned char*) ri.Hunk_AllocateTempMemory( upload_data->buffer_size / 2 );
		format = has_alpha ? VK_FORMAT_B4G4R4A4_UNORM_PACK16 : VK_FORMAT_A1R5G5B5_UNORM_PACK16;
		bytes_per_pixel = 2;
	}
	if (format == VK_FORMAT_A1R5G5B5_UNORM_PACK16) {
		uint16_t* p = (uint16_t*)buffer;
		for (int i = 0; i < upload_data->buffer_size; i += 4, p++) {
			unsigned char r = upload_data->buffer[i+0];
			unsigned char g = upload_data->buffer[i+1];
			unsigned char b = upload_data->buffer[i+2];

			*p = (uint32_t)((b/255.0) * 31.0 + 0.5) |
				((uint32_t)((g/255.0) * 31.0 + 0.5) << 5) |
				((uint32_t)((r/255.0) * 31.0 + 0.5) << 10) |
				(1 << 15);
		}
	} else if (format == VK_FORMAT_B4G4R4A4_UNORM_PACK16) {
		uint16_t* p = (uint16_t*)buffer;
		for (int i = 0; i < upload_data->buffer_size; i += 4, p++) {
			unsigned char r = upload_data->buffer[i+0];
			unsigned char g = upload_data->buffer[i+1];
			unsigned char b = upload_data->buffer[i+2];
			unsigned char a = upload_data->buffer[i+3];

			*p = (uint32_t)((a/255.0) * 15.0 + 0.5) |
				((uint32_t)((r/255.0) * 15.0 + 0.5) << 4) |
				((uint32_t)((g/255.0) * 15.0 + 0.5) << 8) |
				((uint32_t)((b/255.0) * 15.0 + 0.5) << 12);
		}
	}


	struct Vk_Image image = vk_create_image(w, h, format, upload_data->mip_levels, isRepTex);
	vk_upload_image_data(image.handle, w, h, upload_data->mip_levels > 1, buffer, bytes_per_pixel);

	if (bytes_per_pixel == 2)
		ri.Hunk_FreeTempMemory(buffer);

	return image;
}

static void vk_create_image(uint32_t width, uint32_t height, uint32_t mipLevels, VkBool32 repeat_texture, struct Vk_Image* pImg)
{
	// create image
	{
		VkImageCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		desc.pNext = NULL;
		desc.flags = 0;
		desc.imageType = VK_IMAGE_TYPE_2D;
		desc.format = VK_FORMAT_R8G8B8A8_UNORM;
		desc.extent.width = width;
		desc.extent.height = height;
		desc.extent.depth = 1;
		desc.mipLevels = mipLevels;
		desc.arrayLayers = 1;
		desc.samples = VK_SAMPLE_COUNT_1_BIT;
		desc.tiling = VK_IMAGE_TILING_OPTIMAL;
		desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		desc.queueFamilyIndexCount = 0;
		desc.pQueueFamilyIndices = NULL;
		desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_CHECK(qvkCreateImage(vk.device, &desc, NULL, &pImg->handle));
		allocate_and_bind_image_memory(pImg->handle);
	}


	// create image view
	{
		VkImageViewCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		desc.pNext = NULL;
		desc.flags = 0;
		desc.image = pImg->handle;
		desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
		desc.format = VK_FORMAT_R8G8B8A8_UNORM;

        // the components field allows you to swizzle the color channels around
		desc.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // The subresourceRange field describes what the image's purpose is
        // and which part of the image should be accessed. 
		desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		desc.subresourceRange.baseMipLevel = 0;
		desc.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		desc.subresourceRange.baseArrayLayer = 0;
		desc.subresourceRange.layerCount = 1;
		
        VK_CHECK(qvkCreateImageView(vk.device, &desc, NULL, &pImg->view));
	}

	// create associated descriptor set
    // Allocate a descriptor set from the pool. 
    // Note that we have to provide the descriptor set layout that 
    // we defined in the pipeline_layout sample. 
    // This layout describes how the descriptor set is to be allocated.
	{
		VkDescriptorSetAllocateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		desc.pNext = NULL;
		desc.descriptorPool = vk.descriptor_pool;
		desc.descriptorSetCount = 1;
		desc.pSetLayouts = &vk.set_layout;
		
        VK_CHECK(qvkAllocateDescriptorSets(vk.device, &desc, &pImg->descriptor_set));
        ri.Printf(PRINT_ALL, " Allocate Descriptor Sets \n");

        VkBool32 mipmap = (mipLevels > 1) ? VK_TRUE: VK_FALSE;
        
        VkDescriptorImageInfo image_info;
        image_info.sampler = vk_find_sampler(mipmap, repeat_texture);
        image_info.imageView = pImg->view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet descriptor_write;
        descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet = pImg->descriptor_set;
        descriptor_write.dstBinding = 0;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorCount = 1;
        descriptor_write.pNext = NULL;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.pImageInfo = &image_info;
        descriptor_write.pBufferInfo = NULL;
        descriptor_write.pTexelBufferView = NULL;

        qvkUpdateDescriptorSets(vk.device, 1, &descriptor_write, 0, NULL);

        // The above steps essentially copy the VkDescriptorBufferInfo
        // to the descriptor, which is likely in the device memory.
        //
        // This buffer info includes the handle to the uniform buffer
        // as well as the offset and size of the data that is accessed
        // in the uniform buffer. In this case, the uniform buffer 
        // contains only the MVP transform, so the offset is 0 and 
        // the size is the size of the MVP.
        
        
        s_CurrentDescriptorSets[s_CurTmu] = pImg->descriptor_set;
	}
}



unsigned int R_SumOfUsedImages( void )
{
	unsigned int i;

	unsigned int total = 0;
	for ( i = 0; i < tr.numImages; i++ )
    {
		if ( tr.images[i]->frameUsed == tr.frameCount )
        {
			total += tr.images[i]->uploadWidth * tr.images[i]->uploadHeight;
		}
	}

	return total;
}



static void bind_image_with_memory(VkImage image)
{
    VkMemoryRequirements memory_requirements;
    qvkGetImageMemoryRequirements(vk.device, image, &memory_requirements);
    
    // Try to find an existing chunk of sufficient capacity.
    uint32_t mask = (memory_requirements.alignment - 1);


    // ensure that memory region has proper alignment
    uint32_t offset_aligned = (StagImg.used + mask) & (~mask);
    uint32_t needed = offset_aligned + memory_requirements.size; 
    
    // ensure that device local memory is enough
    assert (needed <= IMAGE_CHUNK_SIZE);

    StagImg.used = needed;
    
    // To attach memory to a VkImage object created without the VK_IMAGE_CREATE_DISJOINT_BIT set
    //
    // StagImg.devMemStoreImg is the VkDeviceMemory object describing the device memory to attach.
    // offset_aligned is the start offset of the region of memory which is to be bound to the image. 
    // The number of bytes returned in the VkMemoryRequirements::size member in memory,
    // starting from memoryOffset bytes, will be bound to the specified image.

    VK_CHECK(qvkBindImageMemory(vk.device, image, StagImg.devMemStoreImg, offset_aligned));
    // 	for debug info
    ri.Printf(PRINT_ALL, " StagImg.used = %d \n", StagImg.used);

}


static void R_BindAnimatedImage( textureBundle_t *bundle, uint32_t curTMU )
{
	int		index;

	if ( bundle->isVideoMap ) {
		ri.CIN_RunCinematic(bundle->videoMapHandle);
		ri.CIN_UploadCinematic(bundle->videoMapHandle);
		return;
	}

	if ( bundle->numImageAnimations <= 1 ) {
		updateCurDescriptor( bundle->image[0]->descriptor_set, curTMU);
        //GL_Bind(bundle->image[0]);
		return;
	}

	// it is necessary to do this messy calc to make sure animations line up
	// exactly with waveforms of the same frequency
	index = (int)( tess.shaderTime * bundle->imageAnimationSpeed * FUNCTABLE_SIZE );
	index >>= FUNCTABLE_SIZE2;

	if ( index < 0 ) {
		index = 0;	// may happen with shader time offsets
	}
	index %= bundle->numImageAnimations;

	updateCurDescriptor( bundle->image[ index ]->descriptor_set , curTMU);
    //GL_Bind(bundle->image[ index ], curTMU);

}


static void vk_createImageViewAndDescriptorSet(VkImage h_image, VkImageView* pView)
{
    VkImageViewCreateInfo desc;
    desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    desc.pNext = NULL;
    desc.flags = 0;
    desc.image = h_image;
    desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
    // format is a VkFormat describing the format and type used 
    // to interpret data elements in the image.
    desc.format = VK_FORMAT_R8G8B8A8_UNORM;

    // the components field allows you to swizzle the color channels around
    desc.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    desc.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    desc.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    desc.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // The subresourceRange field describes what the image's purpose is
    // and which part of the image should be accessed. 
    //
    // selecting the set of mipmap levels and array layers to be accessible to the view.
    desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    desc.subresourceRange.baseMipLevel = 0;
    desc.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    desc.subresourceRange.baseArrayLayer = 0;
    desc.subresourceRange.layerCount = 1;
    // Image objects are not directly accessed by pipeline shaders for reading or writing image data.
    // Instead, image views representing contiguous ranges of the image subresources and containing
    // additional metadata are used for that purpose. Views must be created on images of compatible
    // types, and must represent a valid subset of image subresources.
    //
    // Some of the image creation parameters are inherited by the view. In particular, image view
    // creation inherits the implicit parameter usage specifying the allowed usages of the image 
    // view that, by default, takes the value of the corresponding usage parameter specified in
    // VkImageCreateInfo at image creation time.
    //
    // This implicit parameter can be overriden by chaining a VkImageViewUsageCreateInfo structure
    // through the pNext member to VkImageViewCreateInfo.
    VK_CHECK(qvkCreateImageView(vk.device, &desc, NULL, pView));
}



static void vk_upload_image_data(VkImage image, uint32_t width, uint32_t height, const unsigned char* pPixels)
{

    VkBufferImageCopy regions[12];
    uint32_t curLevel = 0;

    uint32_t buffer_size = 0;

    while (1)
    {
	    VkBufferImageCopy region;
	    region.bufferOffset = buffer_size;
	    region.bufferRowLength = 0;
	    region.bufferImageHeight = 0;
	    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	    region.imageSubresource.mipLevel = curLevel;
	    region.imageSubresource.baseArrayLayer = 0;
	    region.imageSubresource.layerCount = 1;
	    region.imageOffset.x = 0;
	    region.imageOffset.y = 0;
	    region.imageOffset.z = 0;

	    region.imageExtent.width = width;
	    region.imageExtent.height = height;
	    region.imageExtent.depth = 1;

	    regions[curLevel] = region;
	    curLevel++;

	    buffer_size += width * height * 4;

	    if ((width == 1) && (height == 1))
		    break;

	    width >>= 1;
	    if (width == 0) 
		    width = 1;

	    height >>= 1;
	    if (height == 0)
		    height = 1;
    }

    //max mipmap buffer size
    assert(buffer_size <= StagImg.buf_size);

    void* data;
    
    VK_CHECK(qvkMapMemory(vk.device, StagImg.devMemMappable, 0, VK_WHOLE_SIZE, 0, &data));

    memcpy(data, pPixels, buffer_size);

    qvkUnmapMemory(vk.device, StagImg.devMemMappable);
    
    
    vk_stagBufferToDeviceLocalMem(image ,regions ,curLevel);
}


static void vk_uploadSingleImage(VkImage image, uint32_t width, uint32_t height, const unsigned char* pPixels)
{

    VkBufferImageCopy region;
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;

    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;


    const uint32_t buffer_size = width * height * 4;

    void* data;
    VK_CHECK(qvkMapMemory(vk.device, StagImg.devMemMappable, 0, VK_WHOLE_SIZE, 0, &data));
    memcpy(data, pPixels, buffer_size);
    qvkUnmapMemory(vk.device, StagImg.devMemMappable);


    vk_stagBufferToDeviceLocalMem(image, &region, 1);
}


static void get_mvp_transform(float* mvp, const int isProj2D)
{
	if (isProj2D)
    {
		float mvp0 = 2.0f / glConfig.vidWidth;
		float mvp5 = 2.0f / glConfig.vidHeight;

		mvp[0]  =  mvp0; mvp[1]  =  0.0f; mvp[2]  = 0.0f; mvp[3]  = 0.0f;
		mvp[4]  =  0.0f; mvp[5]  =  mvp5; mvp[6]  = 0.0f; mvp[7]  = 0.0f;
		mvp[8]  =  0.0f; mvp[9]  =  0.0f; mvp[10] = 1.0f; mvp[11] = 0.0f;
		mvp[12] = -1.0f; mvp[13] = -1.0f; mvp[14] = 0.0f; mvp[15] = 1.0f;
	}
    else
    {
		// update q3's proj matrix (opengl) to vulkan conventions: z - [0, 1] instead of [-1, 1] and invert y direction
		
        MatrixMultiply4x4_SSE(s_modelview_matrix, backEnd.viewParms.projectionMatrix, mvp);
	}
}


model_t* R_AllocModel( void )
{

    ri.Printf( PRINT_ALL, "Allocate Memory for model. \n");

	if ( tr.numModels == MAX_MOD_KNOWN )
    {
        ri.Printf(PRINT_WARNING, "R_AllocModel: MAX_MOD_KNOWN.\n");
		return NULL;
	}

	model_t* mod = ri.Hunk_Alloc( sizeof( model_t ), h_low );
	mod->index = tr.numModels;
	tr.models[tr.numModels] = mod;
	tr.numModels++;

	return mod;
}






	if( ext != NULL )
	{
		uint32_t i;
		// Look for the correct loader and use it
		for( i = 0; i < numModelLoaders; i++ )
		{
			if( !Q_stricmp( ext, modelLoaders[ i ].ext ) )
			{
				// Load
				hModel = modelLoaders[ i ].ModelLoader( localName, mod );
				break;
			}
		}

		// A loader was found
		if( i < numModelLoaders )
		{
			if( !hModel )
			{
				// Loader failed, most likely because the file isn't there;
				// try again without the extension
				orgNameFailed = qtrue;
				orgLoader = i;
				stripExtension( name, localName, MAX_QPATH );
			}
			else
			{
				// Something loaded
				return mod->index;
			}
		}
		else
		{
			ri.Printf( PRINT_WARNING, "RegisterModel: %s not find \n ", name);
		}
	}


static int R_ComputeLOD( trRefEntity_t *ent )
{
    ri.Printf(PRINT_ALL, "\n------ R_ComputeLOD ------\n");
	float flod;
	int lod;

	if ( tr.currentModel->numLods < 2 )
	{
		// model has only 1 LOD level, skip computations and bias
		lod = 0;
	}
	else
	{
		// multiple LODs exist, so compute projected bounding sphere
		// and use that as a criteria for selecting LOD
	    float radius;

		if(tr.currentModel->type == MOD_MDR)
		{
			mdrHeader_t* mdr = (mdrHeader_t *) tr.currentModel->modelData;
			int frameSize = (size_t) (&((mdrFrame_t *)0)->bones[mdr->numBones]);
			mdrFrame_t* mdrframe = (mdrFrame_t *) ((unsigned char *) mdr + mdr->ofsFrames + frameSize * ent->e.frame);
			
			radius = RadiusFromBounds(mdrframe->bounds[0], mdrframe->bounds[1]);
		}
		else
		{
			md3Frame_t* frame = (md3Frame_t *) (( ( unsigned char * ) tr.currentModel->md3[0] ) + tr.currentModel->md3[0]->ofsFrames);

			frame += ent->e.frame;

			radius = RadiusFromBounds( frame->bounds[0], frame->bounds[1] );
		}

        float projectedRadius = ProjectRadius( radius, ent->e.origin, tr.viewParms.projectionMatrix);
		
        if ( projectedRadius != 0 )
		{
			float lodscale = r_lodscale->value;
			if (lodscale > 20)
                lodscale = 20;
			flod = 1.0f - projectedRadius * lodscale;
		}
		else
		{
			// object intersects near view plane, e.g. view weapon
			flod = 0;
		}

		flod *= tr.currentModel->numLods;
		
        lod = (int)(flod);

		if ( lod < 0 )
		{
			lod = 0;
		}
		else if ( lod >= tr.currentModel->numLods )
		{
			lod = tr.currentModel->numLods - 1;
		}
	}

	lod += r_lodbias->integer;
	
	if ( lod >= tr.currentModel->numLods )
		lod = tr.currentModel->numLods - 1;
    else if ( lod < 0 )
		lod = 0;

	return lod;
}


/*
=================
RB_BeginDrawingView

Any mirrored or portaled views have already been drawn, so prepare
to actually render the visible surfaces for this view
=================

void RB_BeginDrawingView (void)
{
	// we will need to change the projection matrix before drawing
	// 2D images again
	backEnd.projection2D = qfalse;

	//
	// set the modelview matrix for the viewer
	//

	// ensures that depth writes are enabled for the depth clear
    if(r_fastsky->integer && !( backEnd.refdef.rd.rdflags & RDF_NOWORLDMODEL ))
    {
        #ifndef NDEBUG
        static const float fast_sky_color[4] = { 0.8f, 0.7f, 0.4f, 1.0f };
        #else
        static const float fast_sky_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        #endif
        vk_clearColorAttachments(fast_sky_color);
    }
    

	// VULKAN
    vk_clearDepthStencilAttachments();

	if ( ( backEnd.refdef.rd.rdflags & RDF_HYPERSPACE ) )
	{
		//RB_Hyperspace();
        // A player has predicted a teleport, but hasn't arrived yet
        const float c = ( backEnd.refdef.rd.time & 255 ) / 255.0f;
        const float color[4] = { c, c, c, 1 };

        // so short, do we really need this?
	    vk_clearColorAttachments(color);

	    backEnd.isHyperspace = qtrue;
	}
	else
	{
		backEnd.isHyperspace = qfalse;
	}
}
*/

/*
void vk_clear_attachments(VkBool32 clear_depth_stencil, VkBool32 clear_color, float* color)
{

	if (!clear_depth_stencil && !clear_color)
		return;

	VkClearAttachment attachments[2];
	uint32_t attachment_count = 0;

	if (clear_depth_stencil)
    {
		attachments[0].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		attachments[0].clearValue.depthStencil.depth = 1.0f;

		if (r_shadows->integer == 2) {
			attachments[0].aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			attachments[0].clearValue.depthStencil.stencil = 0;
		}
		attachment_count = 1;
	}

	if (clear_color)
    {
		attachments[attachment_count].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		attachments[attachment_count].colorAttachment = 0;
		attachments[attachment_count].clearValue.color.float32[0] = color[0];
  		attachments[attachment_count].clearValue.color.float32[1] = color[1];
		attachments[attachment_count].clearValue.color.float32[2] = color[2];
		attachments[attachment_count].clearValue.color.float32[3] = color[3];
		attachment_count++;
	}

	VkClearRect clear_rect[2];
	clear_rect[0].rect = get_scissor_rect();
	clear_rect[0].baseArrayLayer = 0;
	clear_rect[0].layerCount = 1;
	int rect_count = 1;

	// Split viewport rectangle into two non-overlapping rectangles.
	// It's a HACK to prevent Vulkan validation layer's performance warning:
	//		"vkCmdClearAttachments() issued on command buffer object XXX prior to any Draw Cmds.
	//		 It is recommended you use RenderPass LOAD_OP_CLEAR on Attachments prior to any Draw."
	// 
	// NOTE: we don't use LOAD_OP_CLEAR for color attachment when we begin renderpass
	// since at that point we don't know whether we need color buffer clear (usually we don't).
	if (clear_color) {
		uint32_t h = clear_rect[0].rect.extent.height / 2;
		clear_rect[0].rect.extent.height = h;
		clear_rect[1] = clear_rect[0];
		clear_rect[1].rect.offset.y = h;
		rect_count = 2;
	}

	qvkCmdClearAttachments(vk.command_buffer, attachment_count, attachments, rect_count, clear_rect);
}
*/


void vk_createGeometryBuffers(void)
{

    // The buffer has been created, but it doesn't actually have any memory
    // assigned to it yet.

    VkMemoryRequirements vb_memory_requirements;
    qvkGetBufferMemoryRequirements(vk.device, shadingDat.vertex_buffer, &vb_memory_requirements);

    VkMemoryRequirements ib_memory_requirements;
    qvkGetBufferMemoryRequirements(vk.device, shadingDat.index_buffer, &ib_memory_requirements);

    const VkDeviceSize mask = (ib_memory_requirements.alignment - 1);

    const VkDeviceSize idxBufOffset = (vb_memory_requirements.size + mask) & (~mask);

    uint32_t memory_type_bits = vb_memory_requirements.memoryTypeBits & ib_memory_requirements.memoryTypeBits;

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.allocationSize = idxBufOffset + ib_memory_requirements.size;
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with
    // this type can be mapped for host access using vkMapMemory.
    //
    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache
    // management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges
    // are not needed to flush host writes to the device or make device writes visible
    // to the host, respectively.
    alloc_info.memoryTypeIndex = find_memory_type(memory_type_bits, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    ri.Printf(PRINT_ALL, " Allocate device memory: shadingDat.geometry_buffer_memory(%ld bytes). \n",
            alloc_info.allocationSize);

    VK_CHECK(qvkAllocateMemory(vk.device, &alloc_info, NULL, &shadingDat.geometry_buffer_memory));

    // To attach memory to a buffer object
    // vk.device is the logical device that owns the buffer and memory.
    // vertex_buffer/index_buffer is the buffer to be attached to memory.
    // 
    // geometry_buffer_memory is a VkDeviceMemory object describing the 
    // device memory to attach.
    // 
    // idxBufOffset is the start offset of the region of memory 
    // which is to be bound to the buffer.
    // 
    // The number of bytes returned in the VkMemoryRequirements::size member
    // in memory, starting from memoryOffset bytes, will be bound to the 
    // specified buffer.
    qvkBindBufferMemory(vk.device, shadingDat.vertex_buffer, shadingDat.geometry_buffer_memory, 0);
    qvkBindBufferMemory(vk.device, shadingDat.index_buffer, shadingDat.geometry_buffer_memory, idxBufOffset);

    // Host Access to Device Memory Objects
    // Memory objects created with vkAllocateMemory are not directly host accessible.
    // Memory objects created with the memory property VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    // are considered mappable. 
    // Memory objects must be mappable in order to be successfully mapped on the host
    // VK_WHOLE_SIZE: map from offset to the end of the allocation.
    // &data points to a pointer in which is returned a host-accessible pointer
    // to the beginning of the mapped range. This pointer minus offset must be
    // aligned to at least VkPhysicalDeviceLimits::minMemoryMapAlignment.
    void* data;
    VK_CHECK(qvkMapMemory(vk.device, shadingDat.geometry_buffer_memory, 0, VK_WHOLE_SIZE, 0, &data));
    shadingDat.vertex_buffer_ptr = (unsigned char*)data;
    shadingDat.index_buffer_ptr = (unsigned char*)data + idxBufOffset;
}


static void vk_read_pixels(unsigned char* buffer)
{

    ri.Printf(PRINT_ALL, " vk_read_pixels() \n\n");

	qvkDeviceWaitIdle(vk.device);

	// Create image in host visible memory to serve as a destination for framebuffer pixels.
	VkImageCreateInfo desc;
	desc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	desc.pNext = NULL;
	desc.flags = 0;
	desc.imageType = VK_IMAGE_TYPE_2D;
	desc.format = VK_FORMAT_R8G8B8A8_UNORM;
	desc.extent.width = glConfig.vidWidth;
	desc.extent.height = glConfig.vidHeight;
	desc.extent.depth = 1;
	desc.mipLevels = 1;
	desc.arrayLayers = 1;
	desc.samples = VK_SAMPLE_COUNT_1_BIT;
	desc.tiling = VK_IMAGE_TILING_LINEAR;
	desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	desc.queueFamilyIndexCount = 0;
	desc.pQueueFamilyIndices = NULL;
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image;
	VK_CHECK(qvkCreateImage(vk.device, &desc, NULL, &image));

	VkMemoryRequirements memory_requirements;
	qvkGetImageMemoryRequirements(vk.device, image, &memory_requirements);
	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = NULL;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
    VkDeviceMemory memory;
	VK_CHECK(qvkAllocateMemory(vk.device, &alloc_info, NULL, &memory));
	VK_CHECK(qvkBindImageMemory(vk.device, image, memory, 0));

  
    {

        VkCommandBufferAllocateInfo alloc_info;
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.commandPool = vk.command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer cmdBuf;
        VK_CHECK(qvkAllocateCommandBuffers(vk.device, &alloc_info, &cmdBuf));

        VkCommandBufferBeginInfo begin_info;
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.pNext = NULL;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = NULL;

        VK_CHECK(qvkBeginCommandBuffer(cmdBuf, &begin_info));

        record_image_layout_transition(cmdBuf, 
                vk.swapchain_images_array[vk.idx_swapchain_image], 
                VK_IMAGE_ASPECT_COLOR_BIT, VK_ACCESS_MEMORY_READ_BIT, 
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_TRANSFER_READ_BIT, 
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        record_image_layout_transition(cmdBuf, 
                image, VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL);

        VK_CHECK(qvkEndCommandBuffer(cmdBuf));

        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.waitSemaphoreCount = 0;
        submit_info.pWaitSemaphores = NULL;
        submit_info.pWaitDstStageMask = NULL;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmdBuf;
        submit_info.signalSemaphoreCount = 0;
        submit_info.pSignalSemaphores = NULL;

        VK_CHECK(qvkQueueSubmit(vk.queue, 1, &submit_info, VK_NULL_HANDLE));
        VK_CHECK(qvkQueueWaitIdle(vk.queue));
        qvkFreeCommandBuffers(vk.device, vk.command_pool, 1, &cmdBuf);
    }


    //////////////////////////////////////////////////////////////////////
	
    // Check if we can use vkCmdBlitImage for the given 
    // source and destination image formats.
	VkBool32 blit_enabled = 1;
	{
		VkFormatProperties formatProps;
		qvkGetPhysicalDeviceFormatProperties(
                vk.physical_device, vk.surface_format.format, &formatProps );

		if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0)
			blit_enabled = 0;

		qvkGetPhysicalDeviceFormatProperties(
                vk.physical_device, VK_FORMAT_R8G8B8A8_UNORM, &formatProps );

		if ((formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) == 0)
			blit_enabled = 0;
	}

	if (blit_enabled)
    {
        ri.Printf(PRINT_ALL, " blit_enabled \n\n");

        VkCommandBufferAllocateInfo alloc_info;
    	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    	alloc_info.pNext = NULL;
    	alloc_info.commandPool = vk.command_pool;
    	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    	alloc_info.commandBufferCount = 1;

    	VkCommandBuffer command_buffer;
    	VK_CHECK(qvkAllocateCommandBuffers(vk.device, &alloc_info, &command_buffer));

    	VkCommandBufferBeginInfo begin_info;
    	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	begin_info.pNext = NULL;
    	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    	begin_info.pInheritanceInfo = NULL;

    	VK_CHECK(qvkBeginCommandBuffer(command_buffer, &begin_info));
        //	recorder(command_buffer);
        {
            VkImageBlit region;
            region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.srcSubresource.mipLevel = 0;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount = 1;

            region.srcOffsets[0].x = 0;
            region.srcOffsets[0].y = 0;
            region.srcOffsets[0].z = 0;

            region.srcOffsets[1].x = glConfig.vidWidth;
            region.srcOffsets[1].y = glConfig.vidHeight;
            region.srcOffsets[1].z = 1;

            region.dstSubresource = region.srcSubresource;
            region.dstOffsets[0] = region.srcOffsets[0];
            region.dstOffsets[1] = region.srcOffsets[1];

            qvkCmdBlitImage(command_buffer,
                    vk.swapchain_images_array[vk.idx_swapchain_image], 
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                    VK_IMAGE_LAYOUT_GENERAL, 1, &region, VK_FILTER_NEAREST);
        }

	    VK_CHECK(qvkEndCommandBuffer(command_buffer));

    	VkSubmitInfo submit_info;
    	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    	submit_info.pNext = NULL;
    	submit_info.waitSemaphoreCount = 0;
    	submit_info.pWaitSemaphores = NULL;
    	submit_info.pWaitDstStageMask = NULL;
    	submit_info.commandBufferCount = 1;
    	submit_info.pCommandBuffers = &command_buffer;
    	submit_info.signalSemaphoreCount = 0;
    	submit_info.pSignalSemaphores = NULL;

    	VK_CHECK(qvkQueueSubmit(vk.queue, 1, &submit_info, VK_NULL_HANDLE));
    	VK_CHECK(qvkQueueWaitIdle(vk.queue));
    	qvkFreeCommandBuffers(vk.device, vk.command_pool, 1, &command_buffer);   
	}
    else
    {

	    VkCommandBufferAllocateInfo alloc_info;
    	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    	alloc_info.pNext = NULL;
    	alloc_info.commandPool = vk.command_pool;
    	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    	alloc_info.commandBufferCount = 1;

	    VkCommandBuffer command_buffer;
    	VK_CHECK(qvkAllocateCommandBuffers(vk.device, &alloc_info, &command_buffer));

	    VkCommandBufferBeginInfo begin_info;
    	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	begin_info.pNext = NULL;
    	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    	begin_info.pInheritanceInfo = NULL;

	    VK_CHECK(qvkBeginCommandBuffer(command_buffer, &begin_info));
	    {
            ////   recorder(command_buffer);
            VkImageCopy region;
            region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.srcSubresource.mipLevel = 0;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount = 1;
            region.srcOffset.x = 0;
            region.srcOffset.y = 0;
            region.srcOffset.z = 0;
            region.dstSubresource = region.srcSubresource;
            region.dstOffset = region.srcOffset;
            region.extent.width = glConfig.vidWidth;
            region.extent.height = glConfig.vidHeight;
            region.extent.depth = 1;

            qvkCmdCopyImage(command_buffer, 
                    vk.swapchain_images_array[vk.idx_swapchain_image], 
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		
        }
        VK_CHECK(qvkEndCommandBuffer(command_buffer));

	    VkSubmitInfo submit_info;
    	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    	submit_info.pNext = NULL;
    	submit_info.waitSemaphoreCount = 0;
    	submit_info.pWaitSemaphores = NULL;
    	submit_info.pWaitDstStageMask = NULL;
    	submit_info.commandBufferCount = 1;
    	submit_info.pCommandBuffers = &command_buffer;
    	submit_info.signalSemaphoreCount = 0;
    	submit_info.pSignalSemaphores = NULL;

    	VK_CHECK(qvkQueueSubmit(vk.queue, 1, &submit_info, VK_NULL_HANDLE));
        VK_CHECK(qvkQueueWaitIdle(vk.queue));
    	qvkFreeCommandBuffers(vk.device, vk.command_pool, 1, &command_buffer);
	}



	// Copy data from destination image to memory buffer.
	VkImageSubresource subresource;
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;
	VkSubresourceLayout layout;
	qvkGetImageSubresourceLayout(vk.device, image, &subresource, &layout);


    // Memory objects created with vkAllocateMemory are not directly host accessible.
    // Memory objects created with the memory property 
    // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT are considered mappable. 
    // Memory objects must be mappable in order to be successfully 
    // mapped on the host. 
    // To retrieve a host virtual address pointer to 
    // a region of a mappable memory object
    unsigned char* data;
    VK_CHECK(qvkMapMemory(vk.device, memory, 0, VK_WHOLE_SIZE, 0, (void**)&data));
	data += layout.offset;

	unsigned char* buffer_ptr = buffer + glConfig.vidWidth * (glConfig.vidHeight - 1) * 4;
	
    int j = 0;
    for (j = 0; j < glConfig.vidHeight; j++)
    {
		memcpy(buffer_ptr, data, glConfig.vidWidth * 4);
		buffer_ptr -= glConfig.vidWidth * 4;
		data += layout.rowPitch;
	}


	if (!blit_enabled)
    {
        ri.Printf(PRINT_ALL, "blit_enabled = 0 \n\n");

		//auto fmt = vk.surface_format.format;
		VkBool32 swizzle_components = 
            (vk.surface_format.format == VK_FORMAT_B8G8R8A8_SRGB ||
             vk.surface_format.format == VK_FORMAT_B8G8R8A8_UNORM ||
             vk.surface_format.format == VK_FORMAT_B8G8R8A8_SNORM);
        
		if (swizzle_components)
        {
			buffer_ptr = buffer;
            unsigned int i = 0;
            unsigned int pixels = glConfig.vidWidth * glConfig.vidHeight;
			for (i = 0; i < pixels; i++)
            {
				unsigned char tmp = buffer_ptr[0];
				buffer_ptr[0] = buffer_ptr[2];
				buffer_ptr[2] = tmp;
				buffer_ptr += 4;
			}
		}
	}
	qvkUnmapMemory(vk.device, memory);
	qvkFreeMemory(vk.device, memory, NULL);

    qvkDestroyImage(vk.device, image, NULL);
}


/*
=================
Setup that culling frustum planes for the current view
=================
*/
static void R_SetupFrustum (viewParms_t * const pViewParams)
{
	
    {
        float ang = pViewParams->fovX * (M_PI / 360.0f);
        float xs = sin( ang );
        float xc = cos( ang );

        float temp1[3];
        float temp2[3];

        VectorScale( pViewParams->or.axis[0], xs, temp1 );
        VectorScale( pViewParams->or.axis[1], xc, temp2);

        VectorAdd(temp1, temp2, pViewParams->frustum[0].normal);
        VectorSubtract(temp1, temp2, pViewParams->frustum[1].normal);
    }
    // VectorMA( tr.viewParms.frustum[0].normal, xc, tr.viewParms.or.axis[1], tr.viewParms.frustum[0].normal );
	//VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[1].normal );
	//VectorMA( tr.viewParms.frustum[1].normal, -xc, tr.viewParms.or.axis[1], tr.viewParms.frustum[1].normal );
   
    {
        float ang = pViewParams->fovY * (M_PI / 360.0f);
        float xs = sin( ang );
        float xc = cos( ang );
        float temp1[3];
        float temp2[3];

        VectorScale( pViewParams->or.axis[0], xs, temp1);
        VectorScale( pViewParams->or.axis[2], xc, temp2);

        VectorAdd(temp1, temp2, pViewParams->frustum[2].normal);
        VectorSubtract(temp1, temp2, pViewParams->frustum[3].normal);
    }

	//VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[2].normal );
	//VectorMA( tr.viewParms.frustum[2].normal, xc, tr.viewParms.or.axis[2], tr.viewParms.frustum[2].normal );
	//VectorScale( tr.viewParms.or.axis[0], xs, tr.viewParms.frustum[3].normal );
	//VectorMA( tr.viewParms.frustum[3].normal, -xc, tr.viewParms.or.axis[2], tr.viewParms.frustum[3].normal );
	
    uint32_t i = 0;
	for (i=0; i < 4; i++)
    {
		pViewParams->frustum[i].type = PLANE_NON_AXIAL;
		pViewParams->frustum[i].dist = DotProduct (pViewParams->or.origin, pViewParams->frustum[i].normal);
		SetPlaneSignbits( &pViewParams->frustum[i] );
	}
}
