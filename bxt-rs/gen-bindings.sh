#!/bin/sh

bindgen ../HLSDK/common/triangleapi.h \
	-o src/ffi/triangle_api_raw.rs \
	-- --target=i686-unknown-linux-gnu

bindgen ../HLSDK/common/cvardef.h \
	-o src/ffi/cvar_raw.rs \
	-- --target=i686-unknown-linux-gnu
