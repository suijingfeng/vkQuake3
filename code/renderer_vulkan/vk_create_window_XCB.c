#include "VKimpl.h"


#include <xcb/xcb.h>
//#include <xcb/xcb_atom.h>
#include "vulkan/vulkan_xcb.h"

#include "tr_local.h"
#include "vk_instance.h"
#include "tr_displayResolution.h"
#include "tr_cvar.h"
#include "../qcommon/sys_loadlib.h"

// Allow a maximum of two outstanding presentation operations.
#define FRAME_LAG 2


static void* vk_library_handle; // instance of Vulkan library

static xcb_connection_t *connection;

// In the X Window System, a window is characterized by an Id.
// So, in XCB, typedef uint32_t xcb_window_t
typedef uint32_t xcb_window_t;
typedef uint32_t xcb_gcontext_t;

static xcb_window_t window;
static xcb_screen_t *screen;

static PFN_vkCreateXcbSurfaceKHR qvkCreateXcbSurfaceKHR;


static unsigned int GetDesktopWidth(void)
{
    // hardcode now;
    return screen->width_in_pixels;
}

static unsigned int GetDesktopHeight(void)
{
    // hardcode now;
    return screen->height_in_pixels;
}


static void vk_resize( void )
{
/*
    // Don't react to resize until after first initialization.
    if (!demo->prepared) {
        if (demo->is_minimized) {
            demo_prepare(demo);
        }
        return;
    }
    // In order to properly resize the window, we must re-create the swapchain
    // AND redo the command buffers, etc.
    //
    // First, perform part of the demo_cleanup() function:
    demo->prepared = false;
    vkDeviceWaitIdle(demo->device);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyFramebuffer(demo->device, demo->swapchain_image_resources[i].framebuffer, NULL);
    }
    vkDestroyDescriptorPool(demo->device, demo->desc_pool, NULL);

    vkDestroyPipeline(demo->device, demo->pipeline, NULL);
    vkDestroyPipelineCache(demo->device, demo->pipelineCache, NULL);
    vkDestroyRenderPass(demo->device, demo->render_pass, NULL);
    vkDestroyPipelineLayout(demo->device, demo->pipeline_layout, NULL);
    vkDestroyDescriptorSetLayout(demo->device, demo->desc_layout, NULL);

    for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
        vkDestroyImageView(demo->device, demo->textures[i].view, NULL);
        vkDestroyImage(demo->device, demo->textures[i].image, NULL);
        vkFreeMemory(demo->device, demo->textures[i].mem, NULL);
        vkDestroySampler(demo->device, demo->textures[i].sampler, NULL);
    }

    vkDestroyImageView(demo->device, demo->depth.view, NULL);
    vkDestroyImage(demo->device, demo->depth.image, NULL);
    vkFreeMemory(demo->device, demo->depth.mem, NULL);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyImageView(demo->device, demo->swapchain_image_resources[i].view, NULL);
        vkFreeCommandBuffers(demo->device, demo->cmd_pool, 1, &demo->swapchain_image_resources[i].cmd);
        vkDestroyBuffer(demo->device, demo->swapchain_image_resources[i].uniform_buffer, NULL);
        vkFreeMemory(demo->device, demo->swapchain_image_resources[i].uniform_memory, NULL);
    }
    vkDestroyCommandPool(demo->device, demo->cmd_pool, NULL);
    demo->cmd_pool = VK_NULL_HANDLE;
    if (demo->separate_present_queue) {
        vkDestroyCommandPool(demo->device, demo->present_cmd_pool, NULL);
    }
    free(demo->swapchain_image_resources);

    // Second, re-perform the demo_prepare() function, which will re-create the
    // swapchain:
    demo_prepare(demo);
*/
}



