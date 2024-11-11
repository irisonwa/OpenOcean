#pragma warning(disable : 5208)

#include "main.h"
using namespace std;

void init() {
    // enable debug callback
    // glEnable(GL_DEBUG_OUTPUT);
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
    // StaticMesh* m5 = new StaticMesh("cube", TEST_SPEC, 1024, 4);
    // smeshes[m5->name] = m5;
    StaticMesh* m6 = new StaticMesh("kelp", MESH_KELP, -1, -1);
    smeshes[m6->name] = m6;
    StaticMesh* m7 = new StaticMesh("shark", MESH_SHARK, 1024, 1);
    smeshes[m7->name] = m7;
    StaticMesh* m8 = new StaticMesh("island", MESH_ISLAND);
    smeshes[m8->name] = m8;

    /// -------------------------------------------------- SKINNED MESHES -------------------------------------------------- ///
    BoneMesh* bm1 = new BoneMesh("test kelp", MESH_KELP_ANIM, shaders["bones"]);
    bmeshes[bm1->name] = bm1;
    // BoneMesh* bm2 = new BoneMesh("test guy", MESH_GUY_ANIM, shaders["bones"]);
    // bmeshes[bm2->name] = bm2;
    BoneMesh* bm3 = new BoneMesh("test shark", MESH_THREADFIN_ANIM, shaders["bones"], 1024, 1);
    bmeshes[bm3->name] = bm3;

    /// -------------------------------------------------- PLAYER -------------------------------------------------- ///
    player = new Player("Player", vec3(0, 10, 0), Util::FORWARD);
    player->setMesh(MESH_PLAYER_ANIM, 2048, 1);
    player->setShader(shaders["bones"]);

    /// -------------------------------------------------- BOIDS -------------------------------------------------- ///
    vMesh = new VariantMesh(
        "vmesh", shaders["variant"],
        {
            {MESH_MARLIN, 300, 1024, 2, std::vector<unsigned int>(300, 0)},  // black marlin
            {MESH_MARLIN, 300, 1024, 2, std::vector<unsigned int>(300, 1)},  // blue marlin
            {MESH_THREADFIN, 300, 1024, 1, std::vector<unsigned int>(300, 1)},
            {MESH_SPEARFISH, 300, 1024, 1, std::vector<unsigned int>(300, 1)},
            {MESH_SHARK2, 10, 1024, 1, std::vector<unsigned int>(2, 1)},
            {MESH_SHARK2, 10, 1024, 4, std::vector<unsigned int>(3, 2)},
            // {TEST_SPEC, 1, 1024, 4, std::vector<unsigned int>(1, 3)},
            // {TEST_BOID, 3, 1024, 4, std::vector<unsigned int>(3, 2)},
            // {MESH_KELP, 3, 1024, 1, std::vector<unsigned int>(3, 3)},
            // {MESH_SUB, 2, 2048, 1, std::vector<unsigned int>(2, 3)},
            // {TEST_BOID, 100, 1024, 4, std::vector<unsigned int>(100, 1)},
            // {MESH_SUB, 1, 2048, 1, std::vector<unsigned int>(1, 3)},
        });
    flock = new Flock(vMesh);

    // Create skybox
    cubemap = new Cubemap();
    cubemap->loadCubemap(cubemap_faces);

    // Set start time of program near when init() finishes loading
    SM::startTime = timeGetTime();

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
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    staticLight->addSpotLightAtt(vec3(0, 300, 0), vec3(0, -1, 0), vec3(.3), vec3(1), vec3(1));
    staticLight->spotLights[staticLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    // staticLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));
    // staticLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    // staticLight->setDirLightColour(vec3(.05), vec3(.05), vec3(1));
    // staticLight->spotLights[staticLight->nSpotLights - 1].linear = 0.9;
    // staticLight->spotLights[staticLight->nSpotLights - 1].quadratic = 0.3;

    boneLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    boneLight->spotLights[boneLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    boneLight->addSpotLightAtt(vec3(0, 300, 0), vec3(0, -1, 0), vec3(.3), vec3(1), vec3(1));
    boneLight->spotLights[boneLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    boneLight->spotLights[boneLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    // boneLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));
    // boneLight->setDirLightsAtt(vector<vec3>{vec3(0, -1, 0)});
    // boneLight->setDirLightColour(vec3(.2), vec3(.2), vec3(.2));
    // boneLight->addPointLightAtt(vec3(0), vec3(0.2f), vec3(1), vec3(1));

    variantLight->addSpotLightAtt(flashlightCoords, flashlightDir, vec3(0.2f), vec3(1), vec3(1));
    variantLight->spotLights[variantLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].linear = /* 0.022; */ flashAttLin;
    variantLight->spotLights[variantLight->nSpotLights - 1].quadratic = /* 0.0019; */ flashAttQuad;
    variantLight->addSpotLightAtt(vec3(0, 300, 0), vec3(0, -1, 0), vec3(.3), vec3(1), vec3(1));
    variantLight->spotLights[variantLight->nSpotLights - 1].linear = /* 0.022; */ sunlightLin;
    variantLight->spotLights[variantLight->nSpotLights - 1].quadratic = /* 0.0019; */ sunlightQuad;
    variantLight->spotLights[variantLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    variantLight->spotLights[variantLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
}

void display() {
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);  // enable depth-testing
    glEnable(GL_BLEND);       // enable colour blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);  // depth-testing interprets a smaller value as "closer"
    vec4 newBg = SM::bgColour * Util::mapRange(player->pos.y, SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH, 0, 1);
    glClearColor(newBg.x, newBg.y, newBg.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.getViewMatrix();
    mat4 persp_proj = camera.getPerspectiveProjection();

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
    staticLight->setLightAtt(view, persp_proj, camera.pos);
    staticLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    // staticLight->spotLights[staticLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    // staticLight->spotLights[staticLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    staticLight->use();

    // smeshes["kelp"]->render(mat4(1));
    smeshes["island"]->render(scale(translate(mat4(1), vec3(0, 0, 0)), vec3(25)), 1);

    /// ---------------- SKINNED MESHES ---------------- ///
    boneLight->setLightAtt(view, persp_proj, camera.pos);
    boneLight->setSpotLightAtt(0, flashlightCoords, flashlightDir, vec3(0.2f), vec3(1, .6, .2), vec3(1));
    // boneLight->spotLights[boneLight->nSpotLights - 1].cutOff = cos(Util::deg2Rad(24.f));
    // boneLight->spotLights[boneLight->nSpotLights - 1].outerCutOff = cos(Util::deg2Rad(35.f));
    boneLight->shader->setBool("showNormal", SM::showNormal);
    boneLight->use();

    bmeshes["test kelp"]->update(100);
    bmeshes["test kelp"]->render(transm.size(), transm.data());

    // bmeshes["test shark"]->update();
    // bmeshes["test shark"]->render(numInstances, models, depths.data());

    if (SM::isFirstPerson) {
        player->lookAt(camera.front);
        camera.followTarget(player);
    } else if (SM::isThirdPerson) {
        player->render();
        player->lookAt(camera.front);
        camera.followTarget(player);
    } else /* isFreeCam */ {
        player->render();
    }

    /// ---------------- VARIANT MESHES ---------------- ///
    variantLight->setLightAtt(view, persp_proj, camera.pos);
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
        player->processMovement(camera);
    } else {
        camera.processMovement();
    }
    flashlightCoords = SM::flashlightToggled ? (SM::isFreeCam ? camera.pos : player->transform[3]) : vec3(-10000);
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
        if (!SM::isFreeCam) {
            player->DOWN = true;
        } else {
            camera.DOWN = true;
        }
    }
}

void specKeyReleased(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L) {
        if (!SM::isFreeCam) {
            player->DOWN = false;
        } else {
            camera.DOWN = false;
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
        // camera.setPosition(vec3(0));
    }

    if (key == 'r' || key == 'R') SM::changeCameraState();
    if (!SM::isFreeCam) {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = true;
        if (key == ' ') player->UP = true;
        if (key == 'w' || key == 'W') player->FORWARD = true;
        if (key == 's' || key == 'S') player->BACK = true;
        if (key == 'a' || key == 'A') player->LEFT = true;
        if (key == 'd' || key == 'D') player->RIGHT = true;
        if (key == 'e' || key == 'E') player->SPRINT = true;
    } else {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = true;
        if (key == ' ') camera.UP = true;
        if (key == 'w' || key == 'W') camera.FORWARD = true;
        if (key == 's' || key == 'S') camera.BACK = true;
        if (key == 'a' || key == 'A') camera.LEFT = true;
        if (key == 'd' || key == 'D') camera.RIGHT = true;
        if (key == 'e' || key == 'E') camera.SPRINT = true;
    }
}

// Function ran on key release
void keyReleased(unsigned char key, int x, int y) {
    if (!SM::isFreeCam) {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = false;
        if (key == ' ') player->UP = false;
        if (key == 'w' || key == 'W') player->FORWARD = false;
        if (key == 's' || key == 'S') player->BACK = false;
        if (key == 'a' || key == 'A') player->LEFT = false;
        if (key == 'd' || key == 'D') player->RIGHT = false;
        if (key == 'e' || key == 'E') player->SPRINT = false;
        if (key == 'p' || key == 'P') player->CAN_FLY = !player->CAN_FLY;
    } else {
        if (key == 'q' || key == 'Q') SM::flashlightToggled = false;
        if (key == ' ') camera.UP = false;
        if (key == 'w' || key == 'W') camera.FORWARD = false;
        if (key == 's' || key == 'S') camera.BACK = false;
        if (key == 'a' || key == 'A') camera.LEFT = false;
        if (key == 'd' || key == 'D') camera.RIGHT = false;
        if (key == 'e' || key == 'E') camera.SPRINT = false;
        if (key == 'p' || key == 'P') camera.CAN_FLY = !camera.CAN_FLY;
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
