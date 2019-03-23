#include <string.h>
#include <stdlib.h>

#include "VKimpl.h"
#include "vk_instance.h"
#include "tr_globals.h"
#include "vk_image.h"
#include "vk_instance.h"
#include "vk_shade_geometry.h"
#include "vk_pipelines.h"
#include "vk_frame.h"
#include "vk_shaders.h"
#include "vk_depth_attachment.h"

struct Vk_Instance vk;

//
// Vulkan API functions used by the renderer.
//
PFN_vkGetInstanceProcAddr						qvkGetInstanceProcAddr;

PFN_vkCreateInstance							qvkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties		qvkEnumerateInstanceExtensionProperties;

PFN_vkCreateDevice								qvkCreateDevice;
PFN_vkDestroyInstance							qvkDestroyInstance;
PFN_vkEnumerateDeviceExtensionProperties		qvkEnumerateDeviceExtensionProperties;
PFN_vkEnumeratePhysicalDevices					qvkEnumeratePhysicalDevices;
PFN_vkGetDeviceProcAddr							qvkGetDeviceProcAddr;
PFN_vkGetPhysicalDeviceFeatures					qvkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties			qvkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceMemoryProperties			qvkGetPhysicalDeviceMemoryProperties;
PFN_vkGetPhysicalDeviceProperties				qvkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties	qvkGetPhysicalDeviceQueueFamilyProperties;


PFN_vkDestroySurfaceKHR							qvkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	qvkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		qvkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	qvkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR		qvkGetPhysicalDeviceSurfaceSupportKHR;

#ifndef NDEBUG
PFN_vkCreateDebugReportCallbackEXT				qvkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT				qvkDestroyDebugReportCallbackEXT;
#endif

PFN_vkAllocateCommandBuffers					qvkAllocateCommandBuffers;
PFN_vkAllocateDescriptorSets					qvkAllocateDescriptorSets;
PFN_vkAllocateMemory							qvkAllocateMemory;
PFN_vkBeginCommandBuffer						qvkBeginCommandBuffer;
PFN_vkBindBufferMemory							qvkBindBufferMemory;
PFN_vkBindImageMemory							qvkBindImageMemory;
PFN_vkCmdBeginRenderPass						qvkCmdBeginRenderPass;
PFN_vkCmdBindDescriptorSets						qvkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer						qvkCmdBindIndexBuffer;
PFN_vkCmdBindPipeline							qvkCmdBindPipeline;
PFN_vkCmdBindVertexBuffers						qvkCmdBindVertexBuffers;
PFN_vkCmdBlitImage								qvkCmdBlitImage;
PFN_vkCmdClearAttachments						qvkCmdClearAttachments;
PFN_vkCmdCopyBufferToImage						qvkCmdCopyBufferToImage;
PFN_vkCmdCopyImage								qvkCmdCopyImage;
PFN_vkCmdCopyImageToBuffer                      qvkCmdCopyImageToBuffer;
PFN_vkCmdDraw									qvkCmdDraw;
PFN_vkCmdDrawIndexed							qvkCmdDrawIndexed;
PFN_vkCmdEndRenderPass							qvkCmdEndRenderPass;
PFN_vkCmdPipelineBarrier						qvkCmdPipelineBarrier;
PFN_vkCmdPushConstants							qvkCmdPushConstants;
PFN_vkCmdSetDepthBias							qvkCmdSetDepthBias;
PFN_vkCmdSetScissor								qvkCmdSetScissor;
PFN_vkCmdSetViewport							qvkCmdSetViewport;
PFN_vkCreateBuffer								qvkCreateBuffer;
PFN_vkCreateCommandPool							qvkCreateCommandPool;
PFN_vkCreateDescriptorPool						qvkCreateDescriptorPool;
PFN_vkCreateDescriptorSetLayout					qvkCreateDescriptorSetLayout;
PFN_vkCreateFence								qvkCreateFence;
PFN_vkCreateFramebuffer							qvkCreateFramebuffer;
PFN_vkCreateGraphicsPipelines					qvkCreateGraphicsPipelines;
PFN_vkCreateImage								qvkCreateImage;
PFN_vkCreateImageView							qvkCreateImageView;
PFN_vkCreatePipelineLayout						qvkCreatePipelineLayout;
PFN_vkCreateRenderPass							qvkCreateRenderPass;
PFN_vkCreateSampler								qvkCreateSampler;
PFN_vkCreateSemaphore							qvkCreateSemaphore;
PFN_vkCreateShaderModule						qvkCreateShaderModule;
PFN_vkDestroyBuffer								qvkDestroyBuffer;
PFN_vkDestroyCommandPool						qvkDestroyCommandPool;
PFN_vkDestroyDescriptorPool						qvkDestroyDescriptorPool;
PFN_vkDestroyDescriptorSetLayout				qvkDestroyDescriptorSetLayout;
PFN_vkDestroyDevice								qvkDestroyDevice;
PFN_vkDestroyFence								qvkDestroyFence;
PFN_vkDestroyFramebuffer						qvkDestroyFramebuffer;
PFN_vkDestroyImage								qvkDestroyImage;
PFN_vkDestroyImageView							qvkDestroyImageView;
PFN_vkDestroyPipeline							qvkDestroyPipeline;
PFN_vkDestroyPipelineLayout						qvkDestroyPipelineLayout;
PFN_vkDestroyRenderPass							qvkDestroyRenderPass;
PFN_vkDestroySampler							qvkDestroySampler;
PFN_vkDestroySemaphore							qvkDestroySemaphore;
PFN_vkDestroyShaderModule						qvkDestroyShaderModule;
PFN_vkDeviceWaitIdle							qvkDeviceWaitIdle;
PFN_vkEndCommandBuffer							qvkEndCommandBuffer;
PFN_vkFreeCommandBuffers						qvkFreeCommandBuffers;
PFN_vkFreeDescriptorSets						qvkFreeDescriptorSets;
PFN_vkFreeMemory								qvkFreeMemory;
PFN_vkGetBufferMemoryRequirements				qvkGetBufferMemoryRequirements;
PFN_vkGetDeviceQueue							qvkGetDeviceQueue;
PFN_vkGetImageMemoryRequirements				qvkGetImageMemoryRequirements;
PFN_vkGetImageSubresourceLayout					qvkGetImageSubresourceLayout;
PFN_vkMapMemory									qvkMapMemory;
PFN_vkUnmapMemory                               qvkUnmapMemory;
PFN_vkQueueSubmit								qvkQueueSubmit;
PFN_vkQueueWaitIdle								qvkQueueWaitIdle;
PFN_vkResetDescriptorPool						qvkResetDescriptorPool;
PFN_vkResetFences								qvkResetFences;
PFN_vkUpdateDescriptorSets						qvkUpdateDescriptorSets;
PFN_vkWaitForFences								qvkWaitForFences;
PFN_vkAcquireNextImageKHR						qvkAcquireNextImageKHR;
PFN_vkCreateSwapchainKHR						qvkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR						qvkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR						qvkGetSwapchainImagesKHR;
PFN_vkQueuePresentKHR							qvkQueuePresentKHR;



