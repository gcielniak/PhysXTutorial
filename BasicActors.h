#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Plane class
	class Plane : public Actor
	{
		PxVec3 normal;
		PxReal distance;

	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 _normal=PxVec3(0.f, 1.f, 0.f), PxReal _distance=0.f) 
			: Actor(PxTransform(PxIdentity)), normal(_normal), distance(_distance)
		{
			PxRigidStatic* plane = PxCreatePlane(*GetPhysics(), PxPlane(normal, distance), *GetDefaultMaterial());
			actor = plane;
		}
	};

	///Sphere class
	class Sphere : public Actor
	{
		PxReal radius;
		PxReal density;
		PxShape* shape;

	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Sphere(PxTransform pose=PxTransform(PxIdentity), PxReal _radius=1.f, PxReal _density=1.f) 
			: Actor(pose), radius(_radius), density(_density)
		{ 
			PxRigidDynamic* sphere = GetPhysics()->createRigidDynamic(pose);
			shape = sphere->createShape(PxSphereGeometry(radius), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*sphere, density);
			actor = sphere;
			colors.push_back(PxVec3(.0f,.0f,.0f));
			shape->userData = &colors.at(0);
		}
	};

	///Box class
	class Box : public Actor
	{
		PxVec3 dimensions;
		PxReal density;
		PxShape* shape;

	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(PxTransform pose=PxTransform(PxIdentity), PxVec3 _dimensions=PxVec3(.5f,.5f,.5f), PxReal _density=1.f) 
			: Actor(pose), dimensions(_dimensions), density(_density)
		{ 
			PxRigidDynamic* box = GetPhysics()->createRigidDynamic(pose);
			shape = box->createShape(PxBoxGeometry(dimensions), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*box, density);
			actor = box;
		}
	};

	class Capsule : public Actor
	{
		PxVec2 dimensions;
		PxReal density;
		PxShape* shape;
	public:
		Capsule(PxTransform pose=PxTransform(PxIdentity), PxVec2 _dimensions=PxVec2(1.f,1.f), PxReal _density=1.f) 
			: Actor(pose), dimensions(_dimensions), density(_density)
		{
			PxRigidDynamic* capsule = GetPhysics()->createRigidDynamic(pose);
			shape = capsule->createShape(PxCapsuleGeometry(dimensions.x, dimensions.y), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*capsule, density);
			actor = capsule;
		}
	};

	///The ConvexMesh class
	class ConvexMesh : public Actor
	{
		PxReal density;
		const PxVec3* verts;
		size_t verts_size;

	public:
		//constructor
		ConvexMesh(const PxVec3* _verts, size_t _verts_size, PxTransform pose=PxTransform(PxIdentity), PxReal _density=1.f)
			: Actor(pose), density(_density), verts(_verts), verts_size(_verts_size)
		{
			PxRigidDynamic* convex_mesh = GetPhysics()->createRigidDynamic(pose);
			PxShape* shape = convex_mesh->createShape(PxConvexMeshGeometry(CookMesh()), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*convex_mesh, density);
			actor = convex_mesh;
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh()
		{
			PxConvexMeshDesc convexDesc;
			convexDesc.points.count     = verts_size/sizeof(PxVec3);
			convexDesc.points.stride    = sizeof(PxVec3);
			convexDesc.points.data      = verts;
			convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;
			convexDesc.vertexLimit      = 256;

			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookConvexMesh(convexDesc, stream))
				throw new Exception("ConvexMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}
	};

	///The TriangleMesh class
	class TriangleMesh : public Actor
	{
		PxReal density;
		const PxVec3* verts;
		size_t verts_size;
		const PxU32* trigs;
		size_t trigs_size;

	public:
		//constructor
		TriangleMesh(const PxVec3* _verts, size_t _verts_size, const PxU32* _trigs, size_t _trigs_size, PxTransform pose=PxTransform(PxIdentity), PxReal _density=1.f)
			: Actor(pose), density(_density), verts(_verts), verts_size(_verts_size), trigs(_trigs), trigs_size(_trigs_size)
		{
			PxRigidStatic* triangle_mesh = GetPhysics()->createRigidStatic(pose);
			PxShape* shape = triangle_mesh->createShape(PxTriangleMeshGeometry(CookMesh()), *GetDefaultMaterial());
			actor = triangle_mesh;
		}

		//mesh cooking (preparation)
		PxTriangleMesh* CookMesh()
		{
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count           = verts_size/sizeof(PxVec3);
			meshDesc.points.stride          = sizeof(PxVec3);
			meshDesc.points.data            = verts;
			meshDesc.triangles.count           = trigs_size/sizeof(PxU32);
			meshDesc.triangles.stride          = 3*sizeof(PxU32);
			meshDesc.triangles.data            = trigs;

			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookTriangleMesh(meshDesc, stream))
				throw new Exception("TriangleMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createTriangleMesh(input);
		}
	};

	///The HeightField class
	class HeightField : public Actor
	{
		PxReal density;

	public:
		//constructor
		HeightField(PxTransform pose=PxTransform(PxIdentity), PxReal _density=1.f,
			const PxVec3& _color=PxVec3(.9f,0.f,0.f))
			: Actor(pose), density(_density)
		{
			PxU32 numRows = 4;
			PxU32 numCols = 4;

			PxHeightFieldSample* samples = new PxHeightFieldSample[numRows*numCols];

			for (PxU32 i = 0; i < numRows*numCols; i++)
				samples[i].height = 0;

			PxHeightFieldDesc hfDesc;
			hfDesc.format             = PxHeightFieldFormat::eS16_TM;
			hfDesc.nbColumns          = numCols;
			hfDesc.nbRows             = numRows;
			hfDesc.samples.data       = samples;
			hfDesc.samples.stride     = sizeof(PxHeightFieldSample);

			PxHeightFieldGeometry hfGeom(GetPhysics()->createHeightField(hfDesc), PxMeshGeometryFlags(), 1.f, 1.f, 1.f);
			PxRigidStatic* hf = GetPhysics()->createRigidStatic(pose);
			PxShape* shape = hf->createShape(hfGeom, *GetDefaultMaterial());
			actor = hf;
			shape->userData = &shape;
		}
	};

	class Cloth : public Actor
	{
		PxVec2 size;
		PxU32 width, height;
		bool fix_top;

	public:
		PxClothMeshDesc meshDesc;

		//constructor
		Cloth(PxTransform pose=PxTransform(PxIdentity), const PxVec2& _size=PxVec2(1.f,1.f), PxU32 _width=1, PxU32 _height=1, bool _fix_top = true, const PxVec3& _color=PxVec3(.9f,0.f,0.f))
			: Actor(pose), size(_size), width(_width), height(_height), fix_top(_fix_top)
		{
			//prepare vertices
			PxReal w_step = size.x/width;
			PxReal h_step = size.y/height;

			PxClothParticle* vertices = new PxClothParticle[(width+1)*(height+1)*4];
			PxU32* primitives = new PxU32[width*height*4];

			for (PxU32 j = 0; j < (height+1); j++)
				for (PxU32 i = 0; i < (width+1); i++)
				{
					PxU32 offset = i + j*(width+1);
					vertices[offset].pos = PxVec3(w_step*i,0.f,h_step*j);
					if (fix_top && (j == 0)) //fix the top row of vertices
						vertices[offset].invWeight = 0.f;
					else
						vertices[offset].invWeight = 1.f;
				}

				for (PxU32 j = 0; j < height; j++)
					for (PxU32 i = 0; i < width; i++)
					{
						PxU32 offset = (i + j*width)*4;
						primitives[offset + 0] = (i+0) + (j+0)*(width+1);
						primitives[offset + 1] = (i+1) + (j+0)*(width+1);
						primitives[offset + 2] = (i+1) + (j+1)*(width+1);
						primitives[offset + 3] = (i+0) + (j+1)*(width+1);
					}

					//init cloth mesh description
					meshDesc.points.data = vertices;
					meshDesc.points.count = (width+1)*(height+1);
					meshDesc.points.stride = sizeof(PxClothParticle);

					meshDesc.invMasses.data = &vertices->invWeight;
					meshDesc.invMasses.count = (width+1)*(height+1);
					meshDesc.invMasses.stride = sizeof(PxClothParticle);

					meshDesc.quads.data = primitives;
					meshDesc.quads.count = width*height;
					meshDesc.quads.stride = sizeof(PxU32) * 4;

					//create cloth fabric (cooking)
					PxClothFabric* fabric = PxClothFabricCreate(*GetPhysics(), meshDesc, PxVec3(0, -1, 0));

					//create cloth
					PxCloth* cloth = GetPhysics()->createCloth(pose, *fabric, vertices, PxClothFlags());
					//collisions with the scene objects
					cloth->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);

					actor = cloth;
					actor->userData = &meshDesc;
		}
	};
}