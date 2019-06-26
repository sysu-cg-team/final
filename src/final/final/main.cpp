#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include "wood.h"
#include "skybox.h"
#include "plane.h"
#include "grass.h"
#include "resource_manager.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char *argv[])
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "final", nullptr, nullptr);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetKeyCallback(window, key_callback);
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // OpenGL configuration

  glEnable(GL_DEPTH_TEST);

  // laod frog

  // build and compile shaders
  // -------------------------
  Shader ourShader(FileSystem::getPath("src/final/final/model.vs").c_str(), FileSystem::getPath("src/final/final/model.fs").c_str());

  // load models
  // -----------
  Model ourModel(FileSystem::getPath("resources/landscape/landscape.obj"));

  // load wood
  // ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/wood.jpg").c_str(), false, "wood");
  // auto woodShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/wood.vs").c_str(), FileSystem::getPath("src/final/final/wood.fs").c_str(), nullptr, "wood");
  // auto wood = Wood(glm::vec3(0, 20, 0), glm::vec3(2, 0.8, 2), glm::vec3(1));

  // load skybox
  ResourceManager::LoadShader(FileSystem::getPath("src/final/final/skybox.vs").c_str(), FileSystem::getPath("src/final/final/skybox.fs").c_str(), nullptr, "skybox");
  std::vector<std::string>
      faces{
          FileSystem::getPath("resources/textures/sunny/sunny_lf.jpg"),
          FileSystem::getPath("resources/textures/sunny/sunny_rt.jpg"),
          FileSystem::getPath("resources/textures/sunny/sunny_up.jpg"),
          FileSystem::getPath("resources/textures/sunny/sunny_dn.jpg"),
          FileSystem::getPath("resources/textures/sunny/sunny_ft.jpg"),
          FileSystem::getPath("resources/textures/sunny/sunny_bk.jpg")};
  std::vector<std::string>
	  faces2{
		  FileSystem::getPath("resources/textures/night/night_lf.png"),
		  FileSystem::getPath("resources/textures/night/night_rt.png"),
		  FileSystem::getPath("resources/textures/night/night_up.png"),
		  FileSystem::getPath("resources/textures/night/night_dn.png"),
		  FileSystem::getPath("resources/textures/night/night_ft.png"),
		  FileSystem::getPath("resources/textures/night/night_bk.png") };

  Skybox skybox(glm::vec3(1), glm::vec3(1), glm::vec3(1), faces, faces2, "skybox_s", "skybox_n");


  // load plane
  auto planeShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/plane.vs").c_str(), FileSystem::getPath("src/final/final/plane.fs").c_str(), nullptr, "plane");
  planeShader.use();
  planeShader.setInt("texture1", 0);
  planeShader.setInt("texture2", 1);
  planeShader.setInt("mask", 2);
  planeShader.setInt("shadowMap", 3);
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/grass.jpg").c_str(), true, "grass");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mountain.png").c_str(), true, "mountain");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mask.png").c_str(), false, "mask");
  Plane plane(glm::vec3(-125, 0, -125), glm::vec3(1, 0.3, 1), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height.jpg").c_str());

  // load grass
  auto grassShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/grass.vs").c_str(), FileSystem::getPath("src/final/final/grass.fs").c_str(), FileSystem::getPath("src/final/final/grass.gs").c_str(), "grass");
  Grass grass(glm::vec3(-125, 0, -125), glm::vec3(1, 0.3, 1), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height.jpg").c_str());
  Grass grass2(glm::vec3(-125.5, 0, -125.5), glm::vec3(1, 0.3, 1), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height.jpg").c_str());
  Grass grass3(glm::vec3(-125.8, 0, -125.8), glm::vec3(1, 0.3, 1), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height.jpg").c_str());
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/grass.png").c_str(), true, "t_grass");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/alpha.png").c_str(), true, "a_grass");
  grassShader.use();
  grassShader.setInt("texture1", 0);
  grassShader.setInt("alpha", 1);
  grassShader.setInt("shadowMap", 2);

  auto shadowShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/shadow_mapping.vs").c_str(), FileSystem::getPath("src/final/final/shadow_mapping.fs").c_str(), nullptr, "shadow_mapping");
  auto depthShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/shadow_mapping_depth.vs").c_str(), FileSystem::getPath("src/final/final/shadow_mapping_depth.fs").c_str(), nullptr, "shadow_mapping_depth");

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // create depth texture
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glm::vec3 lightPos(5, -100.0f, -100.0f);

  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto time = cos(glfwGetTime() / 5);
    lightPos = glm::vec3(5, 0, 0) + glm::vec3(0, 150 * cos(glfwGetTime() / 5), 150 * sin(glfwGetTime() / 5));

    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 250.5f;
    lightProjection = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    // render scene from light's point of view
    glm::mat4 model(1.0f);
	glm::mat4 modelModel = glm::translate(model ,glm::vec3(3.0f, 0.0f, 22.0f));
	modelModel = glm::scale(modelModel, glm::vec3(2.5, 3, 2.4));
    depthShader.use();
    depthShader.setMat4("model", model);
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //wood.Draw(&depthShader);
    plane.Draw(&depthShader);
	depthShader.setMat4("model", modelModel);
	ourModel.Draw(depthShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. render scene as normal using the generated depth/shadow map
    // --------------------------------------------------------------
	/*
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shadowShader.use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shadowShader.setMat4("projection", projection);
    shadowShader.setMat4("view", view);
    // set light uniforms
    shadowShader.setVec3("viewPos", camera.Position);
    shadowShader.setVec3("lightPos", lightPos);
    shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    wood.Draw(&shadowShader);*/

	// draw plane
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 view = camera.GetViewMatrix();
    auto planeShader = ResourceManager::GetShader("plane");
    planeShader.use();
    glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("mountain").ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("grass").ID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("mask").ID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    planeShader.setMat4("projection", projection);
    planeShader.setMat4("view", view);
    planeShader.setVec3("viewPos", camera.Position);
    planeShader.setVec3("lightPos", lightPos);
    planeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    plane.Draw(&planeShader);

    // draw grass

    grassShader.use();
    grassShader.setMat4("projection", projection);
    grassShader.setMat4("view", view);
    grassShader.setVec3("viewPos", camera.Position);
    grassShader.setVec3("lightPos", lightPos);
    grassShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    grassShader.setFloat("Time", glfwGetTime() / 10);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    grass.Draw(&grassShader);
    grass2.Draw(&grassShader);
    grass3.Draw(&grassShader);

    // draw frog
    ourShader.use();
    ourShader.setInt("shadowMap", 15);
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    ourShader.setVec3("viewPos", camera.Position);
    ourShader.setVec3("lightPos", lightPos);
    ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // render the loaded model
    ourShader.setMat4("model", modelModel);
	ourModel.Draw(ourShader);

    auto skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
	skyboxShader.setFloat("time", time);
    skybox.Draw(&skyboxShader);

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Delete all resources as loaded using the resource manager
  ResourceManager::Clear();

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
  // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
    {
    }

    else if (action == GLFW_RELEASE)
    {
    }
  }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime * 10);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime * 10);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime * 10);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime * 10);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}