#ifndef NDEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL vk_DebugCallback(
        VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type,
        uint64_t object, size_t location, int32_t message_code, 
        const char* layer_prefix, const char* message, void* user_data )
{
    ri.Printf(PRINT_WARNING, "%s\n", message);
	return VK_FALSE;
}


static void vk_createDebugCallback( PFN_vkDebugReportCallbackEXT qvkDebugCB)
{
    ri.Printf( PRINT_ALL, " vk_createDebugCallback() \n" ); 
    
    VkDebugReportCallbackCreateInfoEXT desc;
    desc.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    desc.pNext = NULL;
    desc.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT;
    desc.pfnCallback = qvkDebugCB;
    desc.pUserData = NULL;

    VK_CHECK(qvkCreateDebugReportCallbackEXT(vk.instance, &desc, NULL, &vk.h_debugCB));
}

#endif


static void vk_createInstance(void)
{
    // There is no global state in Vulkan and all per-application state
    // is stored in a VkInstance object. Creating a VkInstance object 
    // initializes the Vulkan library and allows the application to pass
    // information about itself to the implementation.
    ri.Printf(PRINT_ALL, " Creating instance: vk.instance\n");
	
    // The version of Vulkan that is supported by an instance may be 
    // different than the version of Vulkan supported by a device or
    // physical device. Because Vulkan 1.0 implementations may fail
    // with VK_ERROR_INCOMPATIBLE_DRIVER, applications should determine
    // the version of Vulkan available before calling vkCreateInstance.
    // If the vkGetInstanceProcAddr returns NULL for vkEnumerateInstanceVersion,
    // it is a Vulkan 1.0 implementation. Otherwise, the application can
    // call vkEnumerateInstanceVersion to determine the version of Vulkan.

    VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
	appInfo.pApplicationName = "OpenArena";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VulkanArena";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    // apiVersion must be the highest version of Vulkan that the
    // application is designed to use, encoded as described in the
    // API Version Numbers and Semantics section. The patch version
    // number specified in apiVersion is ignored when creating an 
    // instance object. Only the major and minor versions of the 
    // instance must match those requested in apiVersion.
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);


	VkInstanceCreateInfo instanceCreateInfo;
	
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // pNext is NULL or a pointer to an extension-specific structure.
	instanceCreateInfo.pNext = NULL;
    // flags is reserved for future use.
    instanceCreateInfo.flags = 0;
    // pApplicationInfo is NULL or a pointer to an instance of
    // VkApplicationInfo. If not NULL, this information helps 
    // implementations recognize behavior inherent to classes
    // of applications.
    instanceCreateInfo.pApplicationInfo = &appInfo;

    
	// check extensions availability
    //
    // Extensions may define new Vulkan commands, structures, and enumerants.
    // For compilation purposes, the interfaces defined by registered extensions,
    // including new structures and enumerants as well as function pointer types
    // for new commands, are defined in the Khronos-supplied vulkan_core.h 
    // together with the core API. However, commands defined by extensions may
    // not be available for static linking - in which case function pointers to
    // these commands should be queried at runtime as described in Command Function Pointers.
    // Extensions may be provided by layers as well as by a Vulkan implementation.
    //
    // check extensions availability
	uint32_t nInsExt = 0;
    // To retrieve a list of supported extensions before creating an instance
	VK_CHECK( qvkEnumerateInstanceExtensionProperties( NULL, &nInsExt, NULL) );

    assert(nInsExt > 0);

    ri.Printf(PRINT_ALL, "--- Total %d instance extensions. --- \n", nInsExt);

    VkExtensionProperties *pInsExt = (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * nInsExt);
    const char** ppInstanceExt = malloc( sizeof(char *) * (nInsExt) );

    VK_CHECK(qvkEnumerateInstanceExtensionProperties( NULL, &nInsExt, pInsExt));

    uint32_t i = 0;

    uint32_t indicator = 0;

    for (i = 0; i < nInsExt; i++)
    {    
        ri.Printf(PRINT_ALL, "%s\n", pInsExt[i].extensionName );
        unsigned int len = strlen(pInsExt[i].extensionName);
        memcpy(glConfig.extensions_string + indicator, pInsExt[i].extensionName, len);
        indicator += len;
        glConfig.extensions_string[indicator++] = ' ';

        ppInstanceExt[i] = pInsExt[i].extensionName;
    }
    
    instanceCreateInfo.enabledExtensionCount = nInsExt;
	instanceCreateInfo.ppEnabledExtensionNames = ppInstanceExt;

#ifndef NDEBUG
    ri.Printf(PRINT_ALL, "Using VK_LAYER_LUNARG_standard_validation\n");

    const char* const validation_layer_name = "VK_LAYER_LUNARG_standard_validation";    
    instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledLayerNames = &validation_layer_name;
#else
    instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.ppEnabledLayerNames = NULL;
