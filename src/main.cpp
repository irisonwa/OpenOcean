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

    Shader* s2 = new Shader("bones", vert_bmesh, frag_bmesh);
    shaders[s2->name] = s2;
#ifdef TREE
    Shader* s3 = new Shader("variant_skinned", vert_vmesh_cpu, frag_vmesh);
    shaders[s3->name] = s3;
#else
    Shader* s3 = new Shader("variant_skinned", vert_vmesh_gpu, frag_vmesh);
    shaders[s3->name] = s3;
#endif
    Shader* s4 = new Shader("variant_static", vert_vmesh_cpu, frag_vmesh);
    shaders[s4->name] = s4;

    /// -------------------------------------------------- LIGHTING -------------------------------------------------- ///
    staticLight = new Lighting("stony light", shaders["variant_static"], MATERIAL_SHINY);
    boneLight = new Lighting("boney light", shaders["bones"], MATERIAL_SHINY);
    variantLight = new Lighting("variant light", shaders["variant_skinned"], MATERIAL_SHINY);

    /// -------------------------------------------------- STATIC MESHES -------------------------------------------------- ///
    float offset = 1.0f;
    float lim = 6;
    int spread = 60;
    for (vec3 p : beaconLightPos) {
        stvMats.push_back(translate(mat4(1), p)); // beacon
    }
    for (vec3 p : anemonePos) {
        stvMats.push_back(translate(scale(mat4(1), vec3(.25)), p * vec3(4))); // anemone
        for (int z = -lim / 2; z < lim / 2; z += 1) {
            for (int y = -lim / 2; y < lim / 2; y += 1) {
                for (int x = -lim / 2; x < lim / 2; x += 1) {
                    if (x == 0 || y == 0 || z == 0) continue;
                    vec3 translation(
                        p.x + (float)x * (rand() % spread),
                        p.y + (float)y * (rand() % (spread / 4)), // minimal height variation
                        p.z + (float)z * (rand() % spread));
                    skvMats.push_back(scale(translate(mat4(1), translation), vec3(.25))); // kelp
                }
            }
        }
    }
    for (const auto &m : islandMats) stvMats.push_back(m); // islands
    stvMats.push_back(translate(mat4(1), vec3(288, 268, 257))); // beach item
    stvMats.push_back(translate(scale(mat4(1), vec3(1.f)), vec3(0, -220, 0))); // terrain
    stvMats.push_back(translate(mat4(1), sunPos)); // sun
    staticVariants = new VariantMesh(
        "v_geom", shaders["variant_static"], VariantType::STATIC,
        {
            {MESH_BEACON, beaconLightPos.size(), -1, -1, std::vector<unsigned>(beaconLightPos.size(), 0)},
            {MESH_ANEMONE, anemonePos.size(), -1, -1, std::vector<unsigned>(anemonePos.size(), 0)},
            {MESH_BEACH, islandMats.size(), -1, -1, std::vector<unsigned>(islandMats.size(), 0)},
            {MESH_BEACH_ITEM, 1, -1, -1, std::vector<unsigned>(1, 0)},
            {MESH_TERRAIN, 1, -1, -1, std::vector<unsigned>(1, 0)},
            {MESH_SUN, 1, -1, -1, std::vector<unsigned>(1, 0)},
        });

    /// -------------------------------------------------- SKINNED MESHES -------------------------------------------------- ///
    BoneMesh* bm1 = new BoneMesh("kelp", MESH_KELP_ANIM, shaders["bones"]);
    bmeshes[bm1->name] = bm1;

    /// -------------------------------------------------- PLAYER -------------------------------------------------- ///
    player = new Player("Player", vec3(288.050171, 271.612457, 257.632996), Util::FORWARD);
    player->setMesh(MESH_PLAYER_ANIM, -1, -1);
    player->setShader(shaders["bones"]);

    /// -------------------------------------------------- BOIDS -------------------------------------------------- ///
    flockVariants = new VariantMesh(
        "v_flock", shaders["variant_skinned"], VariantType::SKINNED,
        {
            {MESH_BLUE_SHARK_ANIM   , 32,   -1,   -1, std::vector<unsigned int>(32, 0)},
            {MESH_WHITE_SHARK_ANIM  , 32,   -1,   -1, std::vector<unsigned int>(32, 0)},
            {MESH_WHALE_SHARK_ANIM  , 32,   -1,   -1, std::vector<unsigned int>(32, 0)},
            {MESH_WHALE_ANIM        , 4,    -1,   -1, std::vector<unsigned int>(4, 0)},
            {MESH_MARLIN_ANIM       , 500,  1024,  2, std::vector<unsigned int>(500, 0)},  // black marlin
            {MESH_MARLIN_ANIM       , 500,  1024,  2, std::vector<unsigned int>(500, 1)},  // blue marlin
            {MESH_SPEARFISH_ANIM    , 500,  -1,   -1, std::vector<unsigned int>(500, 0)},
            {MESH_DOLPHIN_ANIM      , 100,  -1,   -1, std::vector<unsigned int>(100, 0)},
            {MESH_CLOWNFISH_ANIM    , 2300, -1,   -1, std::vector<unsigned int>(2300, 0)},
            {MESH_HERRING_ANIM      , 2500, -1,   -1, std::vector<unsigned int>(2500, 0)},
            {MESH_PLANKTON_ANIM     , 1000, -1,   -1, std::vector<unsigned int>(1000, 0)},
            {MESH_THREADFIN_ANIM    , 2500, -1,   -1, std::vector<unsigned int>(2500, 0)},
        });
    flock = new Flock(flockVariants, anemonePos);

    /// -------------------------------------------------- LIGHTS -------------------------------------------------- ///
    float beaconAttLin = 0.00000009;
    float beaconAttQuad = 0.000000009;
    for (vec3 p : beaconLightPos) {
        vec3 ofst = vec3(0, 2.f, 0);
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
    updateBox = new Box(vec3(-20), vec3(20));
    updateBox->loadWireframe();
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable colour blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    vec4 newBg = SM::bgColour * Util::mapRange(SM::camera->pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH, useHeightBackground ? 0.2 : 0.99999f, 1);
    SM::seaLevel = SM::isFreeCam ? -1000 : 1000;
    glClearColor(newBg.x, newBg.y, newBg.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = SM::camera->getViewMatrix();
    mat4 persp_proj = SM::camera->getProjectionMatrix();

    /// ------------------------------------------------ STATIC MESHES ------------------------------------------------ ///
    // update camera view and flashlight
    staticLight->setLightAtt(view, persp_proj, SM::camera->pos);
    staticLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    staticLight->use();

    if (showGround) {
        staticVariants->render(stvMats.data());
    }

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
    }

    if (showGround) {
        bmeshes["kelp"]->update(100);
        bmeshes["kelp"]->render(skvMats.size(), skvMats.data());
    }

    if (showLevelBounds) {
        SM::sceneBox->transform = scale(vec3(flock->levelDistance * 4 /* ?? */));
        SM::sceneBox->drawWireframe();
    }
    if (showUpdateBounds) {
        vec3 pp = vec3(player->transform[3]);
        updateBox->transform = scale(translate(mat4(1), pp), vec3(SM::updateDistance * 2));
        updateBox->drawWireframe();
    }

    /// ------------------------------------------------ VARIANT MESHES ------------------------------------------------ ///
    variantLight->setLightAtt(view, persp_proj, SM::camera->pos);
    variantLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    variantLight->use();
    flock->process(player->pos, SM::updateDistance);
    if (showBoids) flock->show();

    /// ------------------------------------------------ DEBUG MENU ------------------------------------------------ ///
    // Handle ImGui window
    if (SM::debug) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = true;
        ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Checkbox("Show Fish", &showBoids);
        ImGui::Checkbox("Show Ground", &showGround);
        ImGui::Checkbox("Show Level Bounds", &showLevelBounds);
        ImGui::SliderFloat("Level Distance", &flock->levelDistance, 1, 300);
        ImGui::SameLine();
        if (ImGui::Button("Reset##Level")) {
            flock->levelDistance = WORLD_BOUND_HIGH;
        }
        ImGui::Checkbox("Change background colour from height", &useHeightBackground);
        ImGui::Checkbox("Enable Attacking", &SM::canBoidsAttack);
        ImGui::SliderFloat("Speed Factor", &flock->speedFactor, 0.1f, 10.f);
        ImGui::SameLine();
        if (ImGui::Button("Reset##Speed")) {
            flock->speedFactor = 1;
        }
        ImGui::Text("\nThe Update Distance value controls the distance at \nwhich boids are updated, from a radius surrounding the player. "
                    "\nIt's actually bound in a sphere, but is displayed as a cube.");
        ImGui::Checkbox("Show Update Bounds", &showUpdateBounds);
        ImGui::SliderFloat("Update Distance", &SM::updateDistance, 1.f, 2048.f);
        ImGui::SameLine();
        if (ImGui::Button("Reset##Update")) {
            SM::updateDistance = 100;
        }
        SM::fogBounds.y = SM::updateDistance; // update fog bounds too
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        if (ImGui::Button("Centre Player")) {
            player->pos = vec3(0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Centre Boids")) {
            flock->reset();
        }
        ImGui::End();
        glUseProgram(0); // ?
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glutSwapBuffers();
}

void updateScene() {
    SM::updateDelta();
    player->processMovement();
    SM::camera->processMovement();
    flashlightCoords = SM::flashlightToggled ? (SM::isFreeCam ? SM::camera->pos : player->transform[3]) : vec3(-10000);
    flashlightDir = SM::flashlightToggled ? SM::camera->front : vec3(0, -1, 0);
    // Draw the next frame
    glutPostRedisplay();
}

void mouseDragged(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
}

// Process the mouse moving without button input
void passiveMouseMoved(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    SM::updateMouse(x, y);
    if (!SM::debug) {
        glutWarpPointer(SM::width / 2, SM::height / 2);
        SM::camera->processView();
    }
}

void passiveMouseWheel(int button, int dir, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(x, y);
    if (!SM::debug) {
        const float MAX_DISTANCE = 2048;
        const float inc = 5;
        if (dir > 0) {
            // increase "render" distance
            SM::updateDistance = Util::clamp(SM::updateDistance - inc, SM::fogBounds.x, MAX_DISTANCE);
            SM::fogBounds.y = SM::updateDistance;
        } else {
            // decrease "render" distance
            SM::updateDistance = Util::clamp(SM::updateDistance + inc, SM::fogBounds.x, MAX_DISTANCE);
            SM::fogBounds.y = SM::updateDistance;
        }
        printf("New fog bounds: "); Util::print(SM::fogBounds);
    }
}

void mousePressed(int button, int state, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, state == GLUT_DOWN);
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
    if (key == VK_TAB) {
        SM::debug = !SM::debug;
        if (SM::debug && !SM::isFreeCam) SM::toggleFreeCam(); // enable free cam when in debug mode
        if (!SM::debug) glutWarpPointer(SM::width / 2, SM::height / 2); // warp immediately to prevent sudden jerk
    }

    // debug
    if (key == ',') SM::showNormal = !SM::showNormal;
    if (key == '\\') Util::print(SM::camera->pos);

    if (key == 'h' || key == 'H') showGround = !showGround;
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
    if (key == 'l' || key == 'L') player->RIGHT = false;
    if (key == 'p' || key == 'P') SM::camera->SPRINT = false;
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
    glutMotionFunc(mouseDragged);
    glutPassiveMotionFunc(passiveMouseMoved);
    glutMouseWheelFunc(passiveMouseWheel);
    glutMouseFunc(mousePressed);
    glutSetCursor(GLUT_CURSOR_NONE);  // hide cursor

    // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
    // Check for any errors
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(SM::width, SM::height);
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 460");

    // Set up your objects and shaders
    init();
    // Begin infinite event loop
    glutMainLoop();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
