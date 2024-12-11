#include"scene.h"


//construct MVP matrix
Scene::Scene()
{
	lookfrom << 1, 1, 3;
	lookat << 0, 0, 0;
	vup << 0, 1, 0;
	width = 800;
	height = 800;

	enviroment_light << 1, 1, 1;
	//调整后环境光方向
	M_ModelView = ModelView(lookfrom,lookat);
	M_PJT = Projection(lookfrom, lookat);
	M_Viewport = Viewport();
	MVP = M_PJT * M_ModelView;
	MVP_Viewport = M_Viewport * M_PJT * M_ModelView;
	world_to_shadow = M_Viewport*ModelView(enviroment_light, lookat);
	pixel_to_shadow = world_to_shadow *MVP_Viewport.inverse();
	M_test = ModelView(enviroment_light, lookat);
	//light__dir
	Eigen::Vector4f temp;
	temp << enviroment_light, 1;
	temp = MVP * temp;
	//(x,y,z,k)->(x,y,z,1)
	for (int i = 0; i < 3; i++)
		temp[i] /= temp[3];
	light_dir = temp.head<3>();

	zbuffer = new float[width * height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
	shadowbuffer = new float[width * height];
	for (int i = width * height; i--; shadowbuffer[i] = -std::numeric_limits<float>::max());

}
Eigen::Matrix4f Scene::ModelView(Eigen::Vector3f Lfrom, Eigen::Vector3f Lat)
{
	//Mrotate
	Eigen::Vector3f Z, X, Y;
	Eigen::Vector4f temp_z, temp_x, temp_y;
	Z << (Lfrom - Lat).normalized();
	X << vup.cross(Z).normalized();
	Y << Z.cross(X).normalized();
	temp_z << Z, 0.f;
	temp_x << X, 0.f;
	temp_y << Y, 0.f;
	Eigen::Matrix4f Mr,Mt;
	Mr << temp_x, temp_y, temp_z, Eigen::Vector4f(0, 0, 0, 1);
	Mr = Mr.inverse().eval();
	Mt << 1, 0, 0, -Lat[0], 0, 1, 0, -Lat[1], 0, 0, 1, -Lat[2], 0, 0, 0, 1;
	return Mr * Mt;
}
Eigen::Matrix4f Scene::Projection(Eigen::Vector3f Lfrom, Eigen::Vector3f Lat)
{
	float c = -1/(Lfrom - Lat).norm();
	Eigen::Matrix4f Mp;
	Mp << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, c, 1;
	return Mp;
}


//作者实现的viewport是有一个x，y偏移量的，这里保持和作者一致,不过x，y定义在了Scene类中
Eigen::Matrix4f Scene::Viewport()
{
	Eigen::Matrix4f Mviewport;
	Mviewport << 0.5 * width, 0, 0, 0.5 * width,
		0, 0.5 * height, 0, 0.5 * height,
		0, 0, 0.5 * depth, 0.5*depth,
		0, 0, 0, 1;
	return Mviewport;
}

//具体的坐标变换 差分 都在triangle里进行，shading只负责调用triangle函数
void Scene::shading(const model&m, const Shader&shader, TGAImage& image)
{
	set_shadowbuffer(m);
	for (int i = 0; i < m.nfaces(); i++)
	{
		vector<Eigen::Vector3i>face = m.get_face(i);
		triangle(m,face, shader,image);
	}
}
//当叉乘结果长度为0时表示三点共线
Eigen::Vector3f Scene::barycentric(const vector<Eigen::Vector3f>&verts, Eigen::Vector2f P)
{
	Eigen::Vector2f line01(verts[1].head<2>() - verts[0].head<2>());
	Eigen::Vector2f line02(verts[2].head<2>() - verts[0].head<2>());
	Eigen::Vector2f lineP0(verts[0].head<2>() - P);
	Eigen::Vector3f vectorX(line01[0], line02[0], lineP0[0]);
	Eigen::Vector3f vectorY(line01[1], line02[1], lineP0[1]);
	Eigen::Vector3f cross(vectorX.cross(vectorY));
	if (cross.norm() < 0.001)
		return Eigen::Vector3f{-1,1,1};
	return Eigen::Vector3f(1 - cross[0] / cross[2] - cross[1] / cross[2], cross[0] / cross[2], cross[1] / cross[2]);
}

//初始化shadowbuffer，遍历一遍faces
void Scene::set_shadowbuffer(const model& m)
{
	for (int i = 0; i < m.nfaces(); i++)
	{
		vector<Eigen::Vector3i>face = m.get_face(i);
		vector<Eigen::Vector3f> pixel_coordinate;
		int boundingbox_min[2] = { width - 1,height - 1 }, boundingbox_max[2] = { 0,0 };
		for (int j = 0; j < 3; j++)
		{
			Eigen::Vector4f temp;
			temp << m.get_vertex(face[j][0]), 1;
			temp = world_to_shadow * temp;
			temp /= temp[3];
			pixel_coordinate.push_back(temp.head<3>());
			boundingbox_min[0] = std::min(boundingbox_min[0], (int)temp[0]);
			boundingbox_min[1] = std::min(boundingbox_min[1], (int)temp[1]);
			boundingbox_max[0] = std::max(boundingbox_max[0], (int)temp[0]);
			boundingbox_max[1] = std::max(boundingbox_max[1], (int)temp[1]);
		}
		for (int j = std::max(boundingbox_min[0], 0); j <= std::min(boundingbox_max[0], width - 1); j++)
		{
			for (int k = std::max(boundingbox_min[1], 0); k <= std::min(boundingbox_max[1], height - 1); k++)
			{
				Eigen::Vector2f P(j, k);
				Eigen::Vector3f barycentric_uv = barycentric(pixel_coordinate, P);
				if (barycentric_uv[0] < 0 || barycentric_uv[1] < 0 || barycentric_uv[2] < 0)
					continue;
				float Z = barycentric_uv.dot(Eigen::Vector3f(pixel_coordinate[0][2], pixel_coordinate[1][2], pixel_coordinate[2][2]));
				if (shadowbuffer[k * width + j] < Z)
					shadowbuffer[k * width + j] = Z;
			}
		}
	}
}

//因为不同的shader会用到顶点不同的信息，所以将face的信息传参到triangle
void Scene::triangle(const model& m,const vector<Eigen::Vector3i>&face, const Shader&shader, TGAImage& image)
{
	vector<Eigen::Vector3f> pixel_coordinate;
	int boundingbox_min[2] = { width - 1,height - 1 }, boundingbox_max[2] = {0,0};
	//坐标变换
	for (int j = 0; j < 3; j++)
	{
		Eigen::Vector4f temp;
		temp << m.get_vertex(face[j][0]), 1;

		temp = MVP_Viewport * temp;
		temp /= temp[3];
		pixel_coordinate.push_back(temp.head<3>());
		boundingbox_min[0] = std::min(boundingbox_min[0], (int)temp[0]);
		boundingbox_min[1] = std::min(boundingbox_min[1], (int)temp[1]);
		boundingbox_max[0] = std::max(boundingbox_max[0], (int)temp[0]);
		boundingbox_max[1] = std::max(boundingbox_max[1], (int)temp[1]);
	}
	//注意排除位于viewport之外的部分
	for (int i = std::max(boundingbox_min[0], 0); i <= std::min(boundingbox_max[0], width - 1); i++)
	{
		for (int j = std::max(boundingbox_min[1], 0); j <= std::min(boundingbox_max[1], height - 1); j++)
		{
			Eigen::Vector2f P(i + 0.5, j + 0.5);
			Eigen::Vector3f barycentric_uv = barycentric(pixel_coordinate, P);
			if (barycentric_uv[0] < 0 || barycentric_uv[1] < 0 || barycentric_uv[2] < 0)
				continue;
			//坐标变换后可能存在点超出屏幕，不做处理的话zbuffer会溢出
			if (i >= width || j >= height)
				continue;
			float Z = barycentric_uv.dot(Eigen::Vector3f(pixel_coordinate[0][2], pixel_coordinate[1][2], pixel_coordinate[2][2]));
			if (zbuffer[j * width + i] < Z)
			{
				Eigen::Vector4f coor(i , j , Z, 1);
				Eigen::Vector4f temp = pixel_to_shadow * coor;
				temp /= temp[3];
				//坐标变换后可能出现超出屏幕的情况，不做处理的话shadowbuffer会溢出
				if (temp[0] >= width || temp[1] >= height)
					continue;
				float shadow = .3f + 0.7*(shadowbuffer[(int)temp[0] + (int)temp[1] * width] < temp[2]+43.34);
				TGAColor color;
				shader.fragment_shading(pixel_coordinate, barycentric_uv, m, face, color, light_dir, MVP,shadow);
				zbuffer[j * width + i] = Z;
				image.set(i, j, color);
			}
		}
	}
}

