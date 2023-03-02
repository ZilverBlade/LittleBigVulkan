workspace "LittleBigVulkan"
   architecture "x86_64"
   preferredtoolarchitecture "x86_64"
   configurations { "Debug", "Release" }

local outputSubfolder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
local outputSubfolderShared = "./lib"
local VULKAN_SDK = os.getenv("VULKAN_SDK")
   
   
local univInclude = {
	  VULKAN_SDK.."/Include",
	  "vendor/glfw-3.3.7/include",
      "vendor/imgui/include",
	  "vendor/simpleini/include",
	  "vendor/simdjson/include",
	  "vendor/entt/include"
 }
project "LittleBigVulkanEngine"
   location "./LBVEngine"
   debugdir "./"
   local bin = outputSubfolderShared
   targetdir (bin)

   kind "SharedLib"
   language "C++"
   cppdialect "C++17"

   flags { "MultiProcessorCompile" }
   disablewarnings { "26812;4244;4996;4005" }
   
   files {
	  "LBVEngine/src/lbv/**.cpp",
	  "LBVEngine/src/lbv/**.h",
	  
      "LBVEngine/src/lbv/vkapi/**.*",
      "LBVEngine/src/lbv/rendering/**.*",
      "LBVEngine/src/lbv/systems/**.*",
      "LBVEngine/src/lbv/graphics/**.*",
      "LBVEngine/src/lbv/ecs/**.*",
      "LBVEngine/src/lbv/utils/**.*",
   }

   includedirs {
	  "./LBVEngine/src",
	  "vendor/stbimage/include",
	  "vendor/assimp/include",
	  univInclude
   }
   libdirs {
      VULKAN_SDK.."/Lib",
	  "vendor/glfw-3.3.7/lib-vc2019", 
	  "vendor/assimp/lib",
	  "vendor/simdjson/lib"
   }
   links {
      "vulkan-1",
	  "glfw3dll",
   }

   defines { "LBVENGINEAPI_DLLEXPORT" }
   
   filter { "system:windows" }
      links {
	   "shlwapi"
      }
	
   filter "configurations:Debug"
      defines { "_DEBUG" }
      runtime "Debug"
      symbols "On"
	  targetname ("lbvengine_d")
	  links {
	  "assimp-vc143-mtd",
	  "simdjson_d",
	  }

   filter "configurations:Release"
	  defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"
	  targetname ("lbvengine")
	  links {
	  "assimp-vc143-mt",
	  "simdjson",
	  }
project "LittleBigVulkanDev"
   location "./LBVDev"
   debugdir "./"
   local bin = "./bin/"..outputSubfolder

   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir (bin)

   flags { "MultiProcessorCompile" }
   disablewarnings { "26812;4244;4996;4005" }
   
   files {
	  "LBVDev/src/lbvd/**.cpp",
	  "LBVDev/src/lbvd/**.h",
	  
      "LBVDev/src/lbvd/apps/**.*"
   }

   includedirs {
	  "./LBVEngine/src",
	  "./LBVDev/src",
	  univInclude
   }
   
   libdirs {
	  "./lib",
	  "vendor/glfw-3.3.7/lib-vc2019", 
	  "vendor/imgui/lib",
   }
   links {
	  "glfw3dll",
	  "LittleBigVulkanEngine",
   }
   filter "configurations:Debug"
      defines { "_DEBUG" }
      runtime "Debug"
      symbols "On"
	  links {
		"lbvengine_d",
		"ImGui_d",
	  }

   filter "configurations:Release"
	  defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"
	  links {
	  "lbvengine",
	  "ImGui",
	  }