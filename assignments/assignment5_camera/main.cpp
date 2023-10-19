#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <hb/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, hb::Camera* camera, hb::CameraControls* controls, float deltaTime);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

int main() {
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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);
	hb::Camera cam;
	hb::CameraControls controls;
	cam.aspectRatio = (float) SCREEN_WIDTH / SCREEN_HEIGHT;
	cam.fov = 46;
	cam.orthoSize = 5;

	cam.nearPlane = 0;
	cam.farPlane = 100;

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	cam.position = ew::Vec3(1, 1, -4);
	float prevTime = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//cam.position = ew::Vec3(0, 0, 0);
		//cam.target = ew::Vec3(1, 1, 0);

		//Set uniforms
		shader.use();

		shader.setMat4("_View", cam.ViewMatrix());
		shader.setMat4("_Projection", cam.ProjectionMatrix());

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Calculate deltaTime
		float time = (float)glfwGetTime(); //Timestamp of current frame
		float deltaTime = time - prevTime;
		prevTime = time;


		moveCamera(window, &cam, &controls, deltaTime);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");

			ImGui::DragFloat3("Position", &cam.position.x, 0.05f);
			ImGui::DragFloat3("Target", &cam.target.x, 0.05f);
			ImGui::DragFloat("fov", &cam.fov, 0.05f);
			ImGui::DragFloat("orthoSize", &cam.orthoSize, 0.05f);
			ImGui::DragFloat("nearPlane", &cam.nearPlane, 0.05f);
			ImGui::DragFloat("farPlane", &cam.farPlane, 0.05f);
			ImGui::Checkbox("Orthographic", &cam.orthographic);

			if (ImGui::Button("Reset Camera"))
			{
				cam.position = ew::Vec3(0, 0, 0);
				cam.target = cubeTransforms[0].position;

			}

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
}

void moveCamera(GLFWwindow* window, hb::Camera* camera, hb::CameraControls* controls, float deltaTime)
{
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	//TODO: Get mouse position delta for this frame
	ew::Vec2 mouseDelta = ew::Vec2(mouseX - controls->prevMouseX, mouseY - controls->prevMouseY);
	//TODO: Add to yaw and pitch
	controls->yaw += mouseDelta.x * controls->mouseSensitivity;
	controls->pitch += mouseDelta.y * controls->mouseSensitivity;
	//TODO: Clamp pitch between -89 and 89 degrees
	//for some reason std::clamp didn't exist so I found this work arround online
	controls->pitch = std::min((float)89, std::max(controls->pitch, (float)-89));

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	//Construct forward vector using yaw and pitch. Don't forget to convert to radians!
	ew::Vec3 forward = ew::Vec3(cos(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)), sin(ew::Radians(controls->pitch)), sin(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)));
	//By setting target to a point in front of the camera along its forward direction, our LookAt will be updated accordingly when rendering.
	camera->target = camera->position + forward;


	ew::Vec3 up = ew::Vec3(0, 1, 0);
	//TODO: Using camera forward and world up (0,1,0), construct camera right and up vectors. Graham-schmidt process!
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, up));
	up = ew::Normalize(ew::Cross(right, forward));
	//TODO: Keyboard controls for moving along forward, back, right, left, up, and down. See Requirements for key mappings.
	//EXAMPLE: Moving along forward axis if W is held.
	//Note that this is framerate dependent, and will be very fast until you scale by deltaTime. See the next section.
	if (glfwGetKey(window,GLFW_KEY_W))
	{
		camera->position+=forward * controls->moveSpeed * deltaTime;
	}
	
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		camera->position -= right * controls->moveSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_S))
	{
		camera->position -= forward * controls->moveSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_D))
	{
		camera->position += right * controls->moveSpeed * deltaTime;
	}
}