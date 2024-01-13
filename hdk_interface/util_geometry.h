#ifndef HINAPE_UTIL_GEOMETRY_H
#define HINAPE_UTIL_GEOMETRY_H

#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>

#include <GA/GA_GBMacros.h>
#include <GA/GA_Edge.h>
#include <GA/GA_PrimitiveTypes.h>

#include <GEO/GEO_PrimPoly.h>

#include <GU/GU_Detail.h>
#include <GU/GU_TriangleMesh.h>
#include <GU/GU_PolyWire.h>
#include <GU/GU_EdgeMesh.h>

#include <iostream>
#include <vector>
#include <set>
#include <utility>

template<typename T>
bool ExtractTriangleMesh(const SIM_Geometry *geo, GU_TriangleMeshT<T> &Out_mesh)
{
	if (!geo) return false;

	SIM_GeometryAutoReadLock lock(geo); // do we actually need lock this? or geo->getGeometry().gdp();
	const GU_Detail *gdp = lock.getGdp();

	Out_mesh = GU_TriangleMeshT<T>(gdp);
	Out_mesh.buildMesh();

	const GA_OffsetArray &polys = Out_mesh.polys();
	for (const auto offset: polys)
	{
		std::cout << offset << " ";
	}
	std::cout << std::endl;
}

template<typename T>
bool ConvertPolyMeshToWireframe(SIM_GeometryCopy *geo)
{
	if (!geo) return false;

	SIM_GeometryAutoWriteLock lock(geo);
	GU_Detail &gdp = lock.getGdp();

	ConvertPolyMeshToWireframe<T>(gdp);
}

template<typename T>
bool ConvertPolyMeshToWireframe(GU_Detail &gdp)
{
	std::map<GA_Offset, UT_Vector3> cache_points;
	GA_Offset ptoff;
	GA_ROHandleV3 P_h(gdp.getP());
	GA_FOR_ALL_PTOFF(&gdp, ptoff)
		{
			cache_points[ptoff] = P_h(ptoff);
		};

	using Edge = std::pair<GA_Offset, GA_Offset>;
	struct EdgeCompare
	{
		bool operator()(const Edge &a, const Edge &b) const
		{
			return a.first < b.first || (a.first == b.first && a.second < b.second);
		}
	};
	std::set<Edge, EdgeCompare> temp_edges;
	GEO_Primitive *prim;
	GA_FOR_ALL_PRIMITIVES(&gdp, prim)
	{
		const GA_Size num_vertices = prim->getVertexCount();
		for (GA_Size i = 0; i < num_vertices; ++i)
		{
			GA_Offset v0 = prim->getVertexOffset(i);
			GA_Offset v1 = prim->getVertexOffset((i + 1) % num_vertices);

			GA_Offset p0 = gdp.vertexOffset(v0);
			GA_Offset p1 = gdp.vertexOffset(v1);

			if (p0 > p1) std::swap(p0, p1);

			temp_edges.insert(std::make_pair(p0, p1));
		}
	};

	gdp.clearAndDestroy();

	for (const Edge &edge: temp_edges)
	{
		GA_Offset start_pt = gdp.appendPoint();
		GA_Offset end_pt = gdp.appendPoint();

		gdp.setPos3(start_pt, cache_points[edge.first]);
		gdp.setPos3(end_pt, cache_points[edge.second]);

		GEO_PrimPoly *new_poly = (GEO_PrimPoly *) gdp.appendPrimitive(GA_PRIMPOLY);
		if (!new_poly)
			return false;

		new_poly->setSize(0);
		new_poly->appendVertex(start_pt);
		new_poly->appendVertex(end_pt);
		new_poly->close(); // for line, we don't need to close;
	}

	return true;
}

#endif //HINAPE_JET_UTIL_GEOMETRY_H
