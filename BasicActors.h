#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Plane class
	class Plane : public StaticActor
	{
	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 normal=PxVec3(0.f, 1.f, 0.f), PxReal distance=0.f) 
			: StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
		{
			AddShape(PxPlaneGeometry());
		}
	};

	///Sphere class
	class Sphere : public DynamicActor
	{
	public:
		//a sphere with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m
		// - denisty: 1kg/m^3
		Sphere(PxTransform pose=PxTransform(PxIdentity), PxReal radius=1.f, PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			AddShape(PxSphereGeometry(radius), density);
		}
	};

	///Box class
	class Box : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(PxTransform pose=PxTransform(PxIdentity), PxVec3 dimensions=PxVec3(.5f,.5f,.5f), PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			AddShape(PxBoxGeometry(dimensions), density);
		}
	};

	class Capsule : public DynamicActor
	{
	public:
		Capsule(PxTransform pose=PxTransform(PxIdentity), PxVec2 dimensions=PxVec2(1.f,1.f), PxReal density=1.f) 
			: DynamicActor(pose)
		{
			AddShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
		}
	};

	///The ConvexMesh class
	class ConvexMesh : public DynamicActor
	{
	public:
		//constructor
		ConvexMesh(const std::vector<PxVec3>& verts, PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f)
			: DynamicActor(pose)
		{
			PxConvexMeshDesc mesh_desc;
			mesh_desc.points.count = verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			mesh_desc.vertexLimit = 256;

			AddShape(PxConvexMeshGeometry(CookMesh(mesh_desc)), density);
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh(const PxConvexMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookConvexMesh(mesh_desc, stream))
				throw new Exception("ConvexMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}
	};

	///The TriangleMesh class
	class TriangleMesh : public StaticActor
	{
	public:
		//constructor
		TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, PxTransform pose=PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			PxTriangleMeshDesc mesh_desc;
			mesh_desc.points.count = verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.triangles.count = trigs.size();
			mesh_desc.triangles.stride = 3*sizeof(PxU32);
			mesh_desc.triangles.data = &trigs.front();

			AddShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
		}

		//mesh cooking (preparation)
		PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookTriangleMesh(mesh_desc, stream))
				throw new Exception("TriangleMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createTriangleMesh(input);
		}
	};

	///The HeightField class
	class HeightField : public StaticActor
	{
	public:
		//constructor
		HeightField(PxTransform pose=PxTransform(PxIdentity))
			: StaticActor(pose)
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

			AddShape(PxHeightFieldGeometry(GetPhysics()->createHeightField(hfDesc),PxMeshGeometryFlags(),1.f,1.f,1.f));
		}
	};

	class Cloth : public Actor
	{
	public:
		PxClothMeshDesc meshDesc;

		//constructor
		Cloth(PxTransform pose=PxTransform(PxIdentity), const PxVec2& size=PxVec2(1.f,1.f), PxU32 width=1, PxU32 height=1, bool fix_top = true)
		{
			//prepare vertices
			PxReal w_step = size.x/width;
			PxReal h_step = size.y/height;

			PxClothParticle* vertices = new PxClothParticle[(width+1)*(height+1)*4];
			PxU32* primitives = new PxU32[width*height*4];

			for (PxU32 j = 0; j < (height+1); j++)
			{
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
				{
					for (PxU32 i = 0; i < width; i++)
					{
						PxU32 offset = (i + j*width)*4;
						primitives[offset + 0] = (i+0) + (j+0)*(width+1);
						primitives[offset + 1] = (i+1) + (j+0)*(width+1);
						primitives[offset + 2] = (i+1) + (j+1)*(width+1);
						primitives[offset + 3] = (i+0) + (j+1)*(width+1);
					}
				}
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