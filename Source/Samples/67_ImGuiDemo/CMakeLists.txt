if (NOT ANDROID AND NOT WEB  AND NOT IOS AND NOT TVOS)
# Define target name
set (TARGET_NAME 67_ImGuiDemo)

add_compile_options(-std=c++11)

file (GLOB CPP_FILES *.h *.cpp)


# ThirdParty
set(IMGUI_SRC_FILES
	ThirdParty/ImGui/imconfig.h
	ThirdParty/ImGui/imgui.h
	ThirdParty/ImGui/imgui_internal.h
	ThirdParty/ImGui/imgui.cpp
	ThirdParty/ImGui/imgui_demo.cpp
	ThirdParty/ImGui/imgui_draw.cpp
	ThirdParty/ImGui/imgui_widgets.cpp
)


## ThirdParty ImGuizmo
file(GLOB_RECURSE IMGUIZMO_SRC_FILES
	ThirdParty/ImGuizmo/*.h
	ThirdParty/ImGuizmo/*.cpp
)

## ThirdParty ImNodes
file(GLOB_RECURSE IMNODES_SRC_FILES
	ThirdParty/ImNodes/imnodes.h
	ThirdParty/ImNodes/imnodes.cpp
)

# Urhox core
#add_subdirectory(Urhox)
file(GLOB_RECURSE URHOX_SRC_FILES
		Urhox/*.h
        Urhox/SystemUI/*.h
        Urhox/SystemUI/*.cpp
)


source_group("ImGui"            FILES  ${IMGUI_SRC_FILES})
source_group("ImGuizmo"            FILES ${IMGUIZMO_SRC_FILES})
source_group("ImNodes"            FILES ${IMNODES_SRC_FILES})
source_group("Urhox"            FILES ${URHOX_SRC_FILES})

set (INCLUDE_DIRS ${INCLUDE_DIRS} . ThirdParty ThirdParty/ImGui ThirdParty/ImGuizmo ThirdParty/ImGuizmo)

# Define source files
define_source_files (EXTRA_H_FILES  ${COMMON_SAMPLE_H_FILES} ${CPP_FILES} ${URHOX_SRC_FILES}  ${IMGUI_SRC_FILES} ${IMGUIZMO_SRC_FILES} ${IMNODES_SRC_FILES})



# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()

endif ()
