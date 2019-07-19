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
#include "fluid.h"
#include <iostream>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(20.0f, 10.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 文字显示-----
/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint textVAO, textVBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
//文字显示-----

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
  
  //文字显示-----
  //glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  //glEnable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Compile and setup the shader
  Shader textshader(FileSystem::getPath("src/final/final/text.vs").c_str(), FileSystem::getPath("src/final/final/text.fs").c_str());
  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
  textshader.use();
  glUniformMatrix4fv(glGetUniformLocation(textshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  // FreeType
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft))
	  std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, FileSystem::getPath("fonts/consolab.ttf").c_str(), 0, &face))
	  std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 48);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++) {
	  // Load character glyph 
	  if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
		  std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		  continue;
	  }
	  // Generate texture
	  GLuint texture;
	  glGenTextures(1, &texture);
	  glBindTexture(GL_TEXTURE_2D, texture);
	  glTexImage2D(
		  GL_TEXTURE_2D,
		  0,
		  GL_RED,
		  face->glyph->bitmap.width,
		  face->glyph->bitmap.rows,
		  0,
		  GL_RED,
		  GL_UNSIGNED_BYTE,
		  face->glyph->bitmap.buffer
	  );
	  // Set texture options
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  // Now store character for later use
	  Character character = {
		  texture,
		  glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		  glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		  face->glyph->advance.x
	  };
	  Characters.insert(std::pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);


  // Configure textVAO/textVBO for texture quads
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glBindVertexArray(textVAO);
  glBindBuffer(GL_ARRAY_BUFFER, textVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  //文字显示-----
  
  // laod frog

  // build and compile shaders
  // -------------------------
  Shader ourShader(FileSystem::getPath("src/final/final/model.vs").c_str(), FileSystem::getPath("src/final/final/model.fs").c_str());

  // load models
  // -----------
  Model ourModel(FileSystem::getPath("resources/landscape/mill.obj"));
  cout << FileSystem::getPath("") << endl;

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

  /*
  // load plane
  auto planeShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/plane.vs").c_str(), FileSystem::getPath("src/final/final/plane.fs").c_str(), nullptr, "plane");
  planeShader.use();
  planeShader.setInt("texture1", 0);
  planeShader.setInt("texture2", 1);
  planeShader.setInt("mask", 2);
  planeShader.setInt("shadowMap", 3);
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/grass_2.jpg").c_str(), true, "grass");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mountain.png").c_str(), true, "mountain");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/plane/mask.png").c_str(), false, "mask");
  Plane plane(glm::vec3(-125, 0, -125), glm::vec3(1, 0.3, 1), glm::vec3(1), FileSystem::getPath("resources/textures/plane/height_2.jpg").c_str());
  */
  
  // load grass
  
  auto grassShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/grass.vs").c_str(), FileSystem::getPath("src/final/final/grass.fs").c_str(), FileSystem::getPath("src/final/final/grass.gs").c_str(), "grass");
  glm::vec3 grassPos(-40.0f, -6.0f, 30.0f);
  glm::vec3 grassScale(2, 0.6, 2);
  Grass grass(grassPos, grassScale, glm::vec3(1), 25, 5, 0.5);
  glm::vec3 grassPos2(-25.0f, -6.0f, 30.0f);
  Grass grass2(grassPos2, grassScale, glm::vec3(1), 7, 11, 0.4);
  glm::vec3 grassPos3(30.0f, -4.0f, 35.0f);
  Grass grass3(grassPos3, grassScale, glm::vec3(1), 11, 11, 0.4);
  glm::vec3 grassPos4(-35.0f, -6.0f, 45.0f);
  Grass grass4(grassPos4, grassScale, glm::vec3(1), 7, 7, 0.6);
  glm::vec3 grassPos5(-38.0f, -6.0f, 45.0f);
  Grass grass5(grassPos5, grassScale, glm::vec3(1), 7, 7, 0.6);
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/grass.png").c_str(), true, "t_grass");
  ResourceManager::LoadTexture(FileSystem::getPath("resources/textures/alpha.png").c_str(), true, "a_grass");
  grassShader.use();
  grassShader.setInt("texture1", 0);
  grassShader.setInt("alpha", 1);
  grassShader.setInt("shadowMap", 2);

  auto shadowShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/shadow_mapping.vs").c_str(), FileSystem::getPath("src/final/final/shadow_mapping.fs").c_str(), nullptr, "shadow_mapping");
  auto depthShader = ResourceManager::LoadShader(FileSystem::getPath("src/final/final/shadow_mapping_depth.vs").c_str(), FileSystem::getPath("src/final/final/shadow_mapping_depth.fs").c_str(), nullptr, "shadow_mapping_depth");

  const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
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

  glm::vec3 lightPos(5, 100.0f, 100.0f);
  /*
  // load lake

  Fluid fluid(FileSystem::getPath("src/final/final/gerstner.vs"), 
	  FileSystem::getPath("src/final/final/gerstner.fs"), 
	  FileSystem::getPath("resources/wave/water-texture-2.tga"), 
	  FileSystem::getPath("resources/wave/water-texture-2-normal.tga"));
*/

  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto time = cos(glfwGetTime() / 10);
    lightPos = glm::vec3(5, 0, 0) + glm::vec3(0, 100 * cos(glfwGetTime() / 10), 100 * sin(glfwGetTime() / 10));

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
	glm::mat4 modelModel = glm::translate(model ,glm::vec3(3.0f, -1.0f, 12.0f));
	modelModel = glm::scale(modelModel, glm::vec3(50, 50, 50));
    depthShader.use();
    depthShader.setMat4("model", model);
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //wood.Draw(&depthShader);
    //plane.Draw(&depthShader);
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

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 view = camera.GetViewMatrix();
	/*


	// draw plane
	
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
	*/
	
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
    grass4.Draw(&grassShader);
    grass5.Draw(&grassShader);
	
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
	
	
	
	/*
	// render lake

	glm::mat4 Projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 ModelViewMat = camera.GetViewMatrix();
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-2, 0, 24));
	modelMat = glm::scale(modelMat, glm::vec3(3, 1, 3));
	modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 NormalMat = glm::transpose(glm::inverse(glm::mat3(ModelViewMat)));
	glUseProgram(fluid.dataset.program);
	glUniformMatrix4fv(glGetUniformLocation(fluid.dataset.program, "modelViewMat"), 1, GL_FALSE, glm::value_ptr(ModelViewMat));
	glUniformMatrix4fv(glGetUniformLocation(fluid.dataset.program, "perspProjMat"), 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix3fv(glGetUniformLocation(fluid.dataset.program, "normalMat"), 1, GL_FALSE, glm::value_ptr(NormalMat));
	glUniformMatrix4fv(glGetUniformLocation(fluid.dataset.program, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
	//glUniform4fv(glGetUniformLocation(fluid.dataset.program, "modelViewMat"), 1, glm::value_ptr(ModelViewMat));
	//glUniform4fv(glGetUniformLocation(fluid.dataset.program, "perspProjMat"), 1, glm::value_ptr(Projection));
	//glUniform3fv(glGetUniformLocation(fluid.dataset.program, "normalMat"), 1, glm::value_ptr(NormalMat));
	//glUniform3fv(glGetUniformLocation(fluid.dataset.program, "modelMat"), 1, glm::value_ptr(modelMat));
	
	fluid.calculateWave();
	glBindVertexArray(fluid.VAO);

	glUniform1f(glGetUniformLocation(fluid.dataset.program, "time"), fluid.water.time);
	glUniform3fv(glGetUniformLocation(fluid.dataset.program, "lightPos"), 1, glm::value_ptr(lightPos));

	glBindBuffer(GL_ARRAY_BUFFER, fluid.dataset.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fluid.vertex_data), fluid.vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(fluid.dataset.attributes.position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(fluid.dataset.attributes.position);

	glBindBuffer(GL_ARRAY_BUFFER, fluid.dataset.normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fluid.normal_data), fluid.normal_data, GL_STATIC_DRAW);
	glVertexAttribPointer(fluid.dataset.attributes.normal, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(fluid.dataset.attributes.normal);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fluid.dataset.diffuse_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fluid.dataset.normal_texture);
	for (int c = 0; c < (STRIP_COUNT - 1); c++) {
		glDrawArrays(GL_TRIANGLE_STRIP, STRIP_LENGTH * 2 * c, STRIP_LENGTH * 2);
	}
	*/

	// render skybox
    auto skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
	skyboxShader.setFloat("time", time);
    skybox.Draw(&skyboxShader);

	//文字显示----
	string texttime = to_string(int(currentFrame / 5 / 3.1416 / 2 * 24 + 12) % 24);
	string mytext = "time: " + texttime + ":00";
	RenderText(textshader, mytext, 570.0f, 570.0f, 0.5f, glm::vec3(1.0f, 0.7f, 0.3f));

	//文字显示----

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
	//fluid.water.time += 0.05;
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

// 显示文字----
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	// Activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
// 显示文字----