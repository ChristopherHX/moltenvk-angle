
# Define target name
set (TARGET_NAME 66_SparkDemo)

add_compile_options(-std=c++11)

file (GLOB CPP_FILES *.cpp)
	
# ThirdParty
#add_subdirectory(ThirdParty/Spark)
file(GLOB_RECURSE SPARK_SRC_FILES
	ThirdParty/Spark/include/Core/*.h
	ThirdParty/Spark/include/Extensions/*.h
	ThirdParty/Spark/src/Core/*.cpp
	ThirdParty/Spark/src/Extensions/*.cpp
)



# Urhox core
#add_subdirectory(Urhox)
file(GLOB_RECURSE URHOX_SRC_FILES
	Urhox/*.h
        Urhox/Spark/*.h
        Urhox/Spark/*.cpp
)


source_group("Spark"            FILES ${SPARK_SRC_FILES})
source_group("Urhox"            FILES ${URHOX_SRC_FILES})

set (INCLUDE_DIRS ${INCLUDE_DIRS} . ThirdParty/Spark/include )
#D:\Urho3D-Dev\dev\Urho3D\Source\Samples\66_SparkDemo\ThirdParty\Spark\include
# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ${CPP_FILES} ${URHOX_SRC_FILES}  ${SPARK_SRC_FILES})



# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()
