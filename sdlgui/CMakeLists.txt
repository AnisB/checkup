cmake_minimum_required(VERSION 3.2)

# The defines we need for the library
set(SDL_GUI_INCLUDE "${SDL_GUI_ROOT}/include")
set(SDL_GUI_SOURCE "${SDL_GUI_ROOT}/src")

bento_headers(tmp_header_list "${SDL_GUI_INCLUDE}/${header_dir}" "${header_dir}")
list(APPEND header_files "${tmp_header_list}")

bento_sources(tmp_source_list "${SDL_GUI_SOURCE}/${source_dir}" "${source_dir}")
list(APPEND source_files "${tmp_source_list}")

# Generate the static library
bento_static_lib("sdlgui" "sdlgui" "${header_files};${source_files};" "${SDL_GUI_INCLUDE};${SDL2_INCLUDE_DIRS}/SDL")
