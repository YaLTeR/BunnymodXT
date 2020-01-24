#!/bin/sh

exec bindgen ../HLSDK/common/triangleapi.h \
	-o src/ffi/triangle_api_raw.rs \
	-- --target=i686-unknown-linux-gnu
