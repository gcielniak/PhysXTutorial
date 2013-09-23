#include "Renderer.h"
#include "UserData.h"

namespace VisualDebugger
{
	namespace Renderer
	{

		const int MAX_NUM_CONVEXMESH_TRIANGLES = 1024;

		static PxU32 gConvexMeshTriIndices[3*MAX_NUM_CONVEXMESH_TRIANGLES];

		const int MAX_NUM_ACTOR_SHAPES = 128;

		bool show_shadows = true;

		PxVec3 default_color = PxVec3(0.5f, 0.6f, 0.7f);

		int render_detail = 10;

		static float gPlaneData[]={
			-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};

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
					glTranslatef(0.0f, halfHeight, 0.0f);
					glScalef(radius,radius,radius);
					glutSolidSphere(1, render_detail, render_detail);		
					glPopMatrix();

					//Sphere
					glPushMatrix();
					glTranslatef(0.0f,-halfHeight, 0.0f);
					glScalef(radius,radius,radius);
					glutSolidSphere(1, render_detail, render_detail);		
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
			glClearColor(default_color.x, default_color.y, default_color.z, 1.0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_COLOR_MATERIAL);

			// Setup lighting
			glEnable(GL_LIGHTING);
			PxReal ambientColor[]	= { 0.5f, 0.5f, 0.5f, 0.0f };
			PxReal diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
			PxReal specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
			PxReal position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
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

		void AdjustSaturation(PxVec3& color, float value)
		{
			const float Pr = .299;
			const float Pg = .587;
			const float Pb = .114;

			double P = sqrt(color.x*color.x*Pr + color.y*color.y*Pg + color.z*color.z*Pb);

			color.x = P + (color.x-P)*value;
			color.y = P + (color.y-P)*value;
			color.z = P + (color.z-P)*value;
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
					if (h.getType() == PxGeometryType::ePLANE)
					{
						pose.p += PxVec3(0,-0.01,0);
						pose.q *= PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f));
					}

					PxMat44 shapePose(pose);

					// render object
					glPushMatrix();						
					glMultMatrixf((float*)&shapePose);

					PxVec3 actor_color = default_color;

					if (actors[i]->userData)
					{
						actor_color = ((UserData*)actors[i]->userData)->color;
						if (h.getType() == PxGeometryType::ePLANE)
							shadow_color = actor_color*0.6;
					}

					if(sleeping)
						AdjustSaturation(actor_color, 0.7);

					glColor4f(actor_color.x, actor_color.y, actor_color.z, 1.0f);

					RenderGeometry(h);
					glPopMatrix();

					if(show_shadows && (h.getType() != PxGeometryType::ePLANE))
					{
						const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
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
	}
}
