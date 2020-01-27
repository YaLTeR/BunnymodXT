#!/bin/sh

bindgen ../HLSDK/common/triangleapi.h \
    -o src/ffi/triangle_api_raw.rs \
    -- --target=i686-unknown-linux-gnu

bindgen ../HLSDK/common/cvardef.h \
    -o src/ffi/cvar_raw.rs \
    -- --target=i686-unknown-linux-gnu

bindgen ../hlstrafe/src/hlstrafe.hpp \
	-o src/ffi/hlstrafe_raw.rs \
	--whitelist-function "HLStrafe::MainFunc" \
	--blacklist-type "HLTAS::.*" \
	--blacklist-type "hltas_frame" \
	--blacklist-function "HLTAS::.*" \
	--opaque-type "std::.*" \
	-- \
	--target=i686-unknown-linux-gnu \
	-std=c++14 \
	-I/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/include \
	-I../hlstrafe/hltas/cpp/src