void vk_createWindow(void)
{
    if(window)
        xcb_destroy_window(connection, window);


    ri.Printf(PRINT_ALL, "... Setting XCB connection ...\n");

    const char *display_envar = getenv("DISPLAY");
    if (display_envar == NULL || display_envar[0] == '\0')
    {
        ri.Error(ERR_FATAL,
            "Environment variable DISPLAY requires a valid value.");
    }

    // An X program first needs to open the connection to the X server, 
    // using xcb_connect(
    // const char *displayname, //<- if NULL, uses the DISPLAY environment variable).
    // int* screenp );  // returns the screen number of the connection;
                        // can provide NULL if you don't care.

    int screenNum;

    connection = xcb_connect(NULL, &screenNum);
    if (xcb_connection_has_error(connection) > 0)
    {
        ri.Error(ERR_FATAL,
            "Cannot find a compatible Vulkan installable client driver (ICD)");
    }

    const xcb_setup_t * setup = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    while (screenNum-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;


    R_InitDisplayResolution();
	ri.Printf(PRINT_ALL, "...vk_createWindow...\n");

    // developing now
    r_mode->integer = 3;
/*
	// Create window.
    if ( (r_mode->integer == -2) || (r_fullscreen->integer == 1))
    {
		ri.Printf( PRINT_ALL, "...setting fullscreen mode:");
		glConfig.vidWidth = GetDesktopWidth();
		glConfig.vidHeight = GetDesktopHeight();
		glConfig.windowAspect = glConfig.vidWidth/glConfig.vidHeight;
        glConfig.isFullscreen = 1;
	}
    else
*/    
    {
		ri.Printf( PRINT_ALL, "...setting mode %d:", r_mode->integer );
		R_GetModeInfo(&glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, r_mode->integer);
        glConfig.isFullscreen = 0;
	}

    ri.Printf( PRINT_ALL, " %d %d %s\n", glConfig.vidWidth, glConfig.vidHeight, glConfig.isFullscreen ? "FS" : "W");


    uint32_t value_mask, value_list[32];


    // We first ask for a new Id for our window
    window = xcb_generate_id(connection);
    
    
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->white_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
                XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE;

/*
    Then, XCB supplies the following function to create new windows:

    xcb_void_cookie_t xcb_create_window (
    xcb_connection_t *connection, // Pointer to the xcb_connection_t structure
    uint8_t depth,    // Depth of the screen
    xcb_window_t wid,    // Id of the window 
    xcb_window_t parent, // Id of the parent windows of the new window 
    int16_t x, // X position of the top-left corner of the window (in pixels)
    int16_t y, // Y position of the top-left corner of the window (in pixels)
    uint16_t width, // Width of the window (in pixels)
    uint16_t height,// Height of the window (in pixels)
    uint16_t border_width,  // Width of the window's border (in pixels)
    uint16_t _class,
    xcb_visualid_t visual,
    uint32_t value_mask,
    const uint32_t* value_list );
*/

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root,
            0, 0, glConfig.vidWidth, glConfig.vidHeight, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual, value_mask, value_list);

    static const char* pVkTitle = "VulkanArena";
    /* Set the title of the window */
    xcb_change_property (connection, XCB_PROP_MODE_REPLACE, window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen (pVkTitle), pVkTitle);

    /* set the title of the window icon */

    static const char * pIconTitle = "OpenArena (iconified)";
    
    xcb_change_property (connection, XCB_PROP_MODE_REPLACE,
        window, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING,
            8, strlen(pIconTitle), pIconTitle);
    
    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie 
        = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply
        = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2
        = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    
    xcb_intern_atom_reply_t *atom_wm_delete_window 
        = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, 
            window, (*reply).atom, 4, 32, 1, &(*atom_wm_delete_window).atom);
    
    free(reply);

    // The fact that we created the window does not mean that
    // it will be drawn on screen. By default, newly created windows
    // are not mapped on the screen (they are invisible). In order to
    // make our window visible, we use the function xcb_map_window()

    // Mapping a window causes the window to appear on the screen, 
    // Un-mapping it causes it to be removed from the screen 
    // (although the window as a logical entity still exists). 
    // This gives the effect of making a window hidden (unmapped) 
    // and shown again (mapped). For example, if we have a dialog box
    // window in our program, instead of creating it every time the user
    // asks to open it, we can create the window once, in an un-mapped mode,
    // and when the user asks to open it, we simply map the window on the screen.
    // When the user clicked the 'OK' or 'Cancel' button, we simply un-map the window.
    // This is much faster than creating and destroying the window, 
    // however, the cost is wasted resources, both on the client side, 
    // and on the X server side. 
    xcb_map_window(connection, window);
	
    ri.Printf(PRINT_ALL, "...xcb_map_window...\n");
    
    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    uint16_t mask = 0;
    mask |= XCB_CONFIG_WINDOW_X;
    mask |= XCB_CONFIG_WINDOW_Y;
    mask |= XCB_CONFIG_WINDOW_WIDTH;
    mask |= XCB_CONFIG_WINDOW_HEIGHT;

    const uint32_t coords[4] = {0, 0, glConfig.vidWidth, glConfig.vidHeight};
    xcb_configure_window(connection, window, mask, coords);


    // We first ask the X server to attribute an Id to our graphic context
    // Then, we set the attributes of the graphic context with xcb_create_gc

    xcb_gcontext_t  gc_black = xcb_generate_id(connection);
    uint32_t        gc_mask     = XCB_GC_FOREGROUND;
    uint32_t        gc_value[]  = { screen->black_pixel };
    xcb_create_gc (connection, gc_black, window, gc_mask, gc_value);


	// This depends on SDL_INIT_VIDEO, hence having it here
	ri.IN_Init(connection, window);

 
}


