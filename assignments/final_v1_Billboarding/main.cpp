#include <stdio.h>
#include <math.h>
#include <cstdlib>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include "billboard.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Vec3 position; //World space
	ew::Vec3 color; //RGB
};

struct Material {
	float ambientK; //Ambient coefficient (0-1)
	float diffuseK; //Diffuse coefficient (0-1)
	float specular; //Specular coefficient (0-1)
	float shininess; //Shininess
};

enum BoardMode
{
	None = 0,
	Rain = 1,
	Fire = 2,
	COUNT = 3
};


int main() {
	const int SPARK_COUNT = 150;
	const int RAIN_COUNT = 75;

	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("Assets/defaultLit.vert", "Assets/defaultLit.frag");
	ew::Shader lightShader("Assets/unlit.vert", "Assets/unlit.frag");
	ew::Shader billShader("Assets/billboard.vert", "Assets/billboard.frag");

	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);


	ew::Transform lightTransfom1;
	ew::Transform lightTransfom2;
	ew::Transform lightTransfom3;
	ew::Transform lightTransfom4;

	Light lights[4];
	lights[0].position = ew::Vec3(2.0f, 1.0f, 2.0f);
	lights[0].color = ew::Vec3(1.0f, 0.0f, 0.0f);
	lights[1].position = ew::Vec3(-2.0f, 1.0f, 2.0f);
	lights[1].color = ew::Vec3(0.0f, 1.0f, 0.0f);
	lights[2].position = ew::Vec3(2.0f, 1.0f, -2.0f);
	lights[2].color = ew::Vec3(0.0f, 0.0f, 1.0f);
	lights[3].position = ew::Vec3(-2.0f, 1.0f, -2.0f);
	lights[3].color = ew::Vec3(1.0f, 1.0f, 1.0f);

	//billboard setup
	int mode = None;
	ew::Mesh CanvasMesh(ew::createPlane(0.344f, 0.23f, 1));
	Billboard Canvas(ew::Vec3(0, 0, 0));

	Canvas.scaleDistance = 0.2;
	Canvas.size = ew::Vec2(0.344f, 0.23f);

	ew::Mesh raindropMesh(ew::createPlane(0.0025f, 0.0055f, 1));
	ew::Mesh sparkMesh(ew::createPlane(0.0025f, 0.0025f, 1));
	Billboard raindrops[RAIN_COUNT];
	Billboard sparks[SPARK_COUNT];

	ew::Mesh lightMesh(ew::createPlane(0.25f, 0.25f, 1));
	Billboard LightBil[4];
	for (int k = 0; k < 4; k++)
	{
		LightBil[k].size = ew::Vec2(0.25, 0.5);
		LightBil[k].transform.position = lights[k].position;
	}
	float PartSpeed = 0.4;
	/*Billboard raindropTest1(ew::Vec3(0, 0, 0));
	Billboard raindropTest2(ew::Vec3(0, 0, 0));

	raindropTest1.size = ew::Vec2(0.005f, 0.005f);
	raindropTest1.canvas = &Canvas;
	
	raindropTest2.size = ew::Vec2(0.005f, 0.005f);
	raindropTest2.canvas = &Canvas;*/

	for (int i = 0; i < RAIN_COUNT; i++)
	{
		raindrops[i].size = ew::Vec2(0.0025f, 0.0055f);
		raindrops[i].canvas = &Canvas;
		float rx = rand();
		float ry = rand();
		float randomOffsetX= (fmod(rx, 200)/100) - 1;
		float randomOffsetY = (fmod(ry, 220) / 100) - 0.8;
		raindrops[i].offSave = ew::Vec2(randomOffsetX, randomOffsetY);
	}
	for (int j = 0; j < SPARK_COUNT; j++)
	{
		sparks[j].size = ew::Vec2(0.0025f, 0.0025f);
		sparks[j].canvas = &Canvas;
		float rx = rand();
		float ry = rand();
		float rs = rand();
		float randomOffsetX = (fmod(rx, 200) / 100) - 1;
		float randomOffsetY = (fmod(ry, 220) / 100) - 0.8;
		float Expire = (fmod(rs, 100) / 100) * 0.8;
		sparks[j].offSave = ew::Vec2(randomOffsetX, randomOffsetY);
		sparks[j].sparkExpire = Expire;
	}

	//set the uniforms of the second shader program

	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {

		for (int k = 0; k < 4; k++)
		{
			//lights update using basic update
			LightBil[k].update(camera.position);
		}

		if (mode == Rain)
		{
			for (int i = 0; i < RAIN_COUNT; i++)
			{
				//rain falls and when off screan reset to random point on screen, like rain hitting a window
				float dropOffsetY = ((raindrops[i].offSave.y * 100) - PartSpeed) / 100;
				float setX = raindrops[i].offSave.x;
				if (abs(dropOffsetY) > 1.3)
				{
					float ry = rand();
					float rx = rand();
					dropOffsetY = (fmod(ry, 220) / 100) - 0.8;
					setX = (fmod(rx, 200) / 100) - 1;
				}
				raindrops[i].offSave = ew::Vec2(setX, dropOffsetY);
			}
		}
		else if (mode == Fire)
		{
			//sparks raise up screan reseting to random x and bottom of screan and starts again
			for (int j = 0; j < SPARK_COUNT; j++)
			{
				float setX = sparks[j].offSave.x;
				float raiseY = ((sparks[j].offSave.y * 100) + PartSpeed) / 100;
				float Expire = sparks[j].sparkExpire;
				if ((raiseY) > sparks[j].sparkExpire)
				{
					float rs = rand();
					Expire = (fmod(rs, 100) / 100);

					float rx = rand();
					setX = (fmod(rx, 200) / 100) - 1;
					raiseY = -1.1;
				}
				sparks[j].offSave = ew::Vec2(setX, raiseY);
				sparks[j].sparkExpire = Expire;
			}
		}
		glfwPollEvents();

		lightTransfom1.position = lights[0].position;
		lightTransfom2.position = lights[1].position;
		lightTransfom3.position = lights[2].position;
		lightTransfom4.position = lights[3].position;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//basic set up, unedited
		shader.use();
		shader.setVec3("_CamPos", camera.position);
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		shader.setVec3("_Lights[0].position", lights[0].position);
		shader.setVec3("_Lights[0].color", lights[0].color);

		shader.setVec3("_Lights[1].position", lights[1].position);
		shader.setVec3("_Lights[1].color", lights[1].color);

		shader.setVec3("_Lights[2].position", lights[2].position);
		shader.setVec3("_Lights[2].color", lights[2].color);

		shader.setVec3("_Lights[3].position", lights[3].position);
		shader.setVec3("_Lights[3].color", lights[3].color);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();
	
		//billboard stuff!
		//add screan position thing to make it so the watter drop can fall, right now it is defaulting to center
		Canvas.update(camera.position, camera.target);
		
		billShader.use();
		//rain effect
		billShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		if (mode == Rain)
		{
			billShader.setVec3("_Color", ew::Vec3(0.25, 0.35, 1));
			billShader.setInt("_Circle", 1);
			

			for (int i = 0; i < RAIN_COUNT; i++)
			{
				raindrops[i].update(camera.position, camera.target, raindrops[i].offSave);

				billShader.setMat4("_Model", raindrops[i].transform.getModelMatrix());
				raindropMesh.draw();
			}
		}
		else if (mode == Fire)
			//sparks draw
			{
				billShader.setInt("_Circle", 1);

				for (int i = 0; i < SPARK_COUNT; i++)
				{
					if (sparks[i].offSave.y < 0)
					{
						billShader.setVec3("_Color", ew::Vec3(1, ew::Clamp(abs(sparks[i].offSave.y), 0, 1), 0));
					}
					else
					{
						billShader.setVec3("_Color",1, 0, 0);
					}

					sparks[i].update(camera.position, camera.target, sparks[i].offSave);

					billShader.setMat4("_Model", sparks[i].transform.getModelMatrix());
					sparkMesh.draw();
				}
			}

		//draw lights billboard addition
		for (int l = 0; l < 4; l++)
		{
			billShader.setVec3("_Color", lights[l].color);
			billShader.setInt("_Circle", 1);
			billShader.setMat4("_Model", LightBil[l].transform.getModelMatrix());
			lightMesh.draw();
		}


		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}
			if (ImGui::CollapsingHeader("LIGHTS")) 
			{
				if (ImGui::CollapsingHeader("light1"))
				{
					ImGui::DragFloat3("Position", &lights[0].position.x, 0.1f);
					ImGui::DragFloat3("Color", &lights[0].color.x, 0.1f);
				}
				if (ImGui::CollapsingHeader("light2"))
				{
					ImGui::DragFloat3("Position", &lights[1].position.x, 0.1f);
					ImGui::DragFloat3("Color", &lights[1].color.x, 0.1f);
				}
				if (ImGui::CollapsingHeader("light3"))
				{
					ImGui::DragFloat3("Position", &lights[2].position.x, 0.1f);
					ImGui::DragFloat3("Color", &lights[2].color.x, 0.1f);
				}
				if (ImGui::CollapsingHeader("light4"))
				{
					ImGui::DragFloat3("Position", &lights[3].position.x, 0.1f);
					ImGui::DragFloat3("Color", &lights[3].color.x, 0.1f);
				}
			}

			ImGui::DragFloat("Particle Speed", &PartSpeed, 0.01f);
			ImGui::SliderInt("Mode", &mode, None, COUNT - 1);
			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}