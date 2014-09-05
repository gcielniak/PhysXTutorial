#include "Renderer.h"
#include <iostream>

using namespace std;

namespace VisualDebugger
{
	namespace Renderer
	{

		const int MAX_NUM_CONVEXMESH_TRIANGLES = 1024;
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxVec3 default_color = PxVec3(0.5f, 0.5f, 0.5f);

		int render_detail = 10;
		bool show_shadows = true;

		static PxU32 gConvexMeshTriIndices[3*MAX_NUM_CONVEXMESH_TRIANGLES];

		static float gPlaneData[]={
			-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};

		void BuildNormals(const PxVec3* PX_RESTRICT vertices, PxU32 numVerts, const PxU16* PX_RESTRICT faces, PxU32 numFaces, PxVec3* PX_RESTRICT normals)
		{
			memset(normals, 0, sizeof(PxVec3)*numVerts);

			const PxU32 numIndices = numFaces*3;

			// accumulate area weighted face normals in each vertex
			for (PxU32 t=0; t < numIndices; t+=3)
			{
				PxU16 i = faces[t];
				PxU16 j = faces[t+1];
				PxU16 k = faces[t+2];

				PxVec3 e1 = vertices[j]-vertices[i];
				PxVec3 e2 = vertices[k]-vertices[i];

				PxVec3 n = e2.cross(e1);

				normals[i] += n;
				normals[j] += n;
				normals[k] += n;
			}

			// average
			for (PxU32 i=0; i < numVerts; ++i)
				normals[i].normalize();
		}

