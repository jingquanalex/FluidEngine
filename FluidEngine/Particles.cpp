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
	maxCount = 20000;
	this->camera = camera;
	this->dt = dt;
	shader = new Shader("particle");
	solver = new WCSPH(dt, &particles, camera);
	//solver = new PCISPH(dt, &particles, camera);

	// Skybox
	cubemaps = new Quad();
	cubemaps->load("lakeblur", "skyquad");
	cubemaps->loadCubemap("lake");
	cubemaps->loadCubemap("lake2");
	cubemaps->loadCubemap("church");

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
	glUniform1f(glGetUniformLocation(shader->getProgram(), "fRadius"), solver->getRadius());
	glUseProgram(0);

	// Setup render targets and their textures
	//mapSize = ivec2(1024);
	mapSize = ivec2(window_width, window_height);
	screenQuad = new Quad();
	screenQuad->load();
	shaderBFilter = new Shader("particleBlur");
	shaderNormal = new Shader("particleCompose");
	shaderGaussian = new Shader("gaussianBlur");

	glGenTextures(1, &sceneMap);
	glBindTexture(GL_TEXTURE_2D, sceneMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x, mapSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &sceneFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Depth and color, mrt
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &depthFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	glGenTextures(1, &colorMap);
	glBindTexture(GL_TEXTURE_2D, colorMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x, mapSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &blurFbo);

	glGenTextures(1, &blurMapV);
	glBindTexture(GL_TEXTURE_2D, blurMapV);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &blurMapH);
	glBindTexture(GL_TEXTURE_2D, blurMapH);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &thickMap);
	glBindTexture(GL_TEXTURE_2D, thickMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x / 4, mapSize.y / 4, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &thickFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, thickFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thickMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &gaussFbo);

	glGenTextures(1, &gaussMapV);
	glBindTexture(GL_TEXTURE_2D, gaussMapV);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x / 4, mapSize.y / 4, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &gaussMapH);
	glBindTexture(GL_TEXTURE_2D, gaussMapH);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x / 4, mapSize.y / 4, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Update particle positions and send to vbo
