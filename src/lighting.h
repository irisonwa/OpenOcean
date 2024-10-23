#ifndef LIGHTING_H
#define LIGHTING_H
#include "shader.h"
#include "util.h"

enum materialPresets {
    /// <summary>
    /// A heavily non-specular material. shininess = 1.0f
    /// </summary>
    MATERIAL_RUBBER = 0x1,

    /// <summary>
    /// A rough non-specular material. shininess = 5.0f
    /// </summary>
    MATERIAL_STONE = 0x2,

    /// <summary>
    /// A flat, lower-specular texture. shininess = 20.0f
    /// </summary>
    MATERIAL_WOOD = 0x3,

    /// <summary>
    /// A moderately specular material. shininess = 50.0f
    /// </summary>
    MATERIAL_PLASTIC = 0x4,

    /// <summary>
    /// A metallic, highly specular material. shininess = 100.0f
    /// </summary>
    MATERIAL_METAL = 0x5,

    /// <summary>
    /// An extremely reflective material. shininess = 850.0f
    /// </summary>
    MATERIAL_SHINY = 0x6
};

class Lighting {
   public:
    struct Material {
        int diffuse;
        int specular;
        float shininess;
    };

    struct DirLight {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    struct PointLight {
        vec3 position;

        float constant = 1.f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    struct SpotLight {
        vec3 position;
        vec3 direction;
        float cutOff = cos(Util::deg2Rad(2.5f));
        float outerCutOff = cos(Util::deg2Rad(5.f));

        float constant = 1.f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    std::string name;
    Shader* shader;
    materialPresets material;
    int nPointLights = 0;
    int nSpotLights = 0;
    int nDirLights = 0;

    DirLight dirLights[100];
    PointLight pointLights[100];
    SpotLight spotLights[100];

    Lighting() {}
    Lighting(std::string nm, Shader* s, materialPresets mp) {
        name = nm;
        shader = s;
        material = mp;
    }

    /// <summary>
    /// Set the view matrix, projection matrix, and view position of the light
    /// </summary>
    /// <param name="view">View matrix</param>
    /// <param name="projection">Projection matrix</param>
    /// <param name="vPos">View position</param>
    void setLightAtt(mat4 view, mat4 projection, vec3 vPos);

    /// <summary>
    /// Set the positions and directions of the spot lights
    /// </summary>
    /// <param name="pos">List of spot light positions</param>
    /// <param name="dirs">List of spot light directions</param>
    void setSpotLightsAtt(std::vector<vec3> pos, std::vector<vec3> dirs);

    /// <summary>
    /// Add positions and directions of the spot lights
    /// </summary>
    /// <param name="pos">List of spot light positions</param>
    /// <param name="dirs">List of spot light directions</param>
    void addSpotLightsAtt(std::vector<vec3> pos, std::vector<vec3> dirs);

    /// <summary>
    /// Add a single spot light
    /// </summary>
    /// <param name="pos">The spot light position</param>
    /// <param name="dir">The spot light direction</param>
    /// <param name="amb">The ambient light colour</param>
    /// <param name="dif">The diffuse light colour</param>
    /// <param name="spe">The specular light colour</param>
    void addSpotLightAtt(vec3 pos, vec3 dir, vec3 amb, vec3 dif, vec3 spec);

    /// <summary>
    /// Set the positions of the point lights
    /// </summary>
    /// <param name="pos"></param>
    void setPointLightsAtt(std::vector<vec3> pos);

    /// <summary>
    /// Add a single point light
    /// </summary>
    /// <param name="pos">The point light position</param>
    /// <param name="amb">The ambient light colour</param>
    /// <param name="dif">The diffuse light colour</param>
    /// <param name="spe">The specular light colour</param>
    void addPointLightAtt(vec3 pos, vec3 amb, vec3 dif, vec3 spec);

    /// <summary>
    /// Set the directions of the directional lights
    /// </summary>
    /// <param name="dir">The directional light directions</param>
    void setDirLightsAtt(std::vector<vec3> dir);

    /// <summary>
    /// Set the ambient, diffuse, and specular colours for the light's spotlights.
    /// </summary>
    /// <param name="amb">The ambient light colours</param>
    /// <param name="dif">The diffuse light colours</param>
    /// <param name="spe">The specular light colours</param>
    void setSpotLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe);

    /// <summary>
    /// Set the ambient, diffuse, and specular colours for the light's point lights.
    /// </summary>
    /// <param name="amb">The ambient light colours</param>
    /// <param name="dif">The diffuse light colours</param>
    /// <param name="spe">The specular light colours</param>
    void setPointLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe);

    /// <summary>
    /// Set the ambient, diffuse, and specular colours for the light's directional lights.
    /// </summary>
    /// <param name="amb">The ambient light colours</param>
    /// <param name="dif">The diffuse light colours</param>
    /// <param name="spe">The specular light colours</param>
    void setDirLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe);

    /// <summary>
    /// set the dir light
    /// </summary>
    /// <param name="amb">The ambient light colour</param>
    /// <param name="dif">The diffuse light colour</param>
    /// <param name="spe">The specular light colour</param>
    void setDirLightColour(vec3 amb, vec3 dif, vec3 spe);
    void use();
    void loadMaterial();
    void loadStoneMaterial();
    void loadPlasticMaterial();
    void loadWoodMaterial();
    void loadRubberMaterial();
    void loadMetalMaterial();
    void loadShinyMaterial();
};

#endif /* LIGHTING_H */
