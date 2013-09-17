// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PHYSX_SNIPPET_RENDER_H
#define PHYSX_SNIPPET_RENDER_H

#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#if defined(PX_WINDOWS)
#include <windows.h>
#pragma warning(disable: 4505)
#include <glut.h>
#elif defined(PX_LINUX)
#include <GL/glut.h>
#elif defined(PX_APPLE) && !defined(PX_APPLE_IOS)
#include <GLUT/glut.h>
#elif defined(PX_PS3)
#include <GL/glut.h>
#else
#error platform not supported.
#endif

namespace Snippets
{
void setupDefaultWindow(const char* name);
void setupDefaultRenderState();

void startRender(const physx::PxVec3& cameraEye, const physx::PxVec3& cameraDir);
void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows = false);
void finishRender();
}

#define MAX_NUM_ACTOR_SHAPES 128

#endif //PHYSX_SNIPPET_RENDER_H
