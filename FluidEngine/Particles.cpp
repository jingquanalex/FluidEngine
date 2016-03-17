#include "Particles.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

Particles::Particles()
{
	count = 0;
	maxCount = 0;
}

Particles::~Particles()
{

}

void Particles::load(float dt, Camera* camera)
{
	// Set max particles count for vbo and load particle shaders
	maxCount = 10000;
	this->camera = camera;
	shader = new Shader("particle");
	solver = new WCSPH(dt, &particles, camera);
	//solver = new PCISPH(dt, &particles, camera);

	// Load a box model to contain the particles
	box = new Object();
	box->setScale(vec3(10));
	box->setWireframeMode(true);
	box->load("cube.obj");

	// Add some particles temp
	//addParticles(10, 0.5);

	// Initialize vertex buffer memory and vao
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, maxCount * sizeof(sParticle), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)offsetof(sParticle, sParticle::Color));

	glBindVertexArray(0);

	glUseProgram(shader->getProgram());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "radius"), solver->getRadius());
	glUseProgram(0);
}

// Update particle positions and send to vbo
void Particles::update()
{
	// Remove stray particles
	auto it = particles.begin();
	while(it != particles.end())
	{
		if (length(it->Position) > 10)
		{
			it = particles.erase(remove(particles.begin(), particles.end(), *it), particles.end());
			count--;
		}
		else
		{
			it++;
		}
	}

	// Click and drag particle force
	if (statePicked)
	{
		//cout << mouseDelta.x << " " << mouseDelta.y << endl;
		mouseDelta = ivec2(mouseX - mouseLastX, mouseY - mouseLastY);
	}
	else
	{
		mouseDelta = ivec2(0);
	}
	mouseLastX = mouseX;
	mouseLastY = mouseY;

	// Solve SPH
	solver->compute(mouseDelta);

	// Create particle data list to send to gpu
	sParticles.clear();
	for (Particle &p : particles)
	{
		sParticle sp;
		sp.Position = p.Position;
		sp.Color = p.Color;
		sParticles.push_back(sp);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(sParticle), sParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Render particles
void Particles::draw()
{
	glUseProgram(shader->getProgram());
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);

	box->draw();
}

float Particles::fRandom(float low, float high)
{
	return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (high - low)));
}

// Add a specified number of Particles with a random position
void Particles::addParticles(int value, float range)
{
	for (int i = 0; i < value; i++)
	{
		Particle particle;
		particle.Position.x = fRandom(-range, range);
		particle.Position.y = fRandom(-range, range);
		particle.Position.z = fRandom(-range, range);
		particle.Color = vec4(1.0);
		particles.push_back(particle);
	}
	count += value;
}

void Particles::addParticles(int numPerSide)
{
	for (float i = 0; i <= numPerSide; i++)
	{
		for (float j = 0; j <= numPerSide; j++)
		{
			for (float k = 0; k <= numPerSide; k++)
			{
				Particle particle;
				particle.Position = vec3(i, j, k) * solver->getRadius() - 
					vec3(numPerSide * solver->getRadius());
				particle.Color = vec4(1.0);
				particles.push_back(particle);

				count++;
			}
		}
	}
}

void Particles::addParticles()
{
	Particle particle;
	particle.Position = vec3(0, -5, 0);
	particle.Color = vec4(1.0);
	particles.push_back(particle);
	Particle particle2;
	particle2.Position = vec3(0, -4.5, 0);
	particle2.Color = vec4(1.0);
	particles.push_back(particle2);

	count += 2;
}

// Remove a specified number of Particles
void Particles::removeParticles(int value)
{
	if (value < count)
	{
		particles.resize(count - value);
		count -= value;
	}
	else
	{
		particles.clear();
		count = 0;
	}
}

// Pick a particle for more control
void Particles::mouse(int button, int state)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Make a ray from 2 points on the near and far planes
		vec3 p0 = unProject(vec3(mouseX, window_height - mouseY, -1), // near
			camera->getMatView(), camera->getMatProjection(), 
			vec4(0, 0, window_width, window_height));
		vec3 p1 = unProject(vec3(mouseX, window_height - mouseY, 1), // far
			camera->getMatView(), camera->getMatProjection(),
			vec4(0, 0, window_width, window_height));
		vec3 ray = normalize(p1 - p0);

		cout << ray.x << " " << ray.y << " " << ray.z << endl;

		// === Sphere vs ray test ===

		// Sort particles according to distance from camera TODO
		/*for (Particle &p : particles)
		{
			p.distance = distance2(p.Position, camera->getPosition());
		}
		sort(particles.begin(), particles.end(), [](Particle p1, Particle p2){
			return p1.distance < p2.distance;
		});*/

		solver->setDebugParticle(solver->getParticleAtRay(ray));

		statePicked = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		statePicked = false;
	}
}

// Apply force at the area of particle
void Particles::mouseMove(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void Particles::mouseMovePassive(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void Particles::keyboard(unsigned char key)
{
	switch (key)
	{
	case 'b':
		addParticles();
		break;
	case ' ':
		//addParticles(2, 0.2f);
		addParticles(1);
		break;
	case '1':
		addParticles(100, 2.5f);
		break;
	case 'c':
		removeParticles(count);
		Particle::ID = 0;
		break;
	}
}