#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/camera.h>

#include "wood.h"
#include "skybox.h"
#include "plane.h"
#include "resource_manager.h"

#include <iostream>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const GLuint SCREEN_WIDTH = 800;

const GLuint SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
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

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "final", nullptr, nullptr);
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

  // load wood
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/wood.jpg").c_str(), false, "wood");
  ResourceManager::LoadShader(FileSystem::getPath("src/final/final/wood.vs").c_str(), FileSystem::getPath("src/final/final/wood.fs").c_str(), nullptr, "wood");
  auto wood = Wood(glm::vec3(0, 0, 0), glm::vec3(2, 0.2, 2), glm::vec3(1));


  // load skybox
  ResourceManager::LoadShader(FileSystem::getPath("src/final/final/skybox.vs").c_str(), FileSystem::getPath("src/final/final/skybox.fs").c_str(), nullptr, "skybox");
  std::vector<std::string>
      faces{
          FileSystem::getPath("resources/textures/sor_sea/sea_ft.jpg"),
          FileSystem::getPath("resources/textures/sor_sea/sea_bk.jpg"),
          FileSystem::getPath("resources/textures/sor_sea/sea_up.jpg"),
          FileSystem::getPath("resources/textures/sor_sea/sea_dn.jpg"),
          FileSystem::getPath("resources/textures/sor_sea/sea_rt.jpg"),
          FileSystem::getPath("resources/textures/sor_sea/sea_lf.jpg")};
  Skybox skybox(glm::vec3(1), glm::vec3(1), glm::vec3(1), faces, "skybox_0");


  // load plane
  auto planeShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/plane.vs").c_str(), FileSystem::getPath("src/final/final/plane.fs").c_str(), nullptr, "plane");
  planeShader.use();
  planeShader.setInt("texture1", 0);
  planeShader.setInt("texture2", 1);
  planeShader.setInt("mask", 2);
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/grass.png").c_str(), true, "grass");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mountain.png").c_str(), true, "mountain");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mask.png").c_str(), false, "mask");

  Plane plane(glm::vec3(-100, 0, -100), glm::vec3(0.3, 0.3, 0.3), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height.jpg").c_str());

  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 200.0f);
    glm::mat4 view = camera.GetViewMatrix();

    auto woodShader = ResourceManager::GetShader("wood");
    woodShader.use();
    woodShader.setMat4("projection", projection);
    woodShader.setMat4("view", view);
    wood.Draw(&woodShader);

    auto planeShader = ResourceManager::GetShader("plane");
    planeShader.use();
    glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("mountain").ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("grass").ID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("mask").ID);
    planeShader.setMat4("projection", projection);
    planeShader.setMat4("view", view);
    plane.Draw(&planeShader);

    auto skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
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
	float dt = deltaTime * 10;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, dt);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, dt);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, dt);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, dt);
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