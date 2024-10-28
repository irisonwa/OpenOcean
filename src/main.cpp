#pragma warning(disable : 5208)

#include "main.h"
using namespace std;

void init() {
    // enable debug callback
    // glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);

    srand(time(nullptr));

    // Create shaders to use
    Shader* s = new Shader("base", vert_main, frag_main);
    shaders[s->name] = s;

    Shader* s2 = new Shader("skybox", vert_sb, frag_sb);
    shaders[s2->name] = s2;

    Shader* s3 = new Shader("bones", vert_bmesh, frag_bmesh);
    shaders[s3->name] = s3;

    baseLight = new Lighting("stony light", shaders["base"], MATERIAL_SHINY);
    boneLight = new Lighting("boney light", shaders["bones"], MATERIAL_SHINY);

    // Load meshes to be used
    Mesh* m1 = new Mesh("boid", TEST_FISHB, 1024, 4);
    meshes[m1->name] = m1;
    Mesh* m2 = new Mesh("boid_display", MESH_PLAYER, 2048, 1);
    meshes[m2->name] = m2;
    Mesh* m3 = new Mesh("ground", TEST_GROUND, 1024, 1);
    meshes[m3->name] = m3;
    Mesh* m4 = new Mesh("cplayer", TEST_FISHB, 1024, 1);
    meshes[m4->name] = m4;
    Mesh* m5 = new Mesh("cube", TEST_SPEC, 1024, 4);
    meshes[m5->name] = m5;
    BoneMesh* bm1 = new BoneMesh("player2", MESH_PLAYER_ANIM);
    bmeshes[bm1->name] = bm1;

    player = new Player("Player", MESH_PLAYER_ANIM, 2048, 1, vec3(10), SM::FORWARD);
    player->setShader(shaders["bones"]);

    // Create skybox
    cubemap = new Cubemap();
    cubemap->loadCubemap(cubemap_faces);

    // Set start time of program near when init() finishes loading
    SM::startTime = timeGetTime();
    float offset = 1.0f;
    float lim = 10;

    vector<float> scls = {1.5, 1, 1.5, 2};
    for (int z = -lim / 2; z < lim / 2; z += 1) {
        for (int y = -lim / 2; y < lim / 2; y += 1) {
            for (int x = -lim / 2; x < lim / 2; x += 1) {
                vec3 translation(
                    (float)x + offset * (rand() % spread),
                    (float)y + offset * (rand() % spread),
                    (float)z + offset * (rand() % spread));
                translations.push_back(translation);
                int idx = rand() % 4;
                // printf("%d\n", idx);
                scales.push_back(vec3(scls[idx]));
                depths.push_back(idx);
                // scales.push_back(vec3(.5));
                // mat_idxs.push_back()
            }
        }
    }
    for (int i = 0; i < translations.size(); ++i) {
        Boid* boid = new Boid(translations[i], normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10)), BoidType::FISH_1, i);
        boids.push_back(boid);
    }

    baseLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    baseLight->spotLights[baseLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    baseLight->spotLights[baseLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    baseLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    baseLight->setDirLightColour(vec3(.05), vec3(.05), vec3(1));

    boneLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    boneLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    boneLight->setDirLightColour(vec3(.05), vec3(.05), vec3(1));
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable colour blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.getViewMatrix();
    mat4 persp_proj = perspective(Util::deg2Rad(camera.FOV), (float)SM::width / (float)SM::height, 0.1f, 1000.0f);

    // Skybox
    // glDepthFunc(GL_LEQUAL);
    // mat4 nvm = mat4(mat3(camera.getViewMatrix()));
    // shaders["skybox"]->use();
    // shaders["skybox"]->setInt("skybox", 0);
    // shaders["skybox"]->setMat4("view", nvm);
    // shaders["skybox"]->setMat4("proj", persp_proj);
    // cubemap->render();
    // glDepthFunc(GL_LESS);

    // update camera view and flashlight
    baseLight->setLightAtt(view, persp_proj, camera.pos);
    baseLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    baseLight->spotLights[baseLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    baseLight->spotLights[baseLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    baseLight->use();

    const unsigned int numInstances = boids.size();
    mat4 models[numInstances];
    for (int i = 0; i < numInstances; i++) {
        boids[i]->process(boids);
        models[i] = scale(Util::lookTowards(boids[i]->pos, boids[i]->dir), scales[i]);
    }
    mat4 cubes[4] = {translate(mat4(1), vec3(20, 0, 10)), translate(mat4(1), vec3(10, 0, 0)), translate(mat4(1), vec3(20, 0, 0)), translate(mat4(1), vec3(30, 0, 0))};
    auto cubeds = std::vector<float>{0, 1, 2, 3}.data();

    meshes["boid"]->render(numInstances, models, depths.data());  // draw cubes
    // meshes["boid_display"]->render(4, cubes, cubeds); // display
    // meshes["ground"]->render(translate(mat4(1), vec3(0, -10, 0)), 1);
    meshes["cube"]->render(scale(mat4(1), vec3(250)), 1);


    /// ---------------- SKINNED MESHES ---------------- ///
    boneLight->setLightAtt(view, persp_proj, camera.pos);
    boneLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    boneLight->use();

    player->lookAt(camera.front);
    player->render();
    camera.followTarget(player);
    
    glutSwapBuffers();
}

void updateScene() {
    SM::updateDelta();
    // std::cout << 1 / SM::delta << std::endl; // fps
    player->processMovement(camera);
    flashlightCoords = SM::flashlightToggled ? player->transform[3] : vec3(-10000);
    flashlightDir = SM::flashlightToggled ? camera.front : vec3(0, -1, 0);
    // Draw the next frame
    glutPostRedisplay();
}

// Process the mouse moving without button input
void passiveMouseMoved(int x, int y) { 
    SM::updateMouse(x, y);
    camera.processView();
}

void specKeyPressed(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        player->DOWN = true;
    }
}

void specKeyReleased(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        player->DOWN = false;
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
        for (auto b : boids) {
            b->pos = vec3(
                (float)((rand() % 600) - 300),
                (float)((rand() % 600) - 300),
                (float)((rand() % 600) - 300));
        }
    }

    if (key == 'q' || key == 'Q') SM::flashlightToggled = true;
    if (key == ' ') player->UP = true;
    if (key == 'w' || key == 'W') player->FORWARD = true;
    if (key == 's' || key == 'S') player->BACK = true;
    if (key == 'a' || key == 'A') player->LEFT = true;
    if (key == 'd' || key == 'D') player->RIGHT = true;
    if (key == 'e' || key == 'E') player->SPRINT = true;
    if (key == 'p' || key == 'P') player->CAN_FLY = !player->CAN_FLY;
    // if (key == 'r' || key == 'R') player->CAN_FLY = !player->CAN_FLY;
}

// Function ran on key release
void keyReleased(unsigned char key, int x, int y) {
    if (key == 'q' || key == 'Q') SM::flashlightToggled = false;
    if (key == ' ') player->UP = false;
    if (key == 'w' || key == 'W') player->FORWARD = false;
    if (key == 's' || key == 'S') player->BACK = false;
    if (key == 'a' || key == 'A') player->LEFT = false;
    if (key == 'd' || key == 'D') player->RIGHT = false;
    if (key == 'e' || key == 'E') player->SPRINT = false;
    // if (key == 'r' || key == 'R') player->SPRINT = false;
}

int main(int argc, char** argv) {
    // Set up the window
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SM::width, SM::height);
    glutCreateWindow("Open Ocean");

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