		void RenderGeometry(const PxGeometryHolder& h)
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
					glutSolidSphere(h.sphere().radius, render_detail, render_detail);		
				}
				break;
			case PxGeometryType::ePLANE:
				{
					glScalef(10240,0,10240);
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gPlaneData);
					glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gPlaneData+3);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
				}
				break;
			case PxGeometryType::eCAPSULE:
				{
					const PxF32 radius = h.capsule().radius;
					const PxF32 halfHeight = h.capsule().halfHeight;

					//Sphere
					glPushMatrix();
					glTranslatef(halfHeight,0.0f, 0.0f);
					glScalef(radius,radius,radius);
					glutSolidSphere(1, render_detail, render_detail);		
					glPopMatrix();

					//Sphere
					glPushMatrix();
					glTranslatef(-halfHeight,0.0f,0.0f);
					glScalef(radius,radius,radius);
					glutSolidSphere(1, render_detail, render_detail);		
					glPopMatrix();

					//Cylinder
					glPushMatrix();
					glTranslatef(-halfHeight,0.0f,0.0f);
					glScalef(2.0f*halfHeight,radius,radius);
					glRotatef(90.0f,0.0f,1.0f,0.0f);

					GLUquadric* qobj = gluNewQuadric();
					gluQuadricNormals(qobj, GLU_SMOOTH);
					gluCylinder(qobj, radius, radius, halfHeight, render_detail, render_detail);
					glPopMatrix();
					gluDeleteQuadric(qobj);
				}
				break;
			case PxGeometryType::eCONVEXMESH:
				{
					//Compute triangles for each polygon.
					PxConvexMesh* mesh = h.convexMesh().convexMesh;
					const PxU32 nbPolys = mesh->getNbPolygons();
					const PxU8* polygons = mesh->getIndexBuffer();
					const PxVec3* verts = mesh->getVertices();
					PxU32 numVerts = mesh->getNbVertices();
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

					//compute normals
					PxVec3* norms = new PxVec3[numVerts];

					for (PxU32 i = 0; i < numVerts; i++)
						norms[i] = PxVec3(0.f,0.f,0.f);

					for (PxU32 i = 0; i < 3*numTotalTriangles; i+=3)
					{
						PxVec3 v1 = verts[gConvexMeshTriIndices[i]];
						PxVec3 v2 = verts[gConvexMeshTriIndices[i+1]];
						PxVec3 v3 = verts[gConvexMeshTriIndices[i+2]];
						PxVec3 n = (v2-v1).cross(v3-v1);

						norms[gConvexMeshTriIndices[i]] += n;
						norms[gConvexMeshTriIndices[i+1]] += n;
						norms[gConvexMeshTriIndices[i+2]] += n;
					}

					for (PxU32 i = 0; i < numVerts; i++)
						norms[i].normalize();

					if(numTotalTriangles < MAX_NUM_CONVEXMESH_TRIANGLES)
					{
						glEnableClientState(GL_VERTEX_ARRAY);
						glEnableClientState(GL_NORMAL_ARRAY);
						glVertexPointer(3, GL_FLOAT, 0, verts);
						glNormalPointer(GL_FLOAT, sizeof(PxVec3), norms);
						glDrawElements(GL_TRIANGLES, numTotalTriangles*3, GL_UNSIGNED_INT, gConvexMeshTriIndices);
						glDisableClientState(GL_NORMAL_ARRAY);
						glDisableClientState(GL_VERTEX_ARRAY);
					}
				}
				break;
			default:
				break;
			}
		}

		void reshapeCallback(int width, int height)
		{
			glViewport(0, 0, width, height);
		}

		void idleCallback()
		{
			glutPostRedisplay();
		}

		void InitWindow(const char *name, int width, int height)
		{
			char* namestr = new char[strlen(name)+1];
			strcpy_s(namestr, strlen(name)+1, name);
			int argc = 1;
			char* argv[1] = { namestr };

			glutInit(&argc, argv);

			glutInitWindowSize(width, height);
			glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
			glutSetWindow(glutCreateWindow(name));
			glutReshapeFunc(reshapeCallback);
			glutIdleFunc(idleCallback);

			delete[] namestr;
		}

		void Init()
		{
			// Setup default render states
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_COLOR_MATERIAL);

			// Setup lighting
			glEnable(GL_LIGHTING);
			PxReal ambientColor[]	= { .25f, .25f, .25f, 1.f };
			PxReal diffuseColor[]	= { 1.f, 1.f, 1.f, 1.f };		
			PxReal specularColor[]	= { 1.f, 1.f, 1.f, 1.f };		
			PxReal position[]		= { 1.f, 1.f, 1.f, 1.0f };		
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
			glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			glEnable(GL_LIGHT0);
		}

		void Start(const PxVec3& cameraEye, const PxVec3& cameraDir)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Setup camera
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(cameraEye.x, cameraEye.y, cameraEye.z, cameraEye.x + cameraDir.x, cameraEye.y + cameraDir.y, cameraEye.z + cameraDir.z, 0.0f, 1.0f, 0.0f);
		}

		void Render(PxRigidActor** actors, const PxU32 numActors)
		{
			PxVec3 shadow_color = default_color*0.6;
			PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
			for(PxU32 i=0;i<numActors;i++)
			{
				const PxU32 nbShapes = actors[i]->getNbShapes();
				PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
				actors[i]->getShapes(shapes, nbShapes);
				bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false;

				for(PxU32 j=0;j<nbShapes;j++)
				{
					PxTransform pose = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);
					PxGeometryHolder h = shapes[j]->getGeometry();
					//move the plane slightly down to avoid visual artefacts
					if (h.getType() == PxGeometryType::ePLANE)
					{
						pose.q *= PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f));
						pose.p += PxVec3(0,-0.01,0);
					}

					PxMat44 shapePose(pose);

					// render object
					glPushMatrix();						
					glMultMatrixf((float*)&shapePose);

					PxVec3 actor_color = default_color;

					if (actors[i]->userData)
					{
						actor_color = *((PxVec3*)actors[i]->userData);
						if (h.getType() == PxGeometryType::ePLANE)
						{
							shadow_color = actor_color*0.6;
						}
					}

					if (h.getType() == PxGeometryType::ePLANE)
						glDisable(GL_LIGHTING);

					glColor4f(actor_color.x, actor_color.y, actor_color.z, 1.0f);

					RenderGeometry(h);

					if (h.getType() == PxGeometryType::ePLANE)
						glEnable(GL_LIGHTING);

					glPopMatrix();

					if(show_shadows && (h.getType() != PxGeometryType::ePLANE))
					{
						const PxVec3 shadowDir(-0.7071067f, -0.7071067f, -0.7071067f);
						const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
						glPushMatrix();						
						glMultMatrixf(shadowMat);
						glMultMatrixf((float*)&shapePose);
						glDisable(GL_LIGHTING);
						glColor4f(shadow_color.x, shadow_color.y, shadow_color.z, 1.0f);
						RenderGeometry(h);
						glEnable(GL_LIGHTING);
						glPopMatrix();
					}
				}
			}
		}

		void Render(PxActor** actors, const PxU32 numActors)
		{
			for(PxU32 i = 0; i < numActors; i++)
			{
				if (actors[i]->isCloth())
				{
					PxCloth* cloth = (PxCloth*)actors[i];

					PxClothMeshDesc* mesh_desc = (PxClothMeshDesc*)cloth->userData;

					PxU32 quad_count = mesh_desc->quads.count;
					PxU32* quads = (PxU32*)mesh_desc->quads.data;

					//get verts data
					cloth->lockParticleData();
					PxClothParticleData* readData = cloth->lockParticleData();
					if (!readData)
						break;

					// copy vertex positions
					PxU32 numVerts = cloth->getNbParticles();
					PxVec3* verts = new PxVec3[numVerts];
					PxVec3* norms = new PxVec3[numVerts];
					for (PxU32 j = 0; j < numVerts; j++)
						verts[j] = readData->particles[j].pos;

					readData->unlock();

					//Compute normals
					for (PxU32 i = 0; i < numVerts; i++)
						norms[i] = PxVec3(0.f,0.f,0.f);

					for (PxU32 i = 0; i < quad_count*4; i+=4)
					{
						PxVec3 v1 = verts[quads[i]];
						PxVec3 v2 = verts[quads[i+1]];
						PxVec3 v3 = verts[quads[i+2]];
						PxVec3 n = -((v2-v1).cross(v3-v1));

						norms[quads[i]] += n;
						norms[quads[i+1]] += n;
						norms[quads[i+2]] += n;
						norms[quads[i+3]] += n;
					}

					for (PxU32 i = 0; i < numVerts; i++)
						norms[i].normalize();

					PxTransform pose = cloth->getGlobalPose();
					PxMat44 shapePose(pose);

					glEnable(GL_LIGHTING);

					glPushMatrix();						
					glMultMatrixf((float*)&shapePose);
					glColor4f(0.9f, 0.f, 0.9f, 1.0f);

					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);

					glVertexPointer(3, GL_FLOAT, sizeof(PxVec3), verts);
					glNormalPointer(GL_FLOAT, sizeof(PxVec3), norms);

					glDrawElements(GL_QUADS, quad_count*4, GL_UNSIGNED_INT, quads);

					glDisableClientState(GL_NORMAL_ARRAY);
					glDisableClientState(GL_VERTEX_ARRAY);

					glPopMatrix();

				}
			}
		}

		void Finish()
		{
			glutSwapBuffers();
		}

		void SetRenderDetail(int value)
		{
			render_detail = value;
		}

		void ShowShadows(bool value)
		{
			show_shadows = value;
		}

		bool ShowShadows() { return show_shadows; }

		void RenderBuffer(float* pVertList, float* pColorList, int type, int num)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT, 0, pVertList);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, pColorList);
			glDrawArrays(type, 0, num);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}


		///Render PxRenderBuffer
		///TODO: support text data
		void Render(const PxRenderBuffer& data)
		{
			glLineWidth(1.0f);

			//render points

			unsigned int NbPoints = data.getNbPoints();
			if(NbPoints)
			{
				float* pVertList = new float[NbPoints*3];
				float* pColorList = new float[NbPoints*4];
				int vertIndex = 0;
				int colorIndex = 0;
				const physx::PxDebugPoint* Points = data.getPoints();
				while(NbPoints--)
				{
					pVertList[vertIndex++] = Points->pos.x;
					pVertList[vertIndex++] = Points->pos.y;
					pVertList[vertIndex++] = Points->pos.z;
					pColorList[colorIndex++] = (float)((Points->color>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Points->color>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Points->color&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;
					Points++;
				}

				RenderBuffer(pVertList, pColorList, GL_POINTS, data.getNbPoints());

				delete[] pVertList;
				delete[] pColorList;
			}

			//render lines

			unsigned int NbLines = data.getNbLines();
			if(NbLines)
			{
				float* pVertList = new float[NbLines*3*2];
				float* pColorList = new float[NbLines*4*2];
				int vertIndex = 0;
				int colorIndex = 0;
				const PxDebugLine* Lines = data.getLines();
				while(NbLines--)
				{
					pVertList[vertIndex++] = Lines->pos0.x;
					pVertList[vertIndex++] = Lines->pos0.y;
					pVertList[vertIndex++] = Lines->pos0.z;
					pColorList[colorIndex++] = (float)((Lines->color0>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Lines->color0>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Lines->color0&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;

					pVertList[vertIndex++] = Lines->pos1.x;
					pVertList[vertIndex++] = Lines->pos1.y;
					pVertList[vertIndex++] = Lines->pos1.z;
					pColorList[colorIndex++] = (float)((Lines->color1>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Lines->color1>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Lines->color1&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;

					Lines++;
				}

				RenderBuffer(pVertList, pColorList, GL_LINES, data.getNbLines()*2);

				delete[] pVertList;
				delete[] pColorList;
			}

			//render triangles

			unsigned int NbTris = data.getNbTriangles();
			if(NbTris)
			{
				float* pVertList = new float[NbTris*3*3];
				float* pColorList = new float[NbTris*4*3];
				int vertIndex = 0;
				int colorIndex = 0;
				const PxDebugTriangle* Triangles = data.getTriangles();
				while(NbTris--)
				{
					pVertList[vertIndex++] = Triangles->pos0.x;
					pVertList[vertIndex++] = Triangles->pos0.y;
					pVertList[vertIndex++] = Triangles->pos0.z;

					pVertList[vertIndex++] = Triangles->pos1.x;
					pVertList[vertIndex++] = Triangles->pos1.y;
					pVertList[vertIndex++] = Triangles->pos1.z;

					pVertList[vertIndex++] = Triangles->pos2.x;
					pVertList[vertIndex++] = Triangles->pos2.y;
					pVertList[vertIndex++] = Triangles->pos2.z;

					pColorList[colorIndex++] = (float)((Triangles->color0>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Triangles->color0>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Triangles->color0&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;

					pColorList[colorIndex++] = (float)((Triangles->color1>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Triangles->color1>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Triangles->color1&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;

					pColorList[colorIndex++] = (float)((Triangles->color2>>16)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)((Triangles->color2>>8)&0xff)/255.0f;
					pColorList[colorIndex++] = (float)(Triangles->color2&0xff)/255.0f;
					pColorList[colorIndex++] = 1.0f;

					Triangles++;
				}

				RenderBuffer(pVertList, pColorList, GL_TRIANGLES, data.getNbTriangles()*3);

				delete[] pVertList;
				delete[] pColorList;
			}

			//TODO: render texts ?
		}

		void RenderText(const std::string& text, const physx::PxVec2& location, 
			const PxVec3& color, PxReal size)
		{
			glColor4f(color.x, color.y, color.z, 1.f);
			GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			GLFontRenderer::print(location.x, location.y, size, text.c_str());
		}
	}
}
