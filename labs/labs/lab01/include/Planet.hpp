#pragma once

#include <atlas/utils/Geometry.hpp>
#include <atlas/gl/Buffer.hpp>
#include <atlas/gl/VertexArrayObject.hpp>
#include <atlas/gl/Texture.hpp>

namespace lab5
{
    class Planet : public atlas::utils::Geometry
    {
    public:
        Planet(std::string const& textureFile);

        void setPosition(atlas::math::Point const& pos);
        void setVelocity(atlas::math::Point const& pos);

        void updateGeometry(atlas::core::Time<> const& t) override;
        void renderGeometry(atlas::math::Matrix4 const& projection,
            atlas::math::Matrix4 const& view) override;

        void resetGeometry() override;
        void drawGui() override;

    private:

        atlas::gl::Buffer mVertexBuffer;
        atlas::gl::Buffer mIndexBuffer;
        atlas::gl::VertexArrayObject mVao;
        atlas::gl::Texture mTexture;

        atlas::math::Point mPosition;
        atlas::math::Point p1;
        atlas::math::Point p2;
        atlas::math::Vector v1;
        atlas::math::Vector v2;
        atlas::math::Vector acc1;
        atlas::math::Vector acc2;
        atlas::math::Point pos1;
        atlas::math::Point pos2;

        atlas::math::Vector mVelocity;

        GLsizei mIndexCount;
        const float mG = 6.67384e-11f;
        const float mCentralMass = 1.0e15f;
        const float m1 = 10.0;
        const float m2 = 1.0;
        const float l1 = 10.0;
        const float l2 = 10.0;
        const float mu = 1 + m1/m2;
        const float mOrbit = 25.0f;
        const float g = 9.8f;
        atlas::math::Point2 twodpoint;
        float radius;
        float theta;
        float rvelocity;
    };
}