#endif


    VkResult e = qvkCreateInstance(&instanceCreateInfo, NULL, &vk.instance);
    if(e == VK_SUCCESS)
    {
        ri.Printf(PRINT_ALL, "--- Vulkan create instance success! ---\n\n");
    }
    else if (e == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		// The requested version of Vulkan is not supported by the driver 
		// or is otherwise incompatible for implementation-specific reasons.
        ri.Error(ERR_FATAL, 
            "The requested version of Vulkan is not supported by the driver.\n" );
    }
    else if (e == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        ri.Error(ERR_FATAL, "Cannot find a specified extension library.\n");
    }
    else 
    {
        ri.Error(ERR_FATAL, "%d, returned by qvkCreateInstance.\n", e);
    }
    
    free(pInsExt);

    free(ppInstanceExt);
}



static void vk_loadGlobalFunctions(void)
{
    ri.Printf(PRINT_ALL, " Loading vulkan instance functions \n");

    vk_getInstanceProcAddrImpl();

    #define INIT_INSTANCE_FUNCTION(func)                                \
    q##func = (PFN_ ## func)qvkGetInstanceProcAddr(vk.instance, #func); \
    if (q##func == NULL) {                                              \
        ri.Error(ERR_FATAL, "Failed to find entrypoint %s", #func);     \
    }

	INIT_INSTANCE_FUNCTION(vkCreateInstance)
	INIT_INSTANCE_FUNCTION(vkEnumerateInstanceExtensionProperties)

    //
	// Get instance level functions.
	//
	vk_createInstance();


	INIT_INSTANCE_FUNCTION(vkCreateDevice)
	INIT_INSTANCE_FUNCTION(vkDestroyInstance)
    INIT_INSTANCE_FUNCTION(vkDestroySurfaceKHR)
	INIT_INSTANCE_FUNCTION(vkEnumerateDeviceExtensionProperties)
	INIT_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices)
	INIT_INSTANCE_FUNCTION(vkGetDeviceProcAddr)

    INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures)
    INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)

    INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
   

#ifndef NDEBUG
    INIT_INSTANCE_FUNCTION(vkCreateDebugReportCallbackEXT)
	INIT_INSTANCE_FUNCTION(vkDestroyDebugReportCallbackEXT)	//
#endif

    #undef INIT_INSTANCE_FUNCTION

    ri.Printf(PRINT_ALL, " Init global functions done. \n");
}

////////////////////////////////


static void vk_selectPhysicalDevice(void)
{
    // After initializing the Vulkan library through a VkInstance
    // we need to look for and select a graphics card in the system
    // that supports the features we need. In fact we can select any
    // number of graphics cards and use them simultaneously.
	uint32_t gpu_count = 0;

    // Initial call to query gpu_count, then second call for gpu info.
	qvkEnumeratePhysicalDevices(vk.instance, &gpu_count, NULL);

	if (gpu_count <= 0)
		ri.Error(ERR_FATAL, "Vulkan: no physical device found");

    VkPhysicalDevice *pPhyDev = (VkPhysicalDevice *) malloc (sizeof(VkPhysicalDevice) * gpu_count);
    
    // TODO: multi graphic cards selection support
    VK_CHECK(qvkEnumeratePhysicalDevices(vk.instance, &gpu_count, pPhyDev));
    // For demo app we just grab the first physical device
    vk.physical_device = pPhyDev[0];
	
    free(pPhyDev);

    ri.Printf(PRINT_ALL, " Total %d graphics card, the first one is choosed. \n", gpu_count);

    ri.Printf(PRINT_ALL, " Get physical device memory properties: vk.devMemProperties \n");
    qvkGetPhysicalDeviceMemoryProperties(vk.physical_device, &vk.devMemProperties);
}