void vk_getInstanceProcAddrImpl(void)
{
    // Load Vulkan DLL.
#if defined( _WIN32 )
    const char* dll_name = "vulkan-1.dll";
#elif defined(MACOS_X)
    const char* dll_name = "what???";
#else
    const char* dll_name = "libvulkan.so.1";
#endif

    ri.Printf(PRINT_ALL, "...calling LoadLibrary('%s')\n", dll_name);
    vk_library_handle = Sys_LoadLibrary(dll_name);

    if (vk_library_handle == NULL) {
        ri.Error(ERR_FATAL, "VKimp_init - could not load %s\n", dll_name);
    }

    qvkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)Sys_LoadFunction(vk_library_handle, "vkGetInstanceProcAddr");
    if( qvkGetInstanceProcAddr == NULL)
    {
        ri.Error(ERR_FATAL, "Failed to find entrypoint vkGetInstanceProcAddr\n"); 
    }
    
    ri.Printf(PRINT_ALL,  " Get instance proc address. (using XCB)\n");
}


void vk_destroyWindow(void)
{
	ri.Printf(PRINT_ALL, " Destroy Window Subsystem.\n");

    // To close a connection, it suffices to use:
    // void xcb_disconnect (xcb_connection_t *c);


    qvkCreateXcbSurfaceKHR = NULL;
  
    
    //xcb_disconnect(connection);

    Sys_UnloadLibrary(vk_library_handle);

    xcb_destroy_window(connection, window);
/*  
	if (g_wv.window) {
		ri.Printf(PRINT_ALL, "...destroying Vulkan window\n");


		if (g_wv.hWnd == g_wv.window) {
			g_wv.hWnd = NULL;
		}
		g_wv.window = NULL;
	}

	if (vk_library_handle != NULL) {
		ri.Printf(PRINT_ALL, "...unloading Vulkan DLL\n");
		FreeLibrary(vk_library_handle);
		vk_library_handle = NULL;
	}
	qvkGetInstanceProcAddr = NULL;

	// For vulkan mode we still have qgl pointers initialized with placeholder values.
	// Reset them the same way as we do in opengl mode.
	QGL_Shutdown();

	WG_RestoreGamma();
*/
	memset(&glConfig, 0, sizeof(glConfig));

}


void vk_createSurfaceImpl(void)
{

    qvkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)qvkGetInstanceProcAddr(vk.instance, "vkCreateXcbSurfaceKHR");
    if( qvkCreateXcbSurfaceKHR == NULL)
    {
        ri.Error(ERR_FATAL, "Failed to find entrypoint qvkCreateXcbSurfaceKHR\n"); 
    }
   
    VkXcbSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.connection = connection;
    createInfo.window = window;

    qvkCreateXcbSurfaceKHR(vk.instance, &createInfo, NULL, &vk.surface);

}

/*
static void CreateInstanceImpl(unsigned int numExt, const char* extNames[])
{
	VkApplicationInfo appInfo;
	memset(&appInfo, 0, sizeof(appInfo));
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "OpenArena";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "OpenArena";
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceCreateInfo;
	memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = numExt;
	instanceCreateInfo.ppEnabledExtensionNames = extNames;


#ifndef NDEBUG
	const char* validation_layer_name = "VK_LAYER_LUNARG_standard_validation";
	instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledLayerNames = &validation_layer_name;
#endif

    VkResult e = qvkCreateInstance(&instanceCreateInfo, NULL, &vk.instance);
    if(!e)
    {
        ri.Printf(PRINT_ALL, "---Vulkan create instance success---\n\n");
    }
    else if (e == VK_ERROR_INCOMPATIBLE_DRIVER) {
        ri.Error(ERR_FATAL, 
            "Cannot find a compatible Vulkan installable client driver (ICD).\n" );
    }
    else if (e == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        ri.Error(ERR_FATAL, "Cannot find a specified extension library.\n");
    }
    else 
    {
        ri.Error(ERR_FATAL, "%d, returned by qvkCreateInstance.\n", e);
    }
}
*/

