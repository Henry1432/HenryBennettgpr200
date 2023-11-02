#include "procGen.h"

namespace hb
{
	ew::MeshData createSphere(float radius, int numSegments)
	{
		ew::MeshData data;

		float thetaStep = 2 * (ew::PI) / numSegments;
		float phiStep = ew::PI / numSegments;
		int poleStart;
		int columns = numSegments + 1;
		for (int row = 0; row <= numSegments; row++)
		{
			ew::Vertex v;

			float phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++) //Duplicate column for each row
			{
				float theta = col * thetaStep;
				v.pos.x = radius * cos(theta) * sin(phi);
				v.pos.y = radius * cos(phi);
				v.pos.z = radius * sin(theta) * sin(phi);

				v.normal = ew::Normalize(v.pos);
				float sub = 1 / numSegments;
				v.uv = ew::Vec2(col * sub, row * sub);

				data.vertices.push_back(v);

				if (row == 0 && col == 0)
				{
					poleStart = data.vertices.size() - 1;
				}

				int start = row * columns + col;
				//Triangle 1
				data.indices.push_back(start);
				data.indices.push_back(start + 1);
				data.indices.push_back(start + columns);
				//Triangle 2
				data.indices.push_back(start + 1);
				data.indices.push_back(start + columns + 1);
				data.indices.push_back(start + columns);
			}
		}

		int sideStart = numSegments + 1;
		for (int i = 0; i < numSegments; i++)
		{
			data.indices.push_back(sideStart + i);
			data.indices.push_back(poleStart + i); //Pole
			data.indices.push_back(sideStart + i + 1);
		}

		return data;
	}
	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		ew::MeshData data;

		float topY = height / 2; //y=0 is centered
		float bottomY = -topY;

		//Top center
		ew::Vertex top;
		top.pos = ew::Vec3(0, topY, 0);
		top.normal = ew::Vec3(0,1,0);
		data.vertices.push_back(top);

		int topStart;
		int topCenter = data.vertices.size() - 1;
		//Top ring
		float thetaStep = 2 * (ew::PI) / numSegments;

		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex v;
			v.pos.y = topY; //or bottomY
			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;

			v.normal = top.normal;
			v.uv = ew::Vec2(v.pos.x / radius, v.pos.y / radius);

			data.vertices.push_back(v);
		}

		//Top side ring
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex v;
			v.pos.y = topY; //or bottomY
			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;

			v.normal = ew::Normalize(top.pos - v.pos);
			v.uv = ew::Vec2((theta/2*ew::PI*radius), 1);

			data.vertices.push_back(v);

			if (i == 0)
			{
				topStart = data.vertices.size() - 1;
			}
		}


		for (int i = 0; i <= numSegments; i++)
		{
			data.indices.push_back(topStart + i);
			data.indices.push_back(topCenter);
			data.indices.push_back(topStart + i + 1);
		}
		//Bottom center
		ew::Vertex bottom;
		bottom.pos = ew::Vec3(0, bottomY, 0);
		bottom.normal = ew::Vec3(0, -1, 0);
		int bottomStart;
		//Bottom side ring
		for (int j = 0; j <= numSegments; j++)
		{
			float theta = j * thetaStep;
			ew::Vertex v;
			v.pos.y = bottomY; //or bottomY
			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;

			v.normal = ew::Normalize(bottom.pos - v.pos);
			v.uv = ew::Vec2((theta / 2 * ew::PI * radius), 0);

			data.vertices.push_back(v);


			if (j == 0)
			{
				bottomStart = data.vertices.size() - 1;
			}
		}
		//Bottom center
		data.vertices.push_back(bottom);
		int bottomCenter = data.vertices.size() - 1;
		for (int i = 0; i <= numSegments; i++)
		{
			data.indices.push_back(bottomStart + i);
			data.indices.push_back(bottomCenter);
			data.indices.push_back(bottomStart + i + 1);
		}

		//Bottom ring
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = i * thetaStep;
			ew::Vertex v;
			v.pos.y = bottomY; //or bottomY
			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;

			v.normal = bottom.normal;
			v.uv = ew::Vec2(v.pos.x / radius, v.pos.y / radius);

			data.vertices.push_back(v);
		}

		//Assumes bottom ring vertices have been added immediately after top ring
		int columns = numSegments + 1;
		for (int i = 0; i < columns; i++)
		{
			int start = topStart + i;
			//Triangle 1
			data.indices.push_back(start);
			data.indices.push_back(start + 1);
			data.indices.push_back(start + columns);
			//Triangle 2
			data.indices.push_back(start + 1);
			data.indices.push_back(start + columns + 1);
			data.indices.push_back(start + columns);
		}

		return data;
	}
	ew::MeshData createPlane(float width, float height, int subdivisions)
	{
		ew::MeshData data;
		int columns = subdivisions + 1;

		for (int row = 0; row <= subdivisions; row++)
		{
			for (int col = 0; col <= subdivisions; col++)
			{
				ew::Vertex v;
				v.pos.x = width * (col / subdivisions);
				v.pos.z = -height * (row / subdivisions);

				float sub = 1 / subdivisions;
				v.uv = ew::Vec2(col * sub, row * sub);
				v.normal = ew::Vec3(0, 1, 0);

				data.vertices.push_back(v);

				int start = row * columns + col;
				//Bottom right triangle
				data.indices.push_back(start);
				data.indices.push_back(start + 1);
				data.indices.push_back(start + columns + 1);
				//Top left triangle
				data.indices.push_back(start);
				data.indices.push_back(start + columns);
				data.indices.push_back(start + columns + 1);
			}
		}

		return data;
	}
}
