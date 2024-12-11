#ifndef MODEL_H
#define MODEL_H
#include<Eigen/Core>
#include <Eigen/Geometry>
#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include"tgaimage.h"
using std::vector;
using std::string;
class model
{
public:
	~model(){}
	model(const string&);
	Eigen::Vector3f get_vertex(int idx) const;
	vector<Eigen::Vector3i> get_face(int idx) const;
	//��model��ȡĳ��ķ�����
	Eigen::Vector3f get_normal(int idx) const;
	//��texture map��ĳ�㷨����
	Eigen::Vector3f get_normal(Eigen::Vector2i)const;
	Eigen::Vector2f get_uv(int idx) const;
	TGAColor get_texture(Eigen::Vector2i)const;
	int nvertexs() const;
	int nfaces() const;
	//����ȡ�ļ�ʧ��ʱ���ٽ�����Ⱦ
	bool flag = false;


private:
	vector<Eigen::Vector3f> vertexs;
	vector<vector<Eigen::Vector3i>> faces;
	vector<Eigen::Vector3f> normals;
	vector<Eigen::Vector2f> uvs;
	//texture map   normal map
	TGAImage diffusemap_;
	TGAImage normalmap_;
	//��ȡ����img
	void load_texture(std::string filename, const char* suffix, TGAImage& img);

};

#endif // !MODEL_H