static void vk_selectSurfaceFormat(void)
{
    uint32_t nSurfmt;
    
    ri.Printf(PRINT_ALL, "\n -------- vk_selectSurfaceFormat() -------- \n");


    // Get the numbers of VkFormat's that are supported
    // "vk.surface" is the surface that will be associated with the swapchain.
    // "vk.surface" must be a valid VkSurfaceKHR handle
    VK_CHECK(qvkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &nSurfmt, NULL));
    assert(nSurfmt > 0);

    VkSurfaceFormatKHR *pSurfFmts = 
        (VkSurfaceFormatKHR *) malloc ( nSurfmt * sizeof(VkSurfaceFormatKHR) );

    // To query the supported swapchain format-color space pairs for a surface
    VK_CHECK(qvkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &nSurfmt, pSurfFmts));

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED, the surface
    // has no preferred format. Otherwise, at least one supported format will be returned.
    if ( (nSurfmt == 1) && (pSurfFmts[0].format == VK_FORMAT_UNDEFINED) )
    {
        // special case that means we can choose any format
        vk.surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        vk.surface_format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        ri.Printf(PRINT_ALL, "VK_FORMAT_R8G8B8A8_UNORM\n");
        ri.Printf(PRINT_ALL, "VK_COLORSPACE_SRGB_NONLINEAR_KHR\n");
    }
    else
    {
        uint32_t i;
        ri.Printf(PRINT_ALL, " Total %d surface formats supported, we choose: \n", nSurfmt);

        for( i = 0; i < nSurfmt; i++)
        {
            if( ( pSurfFmts[i].format == VK_FORMAT_B8G8R8A8_UNORM) &&
                ( pSurfFmts[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) )
            {

                ri.Printf(PRINT_ALL, " format = VK_FORMAT_B8G8R8A8_UNORM \n");
                ri.Printf(PRINT_ALL, " colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR \n");
                
                vk.surface_format = pSurfFmts[i];
                break;
            }
        }

        if (i == nSurfmt)
            vk.surface_format = pSurfFmts[0];
    }

    free(pSurfFmts);


    // To query the basic capabilities of a surface, needed in order to create a swapchain
	VK_CHECK(qvkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.physical_device, vk.surface, &vk.surface_caps));

    // VK_IMAGE_USAGE_TRANSFER_DST_BIT is required by image clear operations.
	if ((vk.surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
		ri.Error(ERR_FATAL, "VK_IMAGE_USAGE_TRANSFER_DST_BIT is not supported by you GPU.\n");

	// VK_IMAGE_USAGE_TRANSFER_SRC_BIT is required in order to take screenshots.
	if ((vk.surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0)
		ri.Error(ERR_FATAL, "VK_IMAGE_USAGE_TRANSFER_SRC_BIT is not supported by you GPU.\n");


    // To query supported format features which are properties of the physical device
	
    VkFormatProperties props;


    // To determine the set of valid usage bits for a given format,
    // call vkGetPhysicalDeviceFormatProperties.

    // ========================= color ================
    qvkGetPhysicalDeviceFormatProperties(vk.physical_device, vk.surface_format.format, &props);
    
    // Check if the device supports blitting to linear images 
    if ( props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT )
        ri.Printf(PRINT_ALL, "--- Linear TilingFeatures supported. ---\n");

    if ( props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT ) 
    {
        ri.Printf(PRINT_ALL, "--- Blitting from linear tiled images supported. ---\n");
    }

    if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT )
    {
        ri.Printf(PRINT_ALL, "--- Blitting from optimal tiled images supported. ---\n");
        vk.isBlitSupported = VK_TRUE;
    }


    //=========================== depth =====================================
    qvkGetPhysicalDeviceFormatProperties(vk.physical_device, VK_FORMAT_D24_UNORM_S8_UINT, &props);
	//glConfig.stencilBits = 8;	
    if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
    {
        ri.Printf(PRINT_ALL, " VK_FORMAT_D24_UNORM_S8_UINT optimal Tiling feature supported.\n");
        vk.fmt_DepthStencil = VK_FORMAT_D24_UNORM_S8_UINT;
    }
    else
    {
        qvkGetPhysicalDeviceFormatProperties(vk.physical_device, VK_FORMAT_D32_SFLOAT_S8_UINT, &props);

        if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
        {
            ri.Printf(PRINT_ALL, " VK_FORMAT_D32_SFLOAT_S8_UINT optimal Tiling feature supported.\n");
            vk.fmt_DepthStencil = VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        else
        {
            //formats[0] = VK_FORMAT_X8_D24_UNORM_PACK32;
		    //formats[1] = VK_FORMAT_D32_SFLOAT;
            // never get here.
	        ri.Error(ERR_FATAL, " Failed to find depth attachment format.\n");
        }
    }

    ri.Printf(PRINT_ALL, " -------- --------------------------- --------\n");
}


static void vk_selectQueueFamilyForPresentation(void)
{
    // Almosty every operation in Vulkan, anything from drawing textures,
    // requires commands to be submitted to a queue. There are different
    // types of queues that originate from differnet queue families and
    // each family of queues allows only a subset of commands. 
    // For example, there could be a queue family allows processing of 
    // compute commands or one that only allows memory thansfer related
    // commands. We need to check which queue families are supported by
    // the device and which one of these supports the commands that we use.


    uint32_t nSurfmt;
    qvkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &nSurfmt, NULL);
    
    assert(nSurfmt > 0);

    VkQueueFamilyProperties* pQueueFamilies = (VkQueueFamilyProperties *) malloc (
            nSurfmt * sizeof(VkQueueFamilyProperties) );

    // To query properties of queues available on a physical device
    qvkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &nSurfmt, pQueueFamilies);

    // Select queue family with presentation and graphics support
    // Iterate over each queue to learn whether it supports presenting:
    vk.queue_family_index = -1;
    
    uint32_t i;
    for (i = 0; i < nSurfmt; ++i)
    {
        // To look for a queue family that has the capability of presenting
        // to our window surface
        
        VkBool32 presentation_supported = VK_FALSE;
        VK_CHECK(qvkGetPhysicalDeviceSurfaceSupportKHR(
                    vk.physical_device, i, vk.surface, &presentation_supported));

        if (presentation_supported && 
                (pQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            vk.queue_family_index = i;
            
            ri.Printf(PRINT_ALL, " Queue family for presentation selected: %d\n",
                    vk.queue_family_index);

            break;
        }
    }

    free(pQueueFamilies);

    if (vk.queue_family_index == -1)
        ri.Error(ERR_FATAL, "Vulkan: failed to find queue family");
}


static void vk_createLogicalDevice(void)
{
    static const char* device_extensions[1] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    //  Not all graphics cards are capble of presenting images directly
    //  to a screen for various reasons, for example because they are 
    //  designed for servers and don't have any display outputs. 
    //  Secondly, since image presentation is heavily tied into the 
    //  window system and the surfaces associated with windows, it is
    //  not actually part of the vulkan core. You have to enable the
    //  VK_KHR_swapchain device extension after querying for its support.
    uint32_t nDevExts = 0;
    VkBool32 swapchainExtFound = 0;

    // To query the numbers of extensions available to a given physical device
    ri.Printf( PRINT_ALL, " Check for VK_KHR_swapchain extension. \n" );

    qvkEnumerateDeviceExtensionProperties( vk.physical_device, NULL, &nDevExts, NULL);

    VkExtensionProperties* pDeviceExt = 
        (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * nDevExts);

    qvkEnumerateDeviceExtensionProperties( vk.physical_device, NULL, &nDevExts, pDeviceExt);


    uint32_t j;
    for (j = 0; j < nDevExts; j++)
    {
        if (!strcmp(device_extensions[0], pDeviceExt[j].extensionName))
        {
            swapchainExtFound = VK_TRUE;
            break;
        }
    }
    if (VK_FALSE == swapchainExtFound)
        ri.Error(ERR_FATAL, "VK_KHR_SWAPCHAIN_EXTENSION_NAME is not available");

    free(pDeviceExt);


    const float priority = 1.0;
    VkDeviceQueueCreateInfo queue_desc;
    queue_desc.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_desc.pNext = NULL;
    queue_desc.flags = 0;
    queue_desc.queueFamilyIndex = vk.queue_family_index;
    queue_desc.queueCount = 1;
    queue_desc.pQueuePriorities = &priority;


    // Query fine-grained feature support for this device. If APP 
    // has specific feature requirements it should check supported
    // features based on this query.

	VkPhysicalDeviceFeatures features;
	qvkGetPhysicalDeviceFeatures(vk.physical_device, &features);
	if (features.shaderClipDistance == VK_FALSE)
		ri.Error(ERR_FATAL,
            "vk_create_device: shaderClipDistance feature is not supported");
	if (features.fillModeNonSolid == VK_FALSE)
	    ri.Error(ERR_FATAL,
            "vk_create_device: fillModeNonSolid feature is not supported");


    VkDeviceCreateInfo device_desc;
    device_desc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_desc.pNext = NULL;
    device_desc.flags = 0;
    device_desc.queueCreateInfoCount = 1;
    device_desc.pQueueCreateInfos = &queue_desc;
    device_desc.enabledLayerCount = 0;
    device_desc.ppEnabledLayerNames = NULL;
    device_desc.enabledExtensionCount = 1;
    device_desc.ppEnabledExtensionNames = device_extensions;
    device_desc.pEnabledFeatures = &features;
    

    // After selecting a physical device to use we need to set up a
    // logical device to interface with it. The logical device 
    // creation process id similar to the instance creation process
    // and describes the features we want to use. we also need to 
    // specify which queues to create now that we've queried which
    // queue families are available. You can create multiple logical
    // devices from the same physical device if you have varying requirements.
    ri.Printf( PRINT_ALL, " Create logical device: vk.device \n" );
    VK_CHECK(qvkCreateDevice(vk.physical_device, &device_desc, NULL, &vk.device));

}


static void vk_loadDeviceFunctions(void)
{
    ri.Printf( PRINT_ALL, " Loading device level function. \n" );

    #define INIT_DEVICE_FUNCTION(func)                              \
    q##func = (PFN_ ## func)qvkGetDeviceProcAddr(vk.device, #func); \
    if (q##func == NULL) {                                          \
        ri.Error(ERR_FATAL, "Failed to find entrypoint %s", #func); \
    }     

	INIT_DEVICE_FUNCTION(vkAllocateCommandBuffers)
	INIT_DEVICE_FUNCTION(vkAllocateDescriptorSets)
	INIT_DEVICE_FUNCTION(vkAllocateMemory)
	INIT_DEVICE_FUNCTION(vkBeginCommandBuffer)
	INIT_DEVICE_FUNCTION(vkBindBufferMemory)
	INIT_DEVICE_FUNCTION(vkBindImageMemory)
	INIT_DEVICE_FUNCTION(vkCmdBeginRenderPass)
	INIT_DEVICE_FUNCTION(vkCmdBindDescriptorSets)
	INIT_DEVICE_FUNCTION(vkCmdBindIndexBuffer)
	INIT_DEVICE_FUNCTION(vkCmdBindPipeline)
	INIT_DEVICE_FUNCTION(vkCmdBindVertexBuffers)
	INIT_DEVICE_FUNCTION(vkCmdBlitImage)
	INIT_DEVICE_FUNCTION(vkCmdClearAttachments)
	INIT_DEVICE_FUNCTION(vkCmdCopyBufferToImage)
	INIT_DEVICE_FUNCTION(vkCmdCopyImage)
    INIT_DEVICE_FUNCTION(vkCmdCopyImageToBuffer)
	INIT_DEVICE_FUNCTION(vkCmdDraw)
	INIT_DEVICE_FUNCTION(vkCmdDrawIndexed)
	INIT_DEVICE_FUNCTION(vkCmdEndRenderPass)
	INIT_DEVICE_FUNCTION(vkCmdPipelineBarrier)
	INIT_DEVICE_FUNCTION(vkCmdPushConstants)
	INIT_DEVICE_FUNCTION(vkCmdSetDepthBias)
	INIT_DEVICE_FUNCTION(vkCmdSetScissor)
	INIT_DEVICE_FUNCTION(vkCmdSetViewport)
	INIT_DEVICE_FUNCTION(vkCreateBuffer)
	INIT_DEVICE_FUNCTION(vkCreateCommandPool)
	INIT_DEVICE_FUNCTION(vkCreateDescriptorPool)
	INIT_DEVICE_FUNCTION(vkCreateDescriptorSetLayout)
	INIT_DEVICE_FUNCTION(vkCreateFence)
	INIT_DEVICE_FUNCTION(vkCreateFramebuffer)
	INIT_DEVICE_FUNCTION(vkCreateGraphicsPipelines)
	INIT_DEVICE_FUNCTION(vkCreateImage)
	INIT_DEVICE_FUNCTION(vkCreateImageView)
	INIT_DEVICE_FUNCTION(vkCreatePipelineLayout)
	INIT_DEVICE_FUNCTION(vkCreateRenderPass)
	INIT_DEVICE_FUNCTION(vkCreateSampler)
	INIT_DEVICE_FUNCTION(vkCreateSemaphore)
	INIT_DEVICE_FUNCTION(vkCreateShaderModule)
	INIT_DEVICE_FUNCTION(vkDestroyBuffer)
	INIT_DEVICE_FUNCTION(vkDestroyCommandPool)
	INIT_DEVICE_FUNCTION(vkDestroyDescriptorPool)
	INIT_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout)
	INIT_DEVICE_FUNCTION(vkDestroyDevice)
	INIT_DEVICE_FUNCTION(vkDestroyFence)
	INIT_DEVICE_FUNCTION(vkDestroyFramebuffer)
	INIT_DEVICE_FUNCTION(vkDestroyImage)
	INIT_DEVICE_FUNCTION(vkDestroyImageView)
	INIT_DEVICE_FUNCTION(vkDestroyPipeline)
	INIT_DEVICE_FUNCTION(vkDestroyPipelineLayout)
	INIT_DEVICE_FUNCTION(vkDestroyRenderPass)
	INIT_DEVICE_FUNCTION(vkDestroySampler)
	INIT_DEVICE_FUNCTION(vkDestroySemaphore)
	INIT_DEVICE_FUNCTION(vkDestroyShaderModule)
	INIT_DEVICE_FUNCTION(vkDeviceWaitIdle)
	INIT_DEVICE_FUNCTION(vkEndCommandBuffer)
	INIT_DEVICE_FUNCTION(vkFreeCommandBuffers)
	INIT_DEVICE_FUNCTION(vkFreeDescriptorSets)
	INIT_DEVICE_FUNCTION(vkFreeMemory)
	INIT_DEVICE_FUNCTION(vkGetBufferMemoryRequirements)
	INIT_DEVICE_FUNCTION(vkGetDeviceQueue)
	INIT_DEVICE_FUNCTION(vkGetImageMemoryRequirements)
	INIT_DEVICE_FUNCTION(vkGetImageSubresourceLayout)
	INIT_DEVICE_FUNCTION(vkMapMemory)
	INIT_DEVICE_FUNCTION(vkUnmapMemory)
	INIT_DEVICE_FUNCTION(vkQueueSubmit)
	INIT_DEVICE_FUNCTION(vkQueueWaitIdle)
	INIT_DEVICE_FUNCTION(vkResetDescriptorPool)
	INIT_DEVICE_FUNCTION(vkResetFences)
	INIT_DEVICE_FUNCTION(vkUpdateDescriptorSets)
	INIT_DEVICE_FUNCTION(vkWaitForFences)
    
	INIT_DEVICE_FUNCTION(vkCreateSwapchainKHR)
	INIT_DEVICE_FUNCTION(vkDestroySwapchainKHR)
	INIT_DEVICE_FUNCTION(vkGetSwapchainImagesKHR)
    INIT_DEVICE_FUNCTION(vkAcquireNextImageKHR)
	INIT_DEVICE_FUNCTION(vkQueuePresentKHR)

    #undef INIT_DEVICE_FUNCTION
}



void vk_getProcAddress(void)
{
    vk_loadGlobalFunctions();

#ifndef NDEBUG
	// Create debug callback.
    vk_createDebugCallback(vk_DebugCallback);
#endif

    // The window surface needs to be created right after the instance creation,
    // because it can actually influence the presentation mode selection.
	vk_createSurfaceImpl(); 
   
    // select physical device
    vk_selectPhysicalDevice();

    vk_selectSurfaceFormat(); 

    vk_selectQueueFamilyForPresentation();

    vk_createLogicalDevice();

    // Get device level functions.
    vk_loadDeviceFunctions();

    // a call to retrieve queue handle
	qvkGetDeviceQueue(vk.device, vk.queue_family_index, 0, &vk.queue);
}


void vk_clearProcAddress(void)
{
    ri.Printf( PRINT_ALL, " clear all proc address \n" );

	qvkCreateInstance                           = NULL;
	qvkEnumerateInstanceExtensionProperties		= NULL;

	qvkCreateDevice								= NULL;
	qvkDestroyInstance							= NULL;
	qvkEnumerateDeviceExtensionProperties		= NULL;
	qvkEnumeratePhysicalDevices					= NULL;
	qvkGetDeviceProcAddr						= NULL;
	qvkGetPhysicalDeviceFeatures				= NULL;
	qvkGetPhysicalDeviceFormatProperties		= NULL;
	qvkGetPhysicalDeviceMemoryProperties		= NULL;
	qvkGetPhysicalDeviceProperties				= NULL;
	qvkGetPhysicalDeviceQueueFamilyProperties	= NULL;

    qvkDestroySurfaceKHR						= NULL;
	qvkGetPhysicalDeviceSurfaceCapabilitiesKHR	= NULL;
	qvkGetPhysicalDeviceSurfaceFormatsKHR		= NULL;
	qvkGetPhysicalDeviceSurfacePresentModesKHR	= NULL;
	qvkGetPhysicalDeviceSurfaceSupportKHR		= NULL;
#ifndef NDEBUG
	qvkCreateDebugReportCallbackEXT				= NULL;
	qvkDestroyDebugReportCallbackEXT			= NULL;
#endif

	qvkAllocateCommandBuffers					= NULL;
	qvkAllocateDescriptorSets					= NULL;
	qvkAllocateMemory							= NULL;
	qvkBeginCommandBuffer						= NULL;
	qvkBindBufferMemory							= NULL;
	qvkBindImageMemory							= NULL;
	qvkCmdBeginRenderPass						= NULL;
	qvkCmdBindDescriptorSets					= NULL;
	qvkCmdBindIndexBuffer						= NULL;
	qvkCmdBindPipeline							= NULL;
	qvkCmdBindVertexBuffers						= NULL;
	qvkCmdBlitImage								= NULL;
	qvkCmdClearAttachments						= NULL;
	qvkCmdCopyBufferToImage						= NULL;
	qvkCmdCopyImage								= NULL;
    qvkCmdCopyImageToBuffer                     = NULL;
	qvkCmdDraw									= NULL;
	qvkCmdDrawIndexed							= NULL;
	qvkCmdEndRenderPass							= NULL;
	qvkCmdPipelineBarrier						= NULL;
	qvkCmdPushConstants							= NULL;
	qvkCmdSetDepthBias							= NULL;
	qvkCmdSetScissor							= NULL;
	qvkCmdSetViewport							= NULL;
	qvkCreateBuffer								= NULL;
	qvkCreateCommandPool						= NULL;
	qvkCreateDescriptorPool						= NULL;
	qvkCreateDescriptorSetLayout				= NULL;
	qvkCreateFence								= NULL;
	qvkCreateFramebuffer						= NULL;
	qvkCreateGraphicsPipelines					= NULL;
	qvkCreateImage								= NULL;
	qvkCreateImageView							= NULL;
	qvkCreatePipelineLayout						= NULL;
	qvkCreateRenderPass							= NULL;
	qvkCreateSampler							= NULL;
	qvkCreateSemaphore							= NULL;
	qvkCreateShaderModule						= NULL;
	qvkDestroyBuffer							= NULL;
	qvkDestroyCommandPool						= NULL;
	qvkDestroyDescriptorPool					= NULL;
	qvkDestroyDescriptorSetLayout				= NULL;
	qvkDestroyDevice							= NULL;
	qvkDestroyFence								= NULL;
	qvkDestroyFramebuffer						= NULL;
	qvkDestroyImage								= NULL;
	qvkDestroyImageView							= NULL;
	qvkDestroyPipeline							= NULL;
	qvkDestroyPipelineLayout					= NULL;
	qvkDestroyRenderPass						= NULL;
	qvkDestroySampler							= NULL;
	qvkDestroySemaphore							= NULL;
	qvkDestroyShaderModule						= NULL;
	qvkDeviceWaitIdle							= NULL;
	qvkEndCommandBuffer							= NULL;
	qvkFreeCommandBuffers						= NULL;
	qvkFreeDescriptorSets						= NULL;
	qvkFreeMemory								= NULL;
	qvkGetBufferMemoryRequirements				= NULL;
	qvkGetDeviceQueue							= NULL;
	qvkGetImageMemoryRequirements				= NULL;
	qvkGetImageSubresourceLayout				= NULL;
	qvkMapMemory								= NULL;
    qvkUnmapMemory                              = NULL;
	qvkQueueSubmit								= NULL;
	qvkQueueWaitIdle							= NULL;
	qvkResetDescriptorPool						= NULL;
	qvkResetFences								= NULL;
	qvkUpdateDescriptorSets						= NULL;
	qvkWaitForFences							= NULL;
	qvkAcquireNextImageKHR						= NULL;
	qvkCreateSwapchainKHR						= NULL;
	qvkDestroySwapchainKHR						= NULL;
	qvkGetSwapchainImagesKHR					= NULL;
	qvkQueuePresentKHR							= NULL;
}

static void vk_create_command_pool(VkCommandPool* pPool)
{
    // Command pools are opaque objects that command buffer memory is allocated from,
    // and which allow the implementation to amortize the cost of resource creation
    // across multiple command buffers. Command pools are externally synchronized,
    // meaning that a command pool must not be used concurrently in multiple threads.
    // That includes use via recording commands on any command buffers allocated from
    // the pool, as well as operations that allocate, free, and reset command buffers
    // or the pool itself.


    VkCommandPoolCreateInfo desc;
    desc.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    desc.pNext = NULL;
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT specifies that command buffers
    // allocated from the pool will be short-lived, meaning that they will
    // be reset or freed in a relatively short timeframe. This flag may be
    // used by the implementation to control memory allocation behavior
    // within the pool.
    //
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT allows any command
    // buffer allocated from a pool to be individually reset to the initial
    // state; either by calling vkResetCommandBuffer, or via the implicit 
    // reset when calling vkBeginCommandBuffer. If this flag is not set on
    // a pool, then vkResetCommandBuffer must not be called for any command
    // buffer allocated from that pool.
    desc.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    desc.queueFamilyIndex = vk.queue_family_index;

    VK_CHECK(qvkCreateCommandPool(vk.device, &desc, NULL, pPool));
}


static void vk_create_command_buffer(VkCommandPool pool, VkCommandBuffer* pBuf)
{
    // Command buffers are objects used to record commands which can be
    // subsequently submitted to a device queue for execution. There are
    // two levels of command buffers:
    // - primary command buffers, which can execute secondary command buffers,
    //   and which are submitted to queues.
    // - secondary command buffers, which can be executed by primary command buffers,
    //   and which are not directly submitted to queues.
    //
    // Recorded commands include commands to bind pipelines and descriptor sets
    // to the command buffer, commands to modify dynamic state, commands to draw
    // (for graphics rendering), commands to dispatch (for compute), commands to
    // execute secondary command buffers (for primary command buffers only), 
    // commands to copy buffers and images, and other commands.
    //
    // Each command buffer manages state independently of other command buffers.
    // There is no inheritance of state across primary and secondary command 
    // buffers, or between secondary command buffers. 
    // 
    // When a command buffer begins recording, all state in that command buffer is undefined. 
    // When secondary command buffer(s) are recorded to execute on a primary command buffer,
    // the secondary command buffer inherits no state from the primary command buffer,
    // and all state of the primary command buffer is undefined after an execute secondary
    // command buffer command is recorded. There is one exception to this rule - if the primary
    // command buffer is inside a render pass instance, then the render pass and subpass state
    // is not disturbed by executing secondary command buffers. Whenever the state of a command
    // buffer is undefined, the application must set all relevant state on the command buffer
    // before any state dependent commands such as draws and dispatches are recorded, otherwise
    // the behavior of executing that command buffer is undefined.
    //
    // Unless otherwise specified, and without explicit synchronization, the various commands
    // submitted to a queue via command buffers may execute in arbitrary order relative to
    // each other, and/or concurrently. Also, the memory side-effects of those commands may
    // not be directly visible to other commands without explicit memory dependencies. 
    // This is true within a command buffer, and across command buffers submitted to a given
    // queue. See the synchronization chapter for information on implicit and explicit
    // synchronization between commands.


    VkCommandBufferAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.commandPool = pool;
    // Can be submitted to a queue for execution,
    // but cannnot be called from other command buffers.
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    VK_CHECK(qvkAllocateCommandBuffers(vk.device, &alloc_info, pBuf));
}


void vk_initialize(void)
{
    // This function is responsible for initializing a valid Vulkan subsystem.

    vk_createWindow();
        
    vk_getProcAddress(); 
 

	// Swapchain. vk.physical_device required to be init. 
	vk_createSwapChain(vk.device, vk.surface, vk.surface_format);

	//
	// Sync primitives.
	//
    vk_create_sync_primitives();

	// we have to create a command pool before we can create command buffers
    // command pools manage the memory that is used to store the buffers and
    // command buffers are allocated from them.
    ri.Printf(PRINT_ALL, " Create command pool: vk.command_pool \n");
    vk_create_command_pool(&vk.command_pool);
    
    ri.Printf(PRINT_ALL, " Create command buffer: vk.command_buffer \n");
    vk_create_command_buffer(vk.command_pool, &vk.command_buffer);

    // Depth attachment image.
    vk_createDepthAttachment();


    vk_createFrameBuffers();

	// Pipeline layout.
	// You can use uniform values in shaders, which are globals similar to
    // dynamic state variables that can be changes at the drawing time to
    // alter the behavior of your shaders without having to recreate them.
    // They are commonly used to create texture samplers in the fragment 
    // shader. The uniform values need to be specified during pipeline
    // creation by creating a VkPipelineLayout object.
    
    vk_createPipelineLayout();

	//
	vk_createVertexBuffer();
    vk_createIndexBuffer();;
	//
	// Shader modules.
	//
	vk_loadShaderModules();

	//
	// Standard pipelines.
	//
    create_standard_pipelines();

    // print info
    vulkanInfo_f();
}


void vk_shutdown(void)
{
    ri.Printf( PRINT_ALL, "vk_shutdown()\n" );

    vk_destroyDepthAttachment();

    vk_destroyFrameBuffers();

    vk_destroy_shading_data();

    vk_destroy_sync_primitives();
    
    vk_destroyShaderModules();

//
    vk_destroyGlobalStagePipeline();
//
    // Command buffers will be automatically freed when their
    // command pool is destroyed, so it don't need an explicit 
    // cleanup.
    ri.Printf( PRINT_ALL, " Free command buffers: vk.command_buffer. \n" );     
    qvkFreeCommandBuffers(vk.device, vk.command_pool, 1, &vk.command_buffer); 
    ri.Printf( PRINT_ALL, " Destroy command pool: vk.command_pool. \n" );
    qvkDestroyCommandPool(vk.device, vk.command_pool, NULL);

    ri.Printf( PRINT_ALL, " Destroy logical device: vk.device. \n" );
	qvkDestroyDevice(vk.device, NULL);

    ri.Printf( PRINT_ALL, " Destroy surface: vk.surface. \n" );
    // make sure that the surface is destroyed before the instance
    qvkDestroySurfaceKHR(vk.instance, vk.surface, NULL);

#ifndef NDEBUG
    ri.Printf( PRINT_ALL, " Destroy callback function: vk.h_debugCB. \n" );

	qvkDestroyDebugReportCallbackEXT(vk.instance, vk.h_debugCB, NULL);
#endif

    ri.Printf( PRINT_ALL, " Destroy instance: vk.instance. \n" );
	qvkDestroyInstance(vk.instance, NULL);

    ri.Printf( PRINT_ALL, " clear vk struct: vk \n" );
	memset(&vk, 0, sizeof(vk));

	vk_clearProcAddress();
}


void vulkanInfo_f( void ) 
{

	// VULKAN

    ri.Printf( PRINT_ALL, "\nActive 3D API: Vulkan\n" );

    // To query general properties of physical devices once enumerated
    VkPhysicalDeviceProperties props;
    qvkGetPhysicalDeviceProperties(vk.physical_device, &props);

    uint32_t major = VK_VERSION_MAJOR(props.apiVersion);
    uint32_t minor = VK_VERSION_MINOR(props.apiVersion);
    uint32_t patch = VK_VERSION_PATCH(props.apiVersion);

    const char* device_type;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        device_type = "INTEGRATED_GPU";
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        device_type = "DISCRETE_GPU";
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
        device_type = "VIRTUAL_GPU";
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        device_type = "CPU";
    else
        device_type = "Unknown";

    const char* vendor_name = "unknown";
    if (props.vendorID == 0x1002) {
        vendor_name = "Advanced Micro Devices, Inc.";
    } else if (props.vendorID == 0x10DE) {
        vendor_name = "NVIDIA";
    } else if (props.vendorID == 0x8086) {
        vendor_name = "Intel Corporation";
    }

    ri.Printf(PRINT_ALL, "Vk api version: %d.%d.%d\n", major, minor, patch);
    ri.Printf(PRINT_ALL, "Vk driver version: %d\n", props.driverVersion);
    ri.Printf(PRINT_ALL, "Vk vendor id: 0x%X (%s)\n", props.vendorID, vendor_name);
    ri.Printf(PRINT_ALL, "Vk device id: 0x%X\n", props.deviceID);
    ri.Printf(PRINT_ALL, "Vk device type: %s\n", device_type);
    ri.Printf(PRINT_ALL, "Vk device name: %s\n", props.deviceName);

//    ri.Printf(PRINT_ALL, "\n The maximum number of sampler objects,  
//    as created by vkCreateSampler, which can simultaneously exist on a device is: %d\n", 
//        props.limits.maxSamplerAllocationCount);
//	 4000

    // Look for device extensions
    {
        uint32_t nDevExts = 0;

        // To query the extensions available to a given physical device
        VK_CHECK( qvkEnumerateDeviceExtensionProperties( vk.physical_device, NULL, &nDevExts, NULL) );

        VkExtensionProperties* pDeviceExt = 
            (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * nDevExts);

        qvkEnumerateDeviceExtensionProperties(
                vk.physical_device, NULL, &nDevExts, pDeviceExt);


        ri.Printf(PRINT_ALL, "---------- Total Device Extension Supported ---------- \n");
        uint32_t i;
        for (i=0; i<nDevExts; i++)
        {
            ri.Printf(PRINT_ALL, " %s \n", pDeviceExt[i].extensionName);
        }
        ri.Printf(PRINT_ALL, "---------- -------------------------------- ---------- \n");
    }

    ri.Printf(PRINT_ALL, "Vk instance extensions: \n%s\n\n", glConfig.extensions_string);


	//
	// Info that for UI display
	//
	strncpy( glConfig.vendor_string, vendor_name, sizeof( glConfig.vendor_string ) );
	strncpy( glConfig.renderer_string, props.deviceName, sizeof( glConfig.renderer_string ) );
    if (*glConfig.renderer_string && glConfig.renderer_string[strlen(glConfig.renderer_string) - 1] == '\n')
         glConfig.renderer_string[strlen(glConfig.renderer_string) - 1] = 0;     
	char tmpBuf[128] = {0};

    snprintf(tmpBuf, 128, " Vk api version: %d.%d.%d ", major, minor, patch);
	
    strncpy( glConfig.version_string, tmpBuf, sizeof( glConfig.version_string ) );

    gpuMemUsageInfo_f();
}


const char * cvtResToStr(VkResult result)
{
    switch(result)
    {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
            return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
//
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_NOT_PERMITTED_EXT:
            return "VK_ERROR_NOT_PERMITTED_EXT";
//
        case VK_RESULT_MAX_ENUM:
            return "VK_RESULT_MAX_ENUM";
        case VK_RESULT_RANGE_SIZE:
            return "VK_RESULT_RANGE_SIZE"; 
        case VK_ERROR_FRAGMENTATION_EXT:
            return "VK_ERROR_FRAGMENTATION_EXT";
    }

    return "UNKNOWN_ERROR";
}
