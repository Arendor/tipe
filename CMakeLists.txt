# CMakeList.txt : projet CMake pour tipe_shotten_totten, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.18)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("tipe_shotten_totten")

#set(CMAKE_PREFIX_PATH "C:/Users/arend/armadillo/armadillo-15.0.3")
#set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/Intel/oneAPI/mkl/2025.2")

list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/Intel/oneAPI/mkl/2025.3;C:/Program Files (x86)/Intel/oneAPI/compiler/2025.3")# "C:/Program Files (x86)/Intel/oneAPI/compiler/2025.2")
set(BLA_VENDOR Intel10_64lp)#Intel10_64lp_seq #Intel10_64lp
#set(BLAS_DIR "C:/Program Files (x86)/Intel/oneAPI/mkl/2025.2/lib/mkl_intel_lp64_dll.lib")
find_package(BLAS REQUIRED)
set(BLA_VENDOR Intel10_64lp)
find_package(LAPACK REQUIRED)


if (NOT BLAS_FOUND)
    message(FATAL_ERROR "blas not found")
endif()

#if(NOT BLAS)
#    message(FATAL_ERROR "lapack not found")
#endif()
#set(MKL_DIR "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.3.279/windows/mkl/lib/intel64_win")
#target_link_libraries(project_x PUBLIC "${MKL_DIR}/mkl_intel_lp64")

list(APPEND CMAKE_PREFIX_PATH "C:/Armadillo/armadillo-15.0.3")
find_package(Armadillo REQUIRED)


if(Armadillo_FOUND AND NOT TARGET Armadillo::Armadillo)
  add_library(Armadillo::Armadillo INTERFACE IMPORTED)
  set_target_properties(
    Armadillo::Armadillo
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "${ARMADILLO_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${ARMADILLO_INCLUDE_DIRS}"
  )
endif()


# soon libtorch-debug
#list(APPEND CMAKE_PREFIX_PATH "C:/Program Files/LibTorch/libtorch")
#find_package(Torch REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


#[[include(FetchContent)
FetchContent_Declare(dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG        v19.24
)
FetchContent_MakeAvailable(dlib)
]]

include(FetchContent)
FetchContent_Declare(dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG        v19.24
)
FetchContent_MakeAvailable(dlib)


#SET(MKL_VERBOSE 1)
# Ajoutez une source à l'exécutable de ce projet.
add_executable (tipe_shotten_totten "tipe_shotten_totten.cpp" "tipe_shotten_totten.h" "src/game/game_engine.cpp" "src/game/game_engine.h" "src/player/player.h" "src/player/player.cpp" "src/player/agent.cpp" "src/player/agent.h" "src/utils/utils.h" 
"src/game/color_manager.cpp" "src/game/color_manager.h" "src/game/sc_manager.h" "src/game/sc_manager.cpp" "src/utils/utils.cpp" "src/policy/policy.cpp" "src/policy/policy.h" "src/player/console_player.h" "src/player/console_player.cpp" 
 "src/utils/deck.h"   "src/policy_save/dqn_policy.h" "src/policy_save/dqn_policy.cpp" "neural_network/functions/activation_function.h" "neural_network/functions/activation_function.cpp" "neural_network/main/Layer.h" 
 "neural_network/main/Layer.cpp" "neural_network/main/Network.cpp" "neural_network/main/Network.h"  "neural_network/main/Network.cpp" "neural_network/utils/utils_nn.cpp" "neural_network/utils/utils_nn.h" 
 "neural_network/utils/custom_errors.h" "neural_network/utils/custom_errors.cpp" "src/policy/ReplayBuffer.h"  "src/policy/dqn_policy_n.cpp" "src/policy/dqn_policy_n.h"
"src/policy/abr_policy.cpp" "src/policy/abr_policy.h")

# Pour utiliser DLIB :
target_compile_definitions(tipe_shotten_totten PUBLIC ARMA_DONT_EXTERNAL_LIB_PROTOTYPES ARMA_DONT_USE_LAPACK ARMA_DONT_USE_BLAS)

#JSON
target_include_directories(tipe_shotten_totten PUBLIC "C:/Users/arend/AppData/Local/jsonparser/include")

#target_include_directories(tipe_shotten_totten PUBLIC "C:/Program Files (x86)/Intel/oneAPI/mkl/2025.3/include")


target_link_libraries(tipe_shotten_totten PUBLIC Armadillo::Armadillo #[["${TORCH_LIBRARIES}"]] dlib::dlib) 


#target_include_directories(tipe_shotten_totten PUBLIC "C:/Program Files (x86)/cereal/cereal-1.3.2/include/" "C:/Program Files (x86)/ensmallen/ensmallen-3.11.0/include")

#if(MSVC)
#    target_compile_options(tipe_shotten_totten PUBLIC "/O2")
#endif()

#[[
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET tipe_shotten_totten
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:tipe_shotten_totten>)
endif (MSVC)]]

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tipe_shotten_totten PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
