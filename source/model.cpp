#include"model.h"


// v  vt  vn f: vertex/uv/normal
//预处理obj文件
model::model(const string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Failed to open the file\n";
		return;
	}
	flag = true;
	string line;
	while (std::getline(file, line))
	{
		std::istringstream linestream(line);
		string head;
		char trash;
		if (!line.compare(0, 2, "v "))
		{
			linestream >> head;
			Eigen::Vector3f vertex;
			for (int i = 0; i < 3; i++)
				linestream >> vertex[i];
			vertexs.push_back(vertex);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			linestream >> head;
			Eigen::Vector2f uv;
			for (int i = 0; i < 2; i++)
				linestream >> uv[i];
			uvs.push_back(uv);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			linestream >> head;
			Eigen::Vector3f normal;
			for (int i = 0; i < 3; i++)
				linestream >> normal[i];
			normals.push_back(normal);
		}
		else if (!line.compare(0, 2, "f "))
		{
			linestream >> head;
			vector<Eigen::Vector3i> face;
			for (int i = 0; i < 3; i++)
			{
				Eigen::Vector3i vertex;
				linestream >> vertex[0] >> trash >> vertex[1] >> trash >> vertex[2];
				vertex[0]--;
				vertex[1]--;
				vertex[2]--;
				face.push_back(vertex);
			}
			faces.push_back(face);
		}
		else{}
	}
	std::cerr << "# v# " << vertexs.size() << " f# " << faces.size() << " vt# " << uvs.size() << " vn# " << normals.size() << std::endl;
	load_texture(filename, "_diffuse.tga", diffusemap_);
	load_texture(filename, "_nm.tga", normalmap_);
}

Eigen::Vector3f model::get_vertex(int idx)const
{
	return vertexs[idx];
}

vector<Eigen::Vector3i> model::get_face(int idx)const
{
	return faces[idx];
}

Eigen::Vector3f model::get_normal(int idx)const
{
	return normals[idx];
}

Eigen::Vector2f model::get_uv(int idx)const
{
	return Eigen::Vector2f(uvs[idx][0] * diffusemap_.get_width(), uvs[idx][1] * diffusemap_.get_height());
}

TGAColor model::get_texture(Eigen::Vector2i uv)const
{
	return diffusemap_.get(uv[0], uv[1]);
}
Eigen::Vector3f model::get_normal(Eigen::Vector2i uv)const
{
	TGAColor color = normalmap_.get(uv[0], uv[1]);
	Eigen::Vector3f normal;
	for (int i = 0; i < 3; i++)
		normal[2-i] = (float)color[i] / 255.f * 2.f - 1.f;
	return normal;
}
int model::nvertexs()const
{
	return vertexs.size();
}
int model::nfaces()const
{
	return faces.size();
}
void model::load_texture(std::string filename, const char* suffix, TGAImage& img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}