/*  

VkResult vkEnumerateInstanceExtensionProperties(
const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties );

pLayerName is either NULL or a pointer to a null-terminated UTF-8 string naming the layer to
retrieve extensions from.

pPropertyCount is a pointer to an integer related to the number of extension properties available
or queried, as described below.

pProperties is either NULL or a pointer to an array of VkExtensionProperties structures.

If pProperties is NULL, then the number of extensions properties available is returned in
pPropertyCount. Otherwise, pPropertyCount must point to a variable set by the user to the number of
elements in the pProperties array, and on return the variable is overwritten with the number of
structures actually written to pProperties. If pPropertyCount is less than the number of extension
properties available, at most pPropertyCount structures will be written. If pPropertyCount is smaller
than the number of extensions available, VK_INCOMPLETE will be returned instead of VK_SUCCESS, to
indicate that not all the available properties were returned.

Because the list of available layers may change externally between calls to 
vkEnumerateInstanceExtensionProperties, two calls may retrieve different results if a pLayerName is
available in one call but not in another. The extensions supported by a layer may also change
between two calls, e.g. if the layer implementation is replaced by a different version between those
calls.



void VKimp_CreateInstance(void)
{
    ri.Printf( PRINT_ALL, " VKimp_CreateInstance() \n" );
	// check extensions availability
	unsigned int instance_extension_count = 0;
    VkBool32 surfaceExtFound = 0;
    VkBool32 platformSurfaceExtFound = 0;
     
    const char* extension_names_supported[64] = {0};
    unsigned int enabled_extension_count = 0;

	VK_CHECK(qvkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL));


    if (instance_extension_count > 0)
    {
        VkExtensionProperties *instance_extensions = 
            (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * instance_extension_count);
        
        VK_CHECK(qvkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions));
            
        unsigned int i = 0;

        for (i = 0; i < instance_extension_count; i++)
        {
            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
            {
                surfaceExtFound = 1;
                extension_names_supported[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
            {
                platformSurfaceExtFound = 1;
                extension_names_supported[enabled_extension_count++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
            }


#ifndef NDEBUG
            if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instance_extensions[i].extensionName))
            {
                extension_names_supported[enabled_extension_count++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
            }
#endif
        }
            
        assert(enabled_extension_count < 64);
            
        free(instance_extensions);
    }

    if (!surfaceExtFound)
		ri.Error(ERR_FATAL, "Vulkan: required instance extension is not available: %s", "surfaceExt");
    if (!platformSurfaceExtFound)
		ri.Error(ERR_FATAL, "Vulkan: required instance extension is not available: %s", "platformSurfaceExt");

    CreateInstanceImpl(enabled_extension_count, extension_names_supported);
}
*/


/*
===============
Minimize the game so that user is back at the desktop
===============
*/
void minimizeWindowImpl( void )
{
    // Hide the window
    xcb_unmap_window(connection, window);

    // Make sure the unmap window command is sent
    xcb_flush(connection);
    //ri.Printf("Not Impled!");
}

// doc

/* 
 * Once we have opened a connection to an X server, 
   we should check some basic information about it: 
   what screens it has, what is the size (width and height) of the screen,
   how many colors it supports, and so on. 
   We get such information from the xcbscreent structure:

    typedef struct {
        xcb_window_t   root;
        xcb_colormap_t default_colormap;
        uint32_t       white_pixel;
        uint32_t       black_pixel;
        uint32_t       current_input_masks;
        uint16_t       width_in_pixels;
        uint16_t       height_in_pixels;
        uint16_t       width_in_millimeters;
        uint16_t       height_in_millimeters;
        uint16_t       min_installed_maps;
        uint16_t       max_installed_maps;
        xcb_visualid_t root_visual;
        uint8_t        backing_stores;
        uint8_t        save_unders;
        uint8_t        root_depth;
        uint8_t        allowed_depths_len;
    } xcb_screen_t;

*/

/*
 * Drawing in a window can be done using various graphical functions
   (drawing pixels, lines, rectangles, etc). In order to draw in a window,
   we first need to define various general drawing parameters, what line 
   width to use, which color to draw with, etc. This is done using a 
   graphical context. A graphical context defines several attributes to be
   used with the various drawing functions. For this, we define a graphical
   context. We can use more than one graphical context with a single window, 
   in order to draw in multiple styles (different colors, line widths, etc).
   In XCB, a Graphics Context is, as a window, characterized by an Id:
*/
