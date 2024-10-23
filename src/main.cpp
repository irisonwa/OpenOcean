#pragma warning(disable : 5208)

#include "main.h"
using namespace std;

void init() {
    srand(time(nullptr));

    // Create shaders to use
    Shader* s = new Shader("base", vert_main, frag_main);
    shaders[s->name] = s;

    Shader* s2 = new Shader("skybox", vert_sb, frag_sb);
    shaders[s2->name] = s2;
    
    baseLight = new Lighting("stony light", shaders["base"], MATERIAL_WOOD);

    // Load meshes to be used
    Mesh* m1 = new Mesh("fish", TEST_FISH);
    meshes.emplace_back(m1);
    Mesh* m2 = new Mesh("ground", TEST_GROUND);
    meshes.emplace_back(m2);

    // Create skybox
    cubemap = new Cubemap();
    cubemap->loadCubemap(cubemap_faces);

    // Set start time of program near when init() finishes loading
    SM::startTime = timeGetTime();
    float offset = 1.0f;
    float lim = 10;
    for (int z = -lim / 2; z < lim / 2; z += 1) {
        for (int y = -lim / 2; y < lim / 2; y += 1) {
            for (int x = -lim / 2; x < lim / 2; x += 1) {
                vec3 translation(
                    (float)x + offset * (rand() % spread),
                    (float)y + offset * (rand() % spread),
                    (float)z + offset * (rand() % spread));
                translations.push_back(translation);
                scales.push_back(vec3(0.5 + rand() % 4));
            }
        }
    }
    for (int i = 0; i < translations.size(); ++i) {
        Boid* boid = new Boid(translations[i], translations[i], BoidType::FISH_1, i);
        boids.push_back(boid);
    }

    baseLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    baseLight->spotLights[baseLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    baseLight->spotLights[baseLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    float fct = 6.f;
    baseLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    baseLight->setDirLightColour(vec3(1, 0, 0.8f) / fct, vec3(1, 0, 0.8f) / fct,
                                 vec3(1, 0, 0.8f));
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable depth-testing
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.getViewMatrix();
    mat4 persp_proj = perspective(Util::deg2Rad(camera.FOV), (float)SM::width / (float)SM::height, 0.1f, 1000.0f);
    // Skybox
#pragma region SKYBOX
    // glDepthFunc(GL_LEQUAL);
    // mat4 nvm = mat4(mat3(camera.getViewMatrix()));
    // shaders["skybox"]->use();
    // shaders["skybox"]->setInt("skybox", 0);
    // shaders["skybox"]->setMat4("view", nvm);
    // shaders["skybox"]->setMat4("proj", persp_proj);
    // cubemap->render();
    // glDepthFunc(GL_LESS);
#pragma endregion

    // update camera view and flashlight
    baseLight->setLightAtt(view, persp_proj, camera.pos);
    baseLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    baseLight->spotLights[baseLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    baseLight->spotLights[baseLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    baseLight->use();

    // Transform each instance
    for (auto b : boids) {
        b->process(boids);
    }

    const unsigned int numInstances = boids.size();
    mat4 models[numInstances];
    for (int i = 0; i < numInstances; i++) {
        models[i] = scale(Util::lookTowards(boids[i]->pos, boids[i]->dir), scales[i]);
    }
    meshes[1]->render(translate(mat4(1), vec3(0, -10, 0)));
    meshes[0]->render(numInstances, models);  // draw cubes

    glutSwapBuffers();
}

void updateScene() {
    SM::updateDelta();
    camera.processMovement();
    flashlightCoords = SM::flashlightToggled ? camera.pos : vec3(-10000);
    flashlightDir = SM::flashlightToggled ? camera.front : vec3(0, -1, 0);
    // Draw the next frame
    glutPostRedisplay();
}

// Process the mouse moving without button input
void passiveMouseMoved(int x, int y) { camera.processView(x, y); }

void specKeyPressed(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        camera.DOWN = true;
    }
}

void specKeyReleased(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        camera.DOWN = false;
    }
}

// Function ran on key press
void keyPressed(unsigned char key, int x, int y) {
    if (key == VK_ESCAPE) {
        cout << endl
             << endl
             << "Exiting..." << endl;
        exit(0);
    }

    if (key == '.') {
        float offset = 1.0f;
        for (int z = -10; z < 10; z += 2) {
            for (int y = -10; y < 10; y += 2) {
                for (int x = -10; x < 10; x += 2) {
                    vec3 translation;
                    translation.x = (float)x + offset * (rand() % spread);
                    translation.y = (float)y + offset * (rand() % spread);
                    translation.z = (float)z + offset * (rand() % spread);
                    translations.push_back(translation);
                }
            }
        }
    }

    if (key == 'q') SM::flashlightToggled = true;
    if (key == ' ') camera.UP = true;
    if (key == 'w' || key == 'W') camera.FORWARD = true;
    if (key == 's' || key == 'S') camera.BACK = true;
    if (key == 'a' || key == 'A') camera.LEFT = true;
    if (key == 'd' || key == 'D') camera.RIGHT = true;
    if (key == 'e' || key == 'E') camera.SPRINT = true;
    if (key == 'p' || key == 'P') camera.CAN_FLY = !camera.CAN_FLY;
}

// Function ran on key release
void keyReleased(unsigned char key, int x, int y) {
    if (key == 'q') SM::flashlightToggled = false;
    if (key == ' ') camera.UP = false;
    if (key == 'w' || key == 'W') camera.FORWARD = false;
    if (key == 's' || key == 'S') camera.BACK = false;
    if (key == 'a' || key == 'A') camera.LEFT = false;
    if (key == 'd' || key == 'D') camera.RIGHT = false;
    if (key == 'e' || key == 'E') camera.SPRINT = false;
}

int main(int argc, char** argv) {
    // Set up the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL | GLUT_DEPTH);
    glutInitWindowSize(SM::width, SM::height);
    glutCreateWindow("Final Project");

    // Tell glut where the display function is
    glutIgnoreKeyRepeat(true);
    glutDisplayFunc(display);
    glutIdleFunc(updateScene);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutSpecialFunc(specKeyPressed);
    glutSpecialUpFunc(specKeyReleased);
    glutPassiveMotionFunc(passiveMouseMoved);
    glutSetCursor(GLUT_CURSOR_NONE);  // hide cursor

    // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
    // Check for any errors
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    // Set up your objects and shaders
    init();
    // Begin infinite event loop
    glutMainLoop();
    return 0;
}
