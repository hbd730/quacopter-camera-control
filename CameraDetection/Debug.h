//
//  Debug.h
//  CameraDetection
//
//  Created by Peter Huang on 21/04/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//

#pragma once

#define CFReleaseSafe(CF) { CFTypeRef _cf = (CF); if (_cf){ (CF) = NULL; CFRelease(_cf); } }

#define BAIL_IF(cond, fmt, ...)                                                 \
if (cond)                                                                       \
{                                                                               \
fprintf(stderr, "*** %s[%d]: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
goto bail;                                                                  \
}
