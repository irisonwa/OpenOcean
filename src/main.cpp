#pragma warning(disable : 5208)

#include "main.h"
using namespace std;

void init() {
    // enable debug callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    srand(time(nullptr));

    /// -------------------------------------------------- SHADERS -------------------------------------------------- ///
    Shader* s = new Shader("static", vert_smesh, frag_smesh);
    shaders[s->name] = s;

    Shader* s2 = new Shader("skybox", vert_sb, frag_sb);
    shaders[s2->name] = s2;

    Shader* s3 = new Shader("bones", vert_bmesh, frag_bmesh);
    shaders[s3->name] = s3;
#ifdef TREE
    Shader* s4 = new Shader("variant_skinned", vert_vmesh_cpu, frag_vmesh);
    shaders[s4->name] = s4;
#else
    Shader* s4 = new Shader("variant_skinned", vert_vmesh_gpu, frag_vmesh);
    shaders[s4->name] = s4;
#endif
    Shader* s5 = new Shader("variant_static", vert_vmesh_cpu, frag_vmesh);
    shaders[s5->name] = s5;

    /// -------------------------------------------------- LIGHTING -------------------------------------------------- ///
    staticLight = new Lighting("stony light", shaders["static"], MATERIAL_SHINY);
    boneLight = new Lighting("boney light", shaders["bones"], MATERIAL_SHINY);
    variantLight = new Lighting("variant light", shaders["variant_skinned"], MATERIAL_SHINY);

    /// -------------------------------------------------- STATIC MESHES -------------------------------------------------- ///
    StaticMesh* m1 = new StaticMesh("beacon", MESH_BEACON);
    smeshes[m1->name] = m1;
    StaticMesh* m2 = new StaticMesh("beach_items", MESH_BEACH_ITEM);
    smeshes[m2->name] = m2;
    StaticMesh* m3 = new StaticMesh("beach", MESH_BEACH);
    smeshes[m3->name] = m3;
    StaticMesh* m4 = new StaticMesh("terr", MESH_TERRAIN);
    smeshes[m4->name] = m4;
    // StaticMesh* m5 = new StaticMesh("sea", MESH_SEA);
    // smeshes[m5->name] = m5;
    // StaticMesh* m6 = new StaticMesh("kelp", MESH_KELP);
    // smeshes[m6->name] = m6;
    StaticMesh* m7 = new StaticMesh("anemone", MESH_ANEMONE);
    smeshes[m7->name] = m7;
    StaticMesh* m8 = new StaticMesh("island", MESH_ISLAND);
    smeshes[m8->name] = m8;
    StaticMesh* m9 = new StaticMesh("sun", MESH_BEACON);
    smeshes[m9->name] = m9;

    /// -------------------------------------------------- SKINNED MESHES -------------------------------------------------- ///
    BoneMesh* bm1 = new BoneMesh("kelp", MESH_KELP_ANIM, shaders["bones"]);
    bmeshes[bm1->name] = bm1;
    // BoneMesh* bm2 = new BoneMesh("test guy", MESH_GUY_ANIM, shaders["bones"]);
    // bmeshes[bm2->name] = bm2;
    BoneMesh* bm3 = new BoneMesh("test shark", MESH_BLUE_SHARK_ANIM, shaders["bones"]);
    bmeshes[bm3->name] = bm3;
    // BoneMesh* bm4 = new BoneMesh("terr", MESH_TERRAIN, shaders["bones"]);
    // bmeshes[bm4->name] = bm4;
    BoneMesh* bm5 = new BoneMesh("test threadfin", MESH_THREADFIN_ANIM, shaders["bones"]);
    bmeshes[bm5->name] = bm5;
    BoneMesh* bm6 = new BoneMesh("test marlin", MESH_MARLIN_ANIM, shaders["bones"]);
    bmeshes[bm6->name] = bm6;
    BoneMesh* bm7 = new BoneMesh("test spearfish", MESH_SPEARFISH_ANIM, shaders["bones"]);
    bmeshes[bm7->name] = bm7;

    /// -------------------------------------------------- PLAYER -------------------------------------------------- ///
    player = new Player("Player", vec3(288.050171, 271.612457, 257.632996), Util::FORWARD);
    player->setMesh(MESH_PLAYER_ANIM, -1, -1);
    player->setShader(shaders["bones"]);

    /// -------------------------------------------------- BOIDS -------------------------------------------------- ///
    float offset = 1.0f;
    float lim = 6;
    int spread = 60;
    for (vec3 p : anemonePos) {
        anemoneMats.push_back(translate(scale(mat4(1), vec3(.25)), p * vec3(4)));
        for (int z = -lim / 2; z < lim / 2; z += 1) {
            for (int y = -lim / 2; y < lim / 2; y += 1) {
                for (int x = -lim / 2; x < lim / 2; x += 1) {
                    if (x == 0 || y == 0 || z == 0) continue;
                    vec3 translation(
                        p.x + (float)x * (rand() % spread),
                        p.y + (float)y * (rand() % (spread / 4)),
                        p.z + (float)z * (rand() % spread));
                    kelpMats.push_back(scale(translate(mat4(1), translation), vec3(.25)));
                }
            }
        }
    }
    vMesh = new VariantMesh(
        "vmesh", shaders["variant_skinned"], VariantType::SKINNED,
        {
            {MESH_BLUE_SHARK_ANIM   , 120,  -1,   -1, std::vector<unsigned int>(120, 0)},
            {MESH_WHITE_SHARK_ANIM  , 32,   -1,   -1, std::vector<unsigned int>(32, 0)},
            {MESH_WHALE_SHARK_ANIM  , 32,   -1,   -1, std::vector<unsigned int>(32, 0)},
            {MESH_WHALE_ANIM        , 4,    -1,   -1, std::vector<unsigned int>(4, 0)},
            {MESH_MARLIN_ANIM       , 300,  1024,  2, std::vector<unsigned int>(300, 0)},  // black marlin
            {MESH_MARLIN_ANIM       , 300,  1024,  2, std::vector<unsigned int>(300, 1)},  // blue marlin
            {MESH_SPEARFISH_ANIM    , 300,  -1,   -1, std::vector<unsigned int>(300, 0)},
            {MESH_DOLPHIN_ANIM      , 100,  -1,   -1, std::vector<unsigned int>(100, 0)},
            {MESH_CLOWNFISH_ANIM    , 1500, -1,   -1, std::vector<unsigned int>(1500, 0)},
            {MESH_HERRING_ANIM      , 1500, -1,   -1, std::vector<unsigned int>(1500, 0)},
            {MESH_PLANKTON_ANIM     , 800,  -1,   -1, std::vector<unsigned int>(800, 0)},
            {MESH_THREADFIN_ANIM    , 1500, -1,   -1, std::vector<unsigned int>(1500, 0)},

            // {MESH_THREADFIN_ANIM, 5000, 1024, 1, std::vector<unsigned int>(5000, 0)},
            // {MESH_SIMPLE_ANIM, 5000, 1024, 1, std::vector<unsigned int>(5000, 0)},
            // {MESH_THREADFIN_ANIM, 170, 1024, 1, std::vector<unsigned int>(170, 0)},
            // {MESH_THREADFIN_ANIM, 170, 1024, 1, std::vector<unsigned int>(170, 0)},
            // {MESH_SHARK2_ANIM, 2, 1024, 1, std::vector<unsigned int>(2, 0)},
            // {MESH_THREADFIN_ANIM, 31, 1024, 1, std::vector<unsigned int>(31, 0)},
            // {MESH_SHARK2_ANIM, 4, 1024, 1, std::vector<unsigned int>(4, 0)},
            // {MESH_PLAYER_ANIM, 10, -1, -1, std::vector<unsigned int>(10, 0)},
            // {TEST_SPEC, 1, 1024, 4, std::vector<unsigned int>(1, 3)},
            // {TEST_BOID, 3, 1024, 4, std::vector<unsigned int>(3, 2)},
            // {MESH_KELP, 3, 1024, 1, std::vector<unsigned int>(3, 3)},
            // {MESH_SUB, 2, 2048, 1, std::vector<unsigned int>(2, 3)},
            // {TEST_BOID, 100, 1024, 4, std::vector<unsigned int>(100, 1)},
            // {MESH_SUB, 1, 2048, 1, std::vector<unsigned int>(1, 3)},
        });
    flock = new Flock(vMesh, anemonePos);

    /// -------------------------------------------------- CUBEMAP -------------------------------------------------- ///
    // cubemap = new Cubemap();
    // cubemap->loadCubemap(cubemap_faces);

    /// -------------------------------------------------- OTHER -------------------------------------------------- ///
    float beaconAttLin = 0.00000009;
    float beaconAttQuad = 0.000000009;
    beaconLightMats.resize(beaconLightPos.size());
    for (vec3 p : beaconLightPos) {
        vec3 ofst = vec3(0, 2.f, 0);
        beaconLightMats.push_back(translate(mat4(1), p));
        staticLight->addPointLightAtt(p + ofst, vec3(1), vec3(1), vec3(1));
        staticLight->spotLights[staticLight->nSpotLights - 1].linear = beaconAttLin;
        staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = beaconAttQuad;
        boneLight->addPointLightAtt(p + ofst, vec3(1), vec3(1), vec3(1));
        boneLight->spotLights[boneLight->nSpotLights - 1].linear = beaconAttLin;
        boneLight->spotLights[boneLight->nSpotLights - 1].quadratic = beaconAttQuad;
        variantLight->addPointLightAtt(p + ofst, vec3(1), vec3(1), vec3(1));
        variantLight->spotLights[variantLight->nSpotLights - 1].linear = beaconAttLin;
        variantLight->spotLights[variantLight->nSpotLights - 1].quadratic = beaconAttQuad;
    }

    float flashAttLin = 0.0022;
    float flashAttQuad = 0.00019;
    float sunlightLin = 0.0004;
    float sunlightQuad = 0.00004;
    float innerCutOff = cos(Util::d2r(170.f));
    float outerCutOff = cos(Util::d2r(179.f));
    staticLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::d2r(24.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::d2r(35.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    staticLight->addSpotLightAtt(sunPos, sunDir, vec3(.3), vec3(1), vec3(1));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = innerCutOff;
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = outerCutOff;
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
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = innerCutOff;
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = outerCutOff;
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
    variantLight->spotLights[variantLight->nSpotLights - 1].cutOff = innerCutOff;
    variantLight->spotLights[variantLight->nSpotLights - 1].outerCutOff = outerCutOff;

    // Set start time of program near when init() finishes loading
    SM::startTime = timeGetTime();
    SM::sceneBox->loadWireframe();
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable colour blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    // vec4 newBg = SM::bgColour * Util::mapRange(player->pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH, 0, 1);
    vec4 newBg = SM::bgColour * Util::mapRange(SM::camera->pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH, 0.1, 1);
    SM::seaLevel = SM::isFreeCam ? -1000 : 300;
    glClearColor(newBg.x, newBg.y, newBg.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = SM::camera->getViewMatrix();
    mat4 persp_proj = SM::camera->getProjectionMatrix();

    /// ------------------------------------------------ STATIC MESHES ------------------------------------------------ ///
    // update camera view and flashlight
    staticLight->setLightAtt(view, persp_proj, SM::camera->pos);
    staticLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    staticLight->use();

    // smeshes["kelp"]->render(mat4(1));
    // smeshes["island"]->render(translate(scale(mat4(1), vec3(1)), vec3(0, 20, 0)));
    smeshes["sun"]->render(translate(mat4(1), sunPos));
    smeshes["terr"]->render(translate(scale(mat4(1), vec3(1.f)), vec3(0, -220, 0)));
    smeshes["beacon"]->render(beaconLightMats.size(), beaconLightMats.data());
    smeshes["anemone"]->render(anemoneMats.size(), anemoneMats.data());
    smeshes["beach_items"]->render(translate(mat4(1), vec3(288.050171, 271.612457, 257.632996)));
    smeshes["beach"]->render(translate(scale(mat4(1), vec3(22)), vec3(13, -6, 13)));

    /// ------------------------------------------------ SKINNED MESHES ------------------------------------------------ ///
    boneLight->setLightAtt(view, persp_proj, SM::camera->pos);
    boneLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    boneLight->shader->setBool("showNormal", SM::showNormal);
    boneLight->use();

    if (SM::isFirstPerson) {
        player->lookAt(SM::camera->front);
        SM::camera->followTarget(vec3(player->transform[3]), Util::FORWARD);
    } else if (SM::isThirdPerson) {
        player->render();
        player->lookAt(SM::camera->front);
        SM::camera->followTarget(vec3(player->transform[3]), Util::FORWARD);
    } else /* isFreeCam */ {
        player->render();
        player->lookAt(player->dir);
        // SM::sceneBox->drawWireframe();
    }

    bmeshes["kelp"]->update(100);
    bmeshes["kelp"]->render(kelpMats.size(), kelpMats.data());

    /// ------------------------------------------------ VARIANT MESHES ------------------------------------------------ ///
    variantLight->setLightAtt(view, persp_proj, SM::camera->pos);
    variantLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    variantLight->use();
    flock->process(player->pos, SM::updateDistance);
    flock->show();

    glutSwapBuffers();
}

void updateScene() {
    SM::updateDelta();
    // std::cout << 1 / SM::delta << std::endl; // fps
    // if (!SM::isFreeCam) {
    // } else {
    // }
    player->processMovement();
    SM::camera->processMovement();
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

void passiveMouseWheel(int button, int dir, int x, int y) {
    const float MAX_DISTANCE = 1024;
    const float inc = 5;
    if (dir > 0) {
        // increase "render" distance
        SM::updateDistance = Util::clamp(SM::updateDistance - inc, SM::fogBounds.x, MAX_DISTANCE);
        SM::fogBounds.y = Util::clamp(SM::fogBounds.y - inc, SM::fogBounds.x + 1, MAX_DISTANCE);
    } else {
        // decrease "render" distance
        SM::updateDistance = Util::clamp(SM::updateDistance + inc, SM::fogBounds.x, MAX_DISTANCE);
        SM::fogBounds.y = Util::clamp(SM::fogBounds.y + inc, SM::fogBounds.x + 1, MAX_DISTANCE);
    }
    printf("New fog bounds: "); Util::print(SM::fogBounds);
}

void specKeyPressed(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        if (SM::isFreeCam) {
            SM::camera->DOWN = true;
        } else {
            player->DOWN = true;
        }
    }
    if (key == GLUT_KEY_SHIFT_R) {
        player->DOWN = true;
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
    if (key == GLUT_KEY_SHIFT_R) {
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

    if (key == ',') SM::showNormal = !SM::showNormal;
    if (key == '\\') Util::print(SM::camera->pos);

    if (key == '.') {
        flock->reset();
        // player->pos = vec3(0);
        // SM::camera->setPosition(vec3(0));
    }

    if (key == '`') SM::toggleFreeCam();
    if (key == 'z' || key == 'Z') SM::changeBoidAttackState();
    if (key == 'r' || key == 'R') SM::switchFirstAndThirdCam();
    if (key == 'q' || key == 'Q') SM::changeFlashlightState();
    if (SM::isFreeCam) {
        if (key == ' ') SM::camera->UP = true;
        if (key == 'w' || key == 'W') SM::camera->FORWARD = true;
        if (key == 's' || key == 'S') SM::camera->BACK = true;
        if (key == 'a' || key == 'A') SM::camera->LEFT = true;
        if (key == 'd' || key == 'D') SM::camera->RIGHT = true;
        if (key == 'e' || key == 'E') SM::camera->SPRINT = true;
    } else {
        if (key == ' ') player->UP = true;
        if (key == 'w' || key == 'W') player->FORWARD = true;
        if (key == 's' || key == 'S') player->BACK = true;
        if (key == 'a' || key == 'A') player->LEFT = true;
        if (key == 'd' || key == 'D') player->RIGHT = true;
        if (key == 'e' || key == 'E') player->SPRINT = true;
    }

    // Move player regardless of user (for demonstration)
    if (key == 'm' || key == 'M') player->UP = true;
    if (key == 'i' || key == 'I') player->FORWARD = true;
    if (key == 'k' || key == 'K') player->BACK = true;
    if (key == 'j' || key == 'J') player->LEFT = true;
    if (key == 'l' || key == 'L') player->RIGHT = true;
    if (key == 'p' || key == 'P') SM::camera->SPRINT = true;
}

// Function ran on key release
void keyReleased(unsigned char key, int x, int y) {
    if (SM::isFreeCam) {
        if (key == ' ') SM::camera->UP = false;
        if (key == 'w' || key == 'W') SM::camera->FORWARD = false;
        if (key == 's' || key == 'S') SM::camera->BACK = false;
        if (key == 'a' || key == 'A') SM::camera->LEFT = false;
        if (key == 'd' || key == 'D') SM::camera->RIGHT = false;
        if (key == 'e' || key == 'E') SM::camera->SPRINT = false;
        if (key == 'p' || key == 'P') SM::camera->CAN_FLY = !SM::camera->CAN_FLY;
    } else {
        if (key == ' ') player->UP = false;
        if (key == 'w' || key == 'W') player->FORWARD = false;
        if (key == 's' || key == 'S') player->BACK = false;
        if (key == 'a' || key == 'A') player->LEFT = false;
        if (key == 'd' || key == 'D') player->RIGHT = false;
        if (key == 'e' || key == 'E') player->SPRINT = false;
        if (key == 'p' || key == 'P') player->CAN_FLY = !player->CAN_FLY;
    }

    // Move player regardless of user (for demonstration)
    if (key == 'm' || key == 'M') player->UP = false;
    if (key == 'i' || key == 'I') player->FORWARD = false;
    if (key == 'k' || key == 'K') player->BACK = false;
    if (key == 'j' || key == 'J') player->LEFT = false;
    if (key == 'p' || key == 'P') player->RIGHT = false;
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
    glutMouseWheelFunc(passiveMouseWheel);
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