void Particles::update()
{
	// Remove stray particles
	auto it = particles.begin();
	while(it != particles.end())
	{
		if (dot(it->Position, it->Position) > pow(10, 2))
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
	solver->compute(mouseDelta, renderMode);

	// Increment/decrement sph attributes
	inctime = (float)(glutGet(GLUT_ELAPSED_TIME) - heldtime) * 0.01f;
	if (stateIncRestdensity)
	{
		solver->setRestDensity(solver->getRestDensity() + 10.0f * inctime * dt);
	}
	else if (stateDecRestdensity)
	{
		if (solver->getRestDensity() <= 0.0f)
		{
			solver->setRestDensity(0.0f);
		}
		else
		{
			solver->setRestDensity(solver->getRestDensity() - 10.0f * inctime * dt);
		}
	}

	inctime = (float)(glutGet(GLUT_ELAPSED_TIME) - heldtime) * 0.001f;
	if (stateIncGasconstant)
	{
		solver->setGasConstant(solver->getGasConstant() + 0.00001f * inctime * dt);
	}
	else if (stateDecGasconstant)
	{
		if (solver->getGasConstant() <= 0.0f)
		{
			solver->setGasConstant(0.0f);
		}
		else
		{
			solver->setGasConstant(solver->getGasConstant() - 0.00001f * inctime * dt);
		}
	}

	inctime = (float)(glutGet(GLUT_ELAPSED_TIME) - heldtime) * 0.1f;
	if (stateIncViscosity)
	{
		solver->setViscosity(solver->getViscosity() + 0.1f * inctime * dt);
	}
	else if (stateDecViscosity)
	{
		if (solver->getViscosity() <= 0.0f)
		{
			solver->setViscosity(0.0f);
		}
		else
		{
			solver->setViscosity(solver->getViscosity() - 0.1f * inctime * dt);
		}
	}

	inctime = (float)(glutGet(GLUT_ELAPSED_TIME) - heldtime) * 0.1f;
	if (stateIncSurfacetension)
	{
		solver->setSurfaceTension(solver->getSurfaceTension() + 0.1f * inctime * dt);
	}
	else if (stateDecSurfacetension)
	{
		if (solver->getSurfaceTension() <= 0.0f)
		{
			solver->setSurfaceTension(0.0f);
		}
		else
		{
			solver->setSurfaceTension(solver->getSurfaceTension() - 0.1f * inctime * dt);
		}
	}

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
void Particles::drawDepth()
{
	if (renderMode == 0) return;

	// Render background scene
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
	glViewport(0, 0, mapSize.x, mapSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cubemaps->draw();
	
	// Render particles' color and depth
	glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
	glViewport(0, 0, mapSize.x, mapSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader->getProgram());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "colorThickness"), 0.0f);
	glUniform1i(glGetUniformLocation(shader->getProgram(), "renderMode"), renderMode);
	glUniform1f(glGetUniformLocation(shader->getProgram(), "near"), camera->getNearPlane());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "far"), camera->getFarPlane());
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);

	// 2 pass bilateral filter depth map
	glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurMapV, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glViewport(0, 0, mapSize.x, mapSize.y);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderBFilter->getProgram());
	glUniform2fv(glGetUniformLocation(shaderBFilter->getProgram(), "blurDir"), 1, value_ptr(vec2(0, 1.0f / mapSize.x)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(screenQuad->getVao());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurMapH, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glViewport(0, 0, mapSize.x, mapSize.y);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderBFilter->getProgram());
	glUniform2fv(glGetUniformLocation(shaderBFilter->getProgram(), "blurDir"), 1, value_ptr(vec2(1.0f / mapSize.x, 0)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurMapV);
	glBindVertexArray(screenQuad->getVao());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Render addictive blended particles
	glBindFramebuffer(GL_FRAMEBUFFER, thickFbo);
	glViewport(0, 0, mapSize.x / 4, mapSize.y / 4);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(shader->getProgram());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "colorThickness"), 0.05f);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);

	// 2 pass gaussian blur thickness map
	glBindFramebuffer(GL_FRAMEBUFFER, gaussFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gaussMapV, 0);
	glViewport(0, 0, mapSize.x / 4, mapSize.y / 4);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderGaussian->getProgram());
	glUniform2fv(glGetUniformLocation(shaderGaussian->getProgram(), "blurDir"), 1, value_ptr(vec2(0, 1.0f / mapSize.y / 4)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, thickMap);
	glBindVertexArray(screenQuad->getVao());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, gaussFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gaussMapH, 0);
	glViewport(0, 0, mapSize.x / 4, mapSize.y / 4);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderGaussian->getProgram());
	glUniform2fv(glGetUniformLocation(shaderGaussian->getProgram(), "blurDir"), 1, value_ptr(vec2(1.0f / mapSize.x / 4, 0)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gaussMapV);
	glBindVertexArray(screenQuad->getVao());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Particles::draw()
{
	if (renderMode == 0)
	{
		cubemaps->draw();

		glUseProgram(shader->getProgram());
		glUniform1f(glGetUniformLocation(shader->getProgram(), "colorThickness"), 0.0f);
		glUniform1i(glGetUniformLocation(shader->getProgram(), "renderMode"), renderMode);
		glUniform1f(glGetUniformLocation(shader->getProgram(), "near"), camera->getNearPlane());
		glUniform1f(glGetUniformLocation(shader->getProgram(), "far"), camera->getFarPlane());
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, count);
		glBindVertexArray(0);
		glUseProgram(0);

		return;
	}

	// Render to normal FBO
	glUseProgram(shaderNormal->getProgram());
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "renderMode"), renderMode);
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "sceneMap"), 0);
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "colorMap"), 1);
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "depthMap"), 2);
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "envMap"), 3);
	glUniform1i(glGetUniformLocation(shaderNormal->getProgram(), "thickMap"), 4);
	glUniform2fv(glGetUniformLocation(shaderNormal->getProgram(), "texSize"), 1, value_ptr(vec2(mapSize)));
	glUniformMatrix4fv(glGetUniformLocation(shaderNormal->getProgram(), "invProj"),
		1, GL_FALSE, value_ptr(camera->getMatInvProjection()));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneMap); 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blurMapH);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps->getCubeMap());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gaussMapH);
	glBindVertexArray(screenQuad->getVao());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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
				particle.Position = vec3(i, j, k) * solver->getRadius() * 2 - 
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
	particle.Position = vec3(0, -4.0, 0);
	particle.Color = vec4(1.0);
	particles.push_back(particle);

	count += 1;
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
		Particle::ID = 0;
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
	if (key >= '0' && key <= '9')
	{
		renderMode = key - '0';
	}
	else
	{
		switch (key)
		{
		case 'b':
			addParticles();
			break;
		case 'v':
			addParticles(21);
			break;
		case ' ':
			addParticles(500, 2.5f);
			break;
		case 'c':
			removeParticles(count);
			break;
		case 'x':
			solver->resetParticleAttributes();
			break;
		case 'g':
			solver->toggleGravity();
			break;
		case']':
			cubemaps->cycleNextMap();
			break;
		case'[':
			cubemaps->cyclePreviousMap();
			break;
		}

		if (!stateHeldtime)
		{
			heldtime = glutGet(GLUT_ELAPSED_TIME);
			stateHeldtime = true;
		}

		switch (key)
		{
		case 'q':
			stateDecRestdensity = true;
			break;
		case 'w':
			stateIncRestdensity = true;
			break;
		case 'e':
			stateDecGasconstant = true;
			break;
		case 'r':
			stateIncGasconstant = true;
			break;
		case 't':
			stateDecViscosity = true;
			break;
		case 'y':
			stateIncViscosity = true;
			break;
		case 'u':
			stateDecSurfacetension = true;
			break;
		case 'i':
			stateIncSurfacetension = true;
			break;
		}
	}
}

void Particles::keyboardUp(unsigned char key)
{
	stateHeldtime = false;

	switch (key)
	{
	case 'q':
		stateDecRestdensity = false;
		break;
	case 'w':
		stateIncRestdensity = false;
		break;
	case 'e':
		stateDecGasconstant = false;
		break;
	case 'r':
		stateIncGasconstant = false;
		break;
	case 't':
		stateDecViscosity = false;
		break;
	case 'y':
		stateIncViscosity = false;
		break;
	case 'u':
		stateDecSurfacetension = false;
		break;
	case 'i':
		stateIncSurfacetension = false;
		break;
	}
}

void Particles::keyboardSpecial(int key)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		solver->setGravity(vec3(0, 9.8, 0));
		break;
	case GLUT_KEY_DOWN:
		solver->setGravity(vec3(0, -9.8, 0));
		break;
	case GLUT_KEY_LEFT:
		solver->setGravity(vec3(-9.8, 0, 0));
		break;
	case GLUT_KEY_RIGHT:
		solver->setGravity(vec3(9.8, 0, 0));
		break;
	}
}

WCSPH* Particles::getSolver()
{
	return solver;
}