#include "MLoader.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

MLoader::MLoader()
{

}

MLoader::~MLoader()
{

}

void MLoader::load()
{
	ifstream file(g_mediaDirectory + "molecules/3E76(54464).txt");

	std::vector<sParticle> listParticles;
	while (file)
	{
		string s;
		if (getline(file, s))
		{
			istringstream iss(s);
			std::vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

			sParticle particle;
			particle.Position = vec3(stof(items.at(0)), stof(items.at(1)), stof(items.at(2)));
			particle.Position = particle.Position / 100;
			particle.Radius = stof(items.at(3)) / 1000 + 0.1f;
			particle.Color = vec4(stof(items.at(4)), stof(items.at(5)), stof(items.at(6)), 1.0);
			listParticles.push_back(particle);
		}
	}
	listSParticles.push_back(listParticles);

	count = (int)listParticles.size();

	// Setup vbo for Molecules
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(sParticle), listParticles.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)offsetof(sParticle, sParticle::Color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)offsetof(sParticle, sParticle::Radius));

	glBindVertexArray(0);
}

void MLoader::draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
}

vector<sParticle>* MLoader::getParticleData(int index)
{
	return &listSParticles.at(index);
}