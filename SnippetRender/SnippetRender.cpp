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

#define _HAS_EXCEPTIONS 0
#include "SnippetRender.h"

using namespace physx;

static float gCylinderData[]={
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
	0.866025f,0.500000f,1.0f,0.866025f,0.500000f,1.0f,0.866025f,0.500000f,0.0f,0.866025f,0.500000f,0.0f,
	0.500000f,0.866025f,1.0f,0.500000f,0.866025f,1.0f,0.500000f,0.866025f,0.0f,0.500000f,0.866025f,0.0f,
	-0.0f,1.0f,1.0f,-0.0f,1.0f,1.0f,-0.0f,1.0f,0.0f,-0.0f,1.0f,0.0f,
	-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,0.0f,-0.500000f,0.866025f,0.0f,
	-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,0.0f,-0.866025f,0.500000f,0.0f,
	-1.0f,-0.0f,1.0f,-1.0f,-0.0f,1.0f,-1.0f,-0.0f,0.0f,-1.0f,-0.0f,0.0f,
	-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,0.0f,-0.866025f,-0.500000f,0.0f,
	-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,0.0f,-0.500000f,-0.866025f,0.0f,
	0.0f,-1.0f,1.0f,0.0f,-1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,-1.0f,0.0f,
	0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,0.0f,0.500000f,-0.866025f,0.0f,
	0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,0.0f,0.866026f,-0.500000f,0.0f,
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f
};

static float gCylinderDataCapsTop[]={
	0.866026f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,-1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,-1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,-0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-1.000000f,-0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-1.000000f,-0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	-0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.500000f,0.866025f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866025f,0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	1.000000f,0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	1.000000f,0.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.000000f,1.000000f,1.000000f,0.000000f,1.000000f,1.000000f,
	0.866026f,-0.500000f,1.000000f,0.000000f,1.000000f,1.000000f,
};

static float gCylinderDataCapsBottom[]={
	1.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.000000f,1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.000000f,1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-1.000000f,-0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-1.000000f,-0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.866025f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	-0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,-1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,-1.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.500000f,-0.866025f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866026f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.866026f,-0.500000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	0.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,0.000000f,-1.000000f,0.000000f,
};

#define MAX_NUM_CONVEXMESH_TRIANGLES 1024
static PxU32 gConvexMeshTriIndices[3*MAX_NUM_CONVEXMESH_TRIANGLES];

void renderGeometry(const PxGeometryHolder& h)
{
	switch(h.getType())
	{
	case PxGeometryType::eBOX:			
		{
			glScalef(h.box().halfExtents.x, h.box().halfExtents.y, h.box().halfExtents.z);
			glutSolidCube(2.0f);		
		}
		break;
	case PxGeometryType::eSPHERE:		
		{
			glutSolidSphere(h.sphere().radius, 10, 10);		
		}
		break;
	case PxGeometryType::eCAPSULE:
		{

			const PxF32 radius = h.capsule().radius;
			const PxF32 halfHeight = h.capsule().halfHeight;

			//Sphere
			glPushMatrix();
			glTranslatef(0.0f, halfHeight, 0.0f);
			glScalef(radius,radius,radius);
			glutSolidSphere(1, 10, 10);		
			glPopMatrix();

			//Sphere
			glPushMatrix();
			glTranslatef(0.0f,-halfHeight, 0.0f);
			glScalef(radius,radius,radius);
			glutSolidSphere(1, 10, 10);		
			glPopMatrix();

			//Cylinder
			glPushMatrix();
			glTranslatef(0.0f,halfHeight, 0.0f);
			glScalef(radius,2.0f*halfHeight,radius);
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderData);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderData+3);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 13*2);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsTop);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsTop+3);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsBottom);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderDataCapsBottom+3);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glPopMatrix();
		}
		break;
	case PxGeometryType::eCONVEXMESH:
		{
			//Compute triangles for each polygon.
			PxConvexMesh* mesh = h.convexMesh().convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 numTotalTriangles = 0;
			for(PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = data.mNbVerts - 2;
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				for(PxU32 j=0;j<nbTris;j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];
					if(numTotalTriangles < MAX_NUM_CONVEXMESH_TRIANGLES)
					{
						gConvexMeshTriIndices[3*numTotalTriangles + 0] = vref0;
						gConvexMeshTriIndices[3*numTotalTriangles + 1] = vref1;
						gConvexMeshTriIndices[3*numTotalTriangles + 2] = vref2;
						numTotalTriangles++;
					}
				}
			}

			if(numTotalTriangles < MAX_NUM_CONVEXMESH_TRIANGLES)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, verts);
				glDrawElements(GL_TRIANGLES, numTotalTriangles*3, GL_UNSIGNED_INT, gConvexMeshTriIndices);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
		break;
	default:
		break;
	}
}

namespace Snippets
{

namespace
{
void reshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}
}

void setupDefaultWindow(const char *name)
{
	char* namestr = new char[strlen(name)+1];
	strcpy(namestr, name);
	int argc = 1;
	char* argv[1] = { namestr };

	glutInit(&argc, argv);
	
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow(name);
	glutSetWindow(mainHandle);
	glutReshapeFunc(reshapeCallback);
	
	delete[] namestr;
}

void setupDefaultRenderState()
{
	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Setup lighting
	glEnable(GL_LIGHTING);
	PxReal ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
	PxReal diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	PxReal specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	PxReal position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
}


void startRender(const PxVec3& cameraEye, const PxVec3& cameraDir)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraEye.x, cameraEye.y, cameraEye.z, cameraEye.x + cameraDir.x, cameraEye.y + cameraDir.y, cameraEye.z + cameraDir.z, 0.0f, 1.0f, 0.0f);

	glColor4f(0.4f, 0.4f, 0.4f, 1.0f); 
}

void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
{
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	for(PxU32 i=0;i<numActors;i++)
	{
		const PxU32 nbShapes = actors[i]->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actors[i]->getShapes(shapes, nbShapes);
		bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false; 

		for(PxU32 j=0;j<nbShapes;j++)
		{
//			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j]));
			PxGeometryHolder h = shapes[j]->getGeometry();
			
			// render object
			glPushMatrix();						
			glMultMatrixf((float*)&shapePose);
			if(sleeping)
				glColor4f(0.75f, 1.0f, 0.75f, 1.0f);
			else
				glColor4f(0.0f, 0.75f, 0.0f, 1.0f);
			renderGeometry(h);
			glPopMatrix();

			if(shadows)
			{
				const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
				const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
				glPushMatrix();						
				glMultMatrixf(shadowMat);
				glMultMatrixf((float*)&shapePose);
				glDisable(GL_LIGHTING);
				glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
				renderGeometry(h);
				glEnable(GL_LIGHTING);
				glPopMatrix();
			}
		}
	}
}

void finishRender()
{
	glutSwapBuffers();
}


} //namespace Snippets

