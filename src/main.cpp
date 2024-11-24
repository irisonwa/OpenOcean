#pragma warning(disable : 5208)

#include "main.h"
using namespace std;

void init() {
    // enable debug callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    // glEnable(GL_POLYGON_OFFSET_FILL);
    // glPolygonOffset(1, 0);
    // glLineWidth(10);
    srand(time(nullptr));

    /// -------------------------------------------------- SHADERS -------------------------------------------------- ///
    Shader* s = new Shader("static", vert_smesh, frag_smesh);
    shaders[s->name] = s;

    Shader* s2 = new Shader("skybox", vert_sb, frag_sb);
    shaders[s2->name] = s2;

    Shader* s3 = new Shader("bones", vert_bmesh, frag_bmesh);
    shaders[s3->name] = s3;

    Shader* s4 = new Shader("variant", vert_vmesh, frag_vmesh);
    shaders[s4->name] = s4;

    /// -------------------------------------------------- LIGHTING -------------------------------------------------- ///
    staticLight = new Lighting("stony light", shaders["static"], MATERIAL_SHINY);
    boneLight = new Lighting("boney light", shaders["bones"], MATERIAL_SHINY);
    variantLight = new Lighting("variant light", shaders["variant"], MATERIAL_SHINY);

    /// -------------------------------------------------- STATIC MESHES -------------------------------------------------- ///
    // StaticMesh* m1 = new StaticMesh("boid", TEST_FISHB, 1024, 4);
    // smeshes[m1->name] = m1;
    StaticMesh* m2 = new StaticMesh("boid_display", MESH_SUB, 2048, 1);
    smeshes[m2->name] = m2;
    StaticMesh* m3 = new StaticMesh("ground", TEST_GROUND, 1024, 1);
    smeshes[m3->name] = m3;
    // StaticMesh* m4 = new StaticMesh("cplayer", TEST_FISHB, 1024, 1);
    // smeshes[m4->name] = m4;
    StaticMesh* m5 = new StaticMesh("sea", MESH_SEA);
    smeshes[m5->name] = m5;
    StaticMesh* m6 = new StaticMesh("kelp", MESH_KELP);
    smeshes[m6->name] = m6;
    StaticMesh* m7 = new StaticMesh("shark", MESH_SHARK, 1024, 1);
    smeshes[m7->name] = m7;
    StaticMesh* m8 = new StaticMesh("island", MESH_ISLAND);
    smeshes[m8->name] = m8;
    StaticMesh* m9 = new StaticMesh("sun", MESH_SUN);
    smeshes[m9->name] = m9;

    /// -------------------------------------------------- SKINNED MESHES -------------------------------------------------- ///
    BoneMesh* bm1 = new BoneMesh("test kelp", MESH_KELP_ANIM, shaders["bones"]);
    bmeshes[bm1->name] = bm1;
    // BoneMesh* bm2 = new BoneMesh("test guy", MESH_GUY_ANIM, shaders["bones"]);
    // bmeshes[bm2->name] = bm2;
    BoneMesh* bm3 = new BoneMesh("test shark", MESH_SHARK2_ANIM, shaders["bones"]);
    bmeshes[bm3->name] = bm3;
    // BoneMesh* bm4 = new BoneMesh("test sea", MESH_SEA_ANIM, shaders["bones"]);
    // bmeshes[bm4->name] = bm4;
    BoneMesh* bm5 = new BoneMesh("test threadfin", MESH_THREADFIN_ANIM, shaders["bones"]);
    bmeshes[bm5->name] = bm5;
    BoneMesh* bm6 = new BoneMesh("test marlin", MESH_MARLIN_ANIM, shaders["bones"]);
    bmeshes[bm6->name] = bm6;
    BoneMesh* bm7 = new BoneMesh("test spearfish", MESH_SPEARFISH_ANIM, shaders["bones"]);
    bmeshes[bm7->name] = bm7;

    /// -------------------------------------------------- PLAYER -------------------------------------------------- ///
    player = new Player("Player", vec3(0, 10, 0), Util::FORWARD);
    player->setMesh(MESH_PLAYER_ANIM, 2048, 1);
    player->setShader(shaders["bones"]);

    /// -------------------------------------------------- BOIDS -------------------------------------------------- ///
    vMesh = new VariantMesh(
        "vmesh", shaders["variant"],
        {
            {MESH_MARLIN_ANIM, 300, 1024, 2, std::vector<unsigned int>(300, 0)},  // black marlin
            {MESH_MARLIN_ANIM, 300, 1024, 2, std::vector<unsigned int>(300, 1)},  // blue marlin
            {MESH_THREADFIN_ANIM, 1600, 1024, 1, std::vector<unsigned int>(1600, 0)},
            {MESH_THREADFIN_ANIM, 170, 1024, 1, std::vector<unsigned int>(170, 0)},
            {MESH_SPEARFISH_ANIM, 300, 1024, 1, std::vector<unsigned int>(300, 0)},
            {MESH_SPEARFISH_ANIM, 300, 1024, 1, std::vector<unsigned int>(300, 0)},
            {MESH_SHARK2_ANIM, 32, 1024, 1, std::vector<unsigned int>(32, 0)},
            // {MESH_SHARK2_ANIM, 4, 1024, 1, std::vector<unsigned int>(4, 0)},
            // {MESH_PLAYER_ANIM, 10, -1, -1, std::vector<unsigned int>(10, 0)},
            // {TEST_SPEC, 1, 1024, 4, std::vector<unsigned int>(1, 3)},
            // {TEST_BOID, 3, 1024, 4, std::vector<unsigned int>(3, 2)},
            // {MESH_KELP, 3, 1024, 1, std::vector<unsigned int>(3, 3)},
            // {MESH_SUB, 2, 2048, 1, std::vector<unsigned int>(2, 3)},
            // {TEST_BOID, 100, 1024, 4, std::vector<unsigned int>(100, 1)},
            // {MESH_SUB, 1, 2048, 1, std::vector<unsigned int>(1, 3)},
        });
    flock = new Flock(vMesh);

    /// -------------------------------------------------- CUBEMAP -------------------------------------------------- ///
    cubemap = new Cubemap();
    cubemap->loadCubemap(cubemap_faces);

    /// -------------------------------------------------- OTHER -------------------------------------------------- ///
    float offset = 1.0f;
    float lim = 6;
    int j = 0;
    transm.resize(lim * lim * lim);
    for (int z = -lim / 2; z < lim / 2; z += 1) {
        for (int y = -lim / 2; y < lim / 2; y += 1) {
            for (int x = -lim / 2; x < lim / 2; x += 1) {
                vec3 translation(
                    (float)x * (rand() % spread),
                    (float)y,
                    (float)z * (rand() % spread));
                translations.push_back(translation);
                // int i = (z * lim * lim) + (y * lim) + x;
                transm[j] = scale(translate(mat4(1), translation), vec3(.25));
                j++;
            }
        }
    }

    float flashAttLin = 0.0022;
    float flashAttQuad = 0.00019;
    float sunlightLin = 0.00008;
    float sunlightQuad = 0.000008;
    staticLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    staticLight->addSpotLightAtt(sunPos, sunDir, vec3(.3), vec3(1), vec3(1));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    // staticLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));
    // staticLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    // staticLight->setDirLightColour(vec3(.05), vec3(.05), vec3(1));
    // staticLight->spotLights[staticLight->nSpotLights - 1].linear = 0.9;
    // staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = 0.3;

    boneLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    boneLight->spotLights[boneLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    boneLight->addSpotLightAtt(sunPos, sunDir, vec3(.3), vec3(1), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    boneLight->spotLights[boneLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    // boneLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));
    // boneLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    // boneLight->setDirLightColour(vec3(.2), vec3(.2), vec3(.2));
    // boneLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));

    variantLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    variantLight->spotLights[variantLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    variantLight->spotLights[variantLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    variantLight->addSpotLightAtt(sunPos, sunDir, vec3(.3), vec3(1), vec3(1));
    variantLight->spotLights[variantLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    variantLight->spotLights[variantLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    variantLight->spotLights[variantLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    
    // Set start time of program near when init() finishes loading
    SM::startTime = timeGetTime();
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable colour blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    // vec4 newBg = SM::bgColour * Util::mapRange(player->pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH, 0, 1);
    vec4 newBg = SM::bgColour * Util::mapRange(SM::camera->pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH, 0, 1);
    glClearColor(newBg.x, newBg.y, newBg.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = SM::camera->getViewMatrix();
    mat4 persp_proj = SM::camera->getProjectionMatrix();

    // Skybox
    // glDepthFunc(GL_LEQUAL);
    // mat4 nvm = mat4(mat3(SM::camera->getViewMatrix()));
    // shaders["skybox"]->use();
    // shaders["skybox"]->setInt("skybox", 0);
    // shaders["skybox"]->setMat4("view", nvm);
    // shaders["skybox"]->setMat4("proj", persp_proj);
    // cubemap->render();
    // glDepthFunc(GL_LESS);

    // update camera view and flashlight
    staticLight->setLightAtt(view, persp_proj, SM::camera->pos);
    staticLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    // staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    // staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    staticLight->use();

    // smeshes["kelp"]->render(mat4(1));
    smeshes["island"]->render(scale(mat4(1), vec3(25)));
    smeshes["sun"]->render(translate(mat4(1), sunPos));

    /// ---------------- SKINNED MESHES ---------------- ///
    boneLight->setLightAtt(view, persp_proj, SM::camera->pos);
    boneLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    // boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    // boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    boneLight->shader->setBool("showNormal", SM::showNormal);
    boneLight->use();

    if (SM::isFirstPerson) {
        player->lookAt(SM::camera->front);
        SM::camera->followTarget(player);
    } else if (SM::isThirdPerson) {
        player->render();
        player->lookAt(SM::camera->front);
        SM::camera->followTarget(player);
    } else /* isFreeCam */ {
        player->render();
    }

    bmeshes["test kelp"]->update(100);
    bmeshes["test kelp"]->render(transm.size(), transm.data());

    /// ---------------- VARIANT MESHES ---------------- ///
    variantLight->setLightAtt(view, persp_proj, SM::camera->pos);
    variantLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    variantLight->use();
    flock->process();
    flock->show();

    glutSwapBuffers();
}

void updateScene() {
    SM::updateDelta();
    // std::cout << 1 / SM::delta << std::endl; // fps
    if (!SM::isFreeCam) {
        player->processMovement();
    } else {
        SM::camera->processMovement();
    }
    flashlightCoords = SM::flashlightToggled ? (SM::isFreeCam ? SM::camera->pos : player->transform[3]) : vec3(-10000);
    flashlightDir = SM::flashlightToggled ? SM::camera->front : vec3(0, -1, 0);
    // Draw the next frame
    glutPostRedisplay();
}

// Process the mouse moving without button input
void passiveMouseMoved(int x, int y) {
    SM::updateMouse(x, y);
    SM::camera->processView();
}

void specKeyPressed(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        if (SM::isFreeCam) {
            SM::camera->DOWN = true;
        } else {
            player->DOWN = true;
        }
    }
}

void specKeyReleased(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        if (SM::isFreeCam) {
            SM::camera->DOWN = false;
        } else {
            player->DOWN = false;
        }
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

    if (key == ',') {
        SM::showNormal = !SM::showNormal;
    }

    if (key == '.') {
        flock->reset();
        // player->pos = vec3(0);
        // SM::camera->setPosition(vec3(0));
    }

    if (key == 'r' || key == 'R') SM::changeCameraState();
    if (SM::isFreeCam) {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = true;
        if (key == ' ') SM::camera->UP = true;
        if (key == 'w' || key == 'W') SM::camera->FORWARD = true;
        if (key == 's' || key == 'S') SM::camera->BACK = true;
        if (key == 'a' || key == 'A') SM::camera->LEFT = true;
        if (key == 'd' || key == 'D') SM::camera->RIGHT = true;
        if (key == 'e' || key == 'E') SM::camera->SPRINT = true;
    } else {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = true;
        if (key == ' ') player->UP = true;
        if (key == 'w' || key == 'W') player->FORWARD = true;
        if (key == 's' || key == 'S') player->BACK = true;
        if (key == 'a' || key == 'A') player->LEFT = true;
        if (key == 'd' || key == 'D') player->RIGHT = true;
        if (key == 'e' || key == 'E') player->SPRINT = true;
    }
}

// Function ran on key release
void keyReleased(unsigned char key, int x, int y) {
    if (SM::isFreeCam) {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = false;
        if (key == ' ') SM::camera->UP = false;
        if (key == 'w' || key == 'W') SM::camera->FORWARD = false;
        if (key == 's' || key == 'S') SM::camera->BACK = false;
        if (key == 'a' || key == 'A') SM::camera->LEFT = false;
        if (key == 'd' || key == 'D') SM::camera->RIGHT = false;
        if (key == 'e' || key == 'E') SM::camera->SPRINT = false;
        if (key == 'p' || key == 'P') SM::camera->CAN_FLY = !SM::camera->CAN_FLY;
    } else {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = false;
        if (key == ' ') player->UP = false;
        if (key == 'w' || key == 'W') player->FORWARD = false;
        if (key == 's' || key == 'S') player->BACK = false;
        if (key == 'a' || key == 'A') player->LEFT = false;
        if (key == 'd' || key == 'D') player->RIGHT = false;
        if (key == 'e' || key == 'E') player->SPRINT = false;
        if (key == 'p' || key == 'P') player->CAN_FLY = !player->CAN_FLY;
    }
}

int main(int argc, char** argv) {
    // Set up the window
    glutInit(&argc, argv);
    glutInitContextVersion(4, 6);
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
