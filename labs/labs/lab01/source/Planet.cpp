#include "Planet.hpp"
#include "Paths.hpp"
#include "LayoutLocations.glsl"

#include <atlas/utils/Mesh.hpp>
#include <atlas/core/STB.hpp>
#include <atlas/math/Coordinates.hpp>
#include <atlas/core/Float.hpp>
#include <atlas/utils/GUI.hpp>

namespace lab5
{
    Planet::Planet(std::string const& textureFile) :
        mVertexBuffer(GL_ARRAY_BUFFER),
        mIndexBuffer(GL_ELEMENT_ARRAY_BUFFER),
        mTexture(GL_TEXTURE_2D),
        acc1(0.0f, 0.0f, 0.0f),
        acc2(0.0f, 0.0f, 0.0f)
    {
        using atlas::utils::Mesh;
        namespace gl = atlas::gl;
        namespace math = atlas::math;

        Mesh sphere;
        std::string path{ DataDirectory };
        path = path + "sphere.obj";
        Mesh::fromFile(path, sphere);

        mIndexCount = static_cast<GLsizei>(sphere.indices().size());

        std::vector<float> data;
        for (std::size_t i = 0; i < sphere.vertices().size(); ++i)
        {
            data.push_back(sphere.vertices()[i].x);
            data.push_back(sphere.vertices()[i].y);
            data.push_back(sphere.vertices()[i].z);

            data.push_back(sphere.normals()[i].x);
            data.push_back(sphere.normals()[i].y);
            data.push_back(sphere.normals()[i].z);

            data.push_back(sphere.texCoords()[i].x);
            data.push_back(sphere.texCoords()[i].y);
        }

        mVao.bindVertexArray();
        mVertexBuffer.bindBuffer();
        mVertexBuffer.bufferData(gl::size<float>(data.size()), data.data(),
            GL_STATIC_DRAW);
        mVertexBuffer.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(0));
        mVertexBuffer.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(3));
        mVertexBuffer.vertexAttribPointer(TEXTURES_LAYOUT_LOCATION, 2, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(6));

        mVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
        mVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);
        mVao.enableVertexAttribArray(TEXTURES_LAYOUT_LOCATION);

        mIndexBuffer.bindBuffer();
        mIndexBuffer.bufferData(gl::size<GLuint>(sphere.indices().size()),
            sphere.indices().data(), GL_STATIC_DRAW);

        mIndexBuffer.unBindBuffer();
        mVertexBuffer.unBindBuffer();
        mVao.unBindVertexArray();

        int width, height, nrChannels;
        std::string imagePath = std::string(DataDirectory) + textureFile;
        unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height,
            &nrChannels, 0);

        mTexture.bindTexture();
        mTexture.texImage2D(0, GL_RGB, width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, imageData);
        mTexture.texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
        mTexture.texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
        mTexture.texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mTexture.texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(imageData);

        std::vector<gl::ShaderUnit> shaders
        {
            {std::string(ShaderDirectory) + "Planet.vs.glsl", GL_VERTEX_SHADER},
            {std::string(ShaderDirectory) + "Planet.fs.glsl", GL_FRAGMENT_SHADER}
        };

        mShaders.emplace_back(shaders);
        mShaders[0].setShaderIncludeDir(ShaderDirectory);
        mShaders[0].compileShaders();
        mShaders[0].linkShaders();

        auto var = mShaders[0].getUniformVariable("model");
        mUniforms.insert(UniformKey("model", var));
        var = mShaders[0].getUniformVariable("projection");
        mUniforms.insert(UniformKey("projection", var));
        var = mShaders[0].getUniformVariable("view");
        mUniforms.insert(UniformKey("view", var));

        mShaders[0].disableShaders();
        mModel = glm::scale(math::Matrix4(1.0f), math::Vector(4.0f));

        mVelocity = atlas::math::Vector(glm::sqrt(mG * mCentralMass) / mOrbit);
    }

    void Planet::setPosition(atlas::math::Point const& pos)
    {
        using atlas::math::Matrix4;
        using atlas::math::cartesianToPolar;

        p1.xz = cartesianToPolar(pos.xz);
        mModel = glm::translate(Matrix4(1.0f), pos);
    }


    void Planet::setVelocity(atlas::math::Point const &pos)
    {
        using atlas::math::Matrix4;
        using atlas::math::cartesianToPolar;

        mVelocity.xz = cartesianToPolar(pos.xz);

    }

    void Planet::updateGeometry(atlas::core::Time<> const& t)
    {
        using atlas::math::Matrix4;
        using atlas::math::Vector;
        using atlas::math::polarToCartesian;


        Vector acc;

        acc.x =  mPosition.x * mVelocity.z * mVelocity.z  - (mG * 10 * mCentralMass) / (mPosition.x * mPosition.x);
        acc.z = -2.0  * mVelocity.x * mVelocity.z /  mPosition.x;

        mVelocity += t.deltaTime * acc;
        mPosition += t.deltaTime * mVelocity;

        /*

        acc1.z = (g*(sin(p2.z) * cos(p2.z - p1.z) - mu * sin(p1.z)) -
                (l2 * v2.z * v2.z + l1 * v1.z * v1.z * cos(p1.z - p2.z))*sin(p1.z - p2.z))/
                (l1 * (mu - cos(p1.z - p2.z) * cos(p1.z - p2.z)));

        acc2.z = (mu*g*(sin(p1.z) * cos(p1.z - p2.z) - sin(p2.z)) +
                  mu * l1 * v1.z * v1.z + l2 * v2.z * v2.z * cos(p1.z - p2.z))/
                 (l2 * (mu - cos(p1.z - p2.z) * cos(p1.z - p2.z)));

                 */
        acc1.x = p1.x * v1.z * v1.z - (mG * mCentralMass)/(p1.x * p1.x);
        acc1.z = - 2.0 * v1.x * v1.z / p1.x;

        v1 += t.deltaTime * acc1;
        p1 += t.deltaTime * v1;

        v2.z += t.deltaTime * acc2.z;
        p2.z += t.deltaTime * v2.z;


        pos2.xz = polarToCartesian(p2.xz);
        pos1.xz = polarToCartesian(mPosition.xz);


        // Todo by students
    }

    
    void Planet::renderGeometry(atlas::math::Matrix4 const& projection,
        atlas::math::Matrix4 const& view)
    {
        namespace math = atlas::math;

        mShaders[0].hotReloadShaders();
        if (!mShaders[0].shaderProgramValid())
        {
            return;
        }

        mShaders[0].enableShaders();

        mTexture.bindTexture();
        mVao.bindVertexArray();
        mIndexBuffer.bindBuffer();

        //auto model = mModel;

        glUniformMatrix4fv(mUniforms["projection"], 1, GL_FALSE,
            &projection[0][0]);
        glUniformMatrix4fv(mUniforms["view"], 1, GL_FALSE, &view[0][0]);

        {
            auto model = glm::translate(math::Matrix4(1.0f), pos1);
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &mModel[0][0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
        }

        {
            auto model = glm::translate(math::Matrix4(1.0f), pos2);
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &mModel[0][0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
        }

        mIndexBuffer.unBindBuffer();
        mVao.unBindVertexArray();
        mTexture.unBindTexture();
        mShaders[0].disableShaders();
    }

    void Planet::resetGeometry()
    {
        p1 = {14.14, 0.0f, 1.0f};
        p2 = {28.28, 0.0f, 1.0f};
        v1 = {0.0f , 0.0f, 0.0f};
        v2 = {0.0f , 0.0f, 0.0f};
        //mVelocity = atlas::math::Vector(glm::sqrt(mG * mCentralMass) / mOrbit);
    }

    void Planet::drawGui()
    {
        ImGui::SetNextWindowSize(ImVec2(300, 140), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Configurations");

        // std::vector<const char*> integratorNames = { "Euler Intergrator",
        // "Implicit Euler Integrator", "Verlet Integrator" };
        // ImGui::Combo("Integrator", &mIntegrator, integratorNames.data(),
        //     ((int)integratorNames.size()));
        ImGui::InputFloat("mPosition.x", &mPosition.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("mPosition.z", &mPosition.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("pos1.x", &pos1.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("pos1.z", &pos1.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("acc1.z", &acc1.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("acc2.z", &acc2.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("p1.z", &p1.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("p2.z", &p2.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("v1.z", &v1.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("v2.z", &v2.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("p1.x", &p1.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("p2.x", &p2.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("v1.x", &v1.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("v2.x", &v2.x, 1.0f, 5.0f, 1);



        ImGui::End();
    }
}