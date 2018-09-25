#pragma once

#include <atlas/utils/Geometry.hpp>
#include <atlas/gl/Buffer.hpp>
#include <atlas/gl/VertexArrayObject.hpp>
#include <atlas/gl/Texture.hpp>

namespace lab2
{
    class GolfBall : public atlas::utils::Geometry
    {
    public:
        GolfBall();

        void updateGeometry(atlas::core::Time<> const& t) override;
        void drawGui() override;
        void renderGeometry(atlas::math::Matrix4 const& projection,
            atlas::math::Matrix4 const& view) override;

        void resetGeometry() override;

        atlas::math::Vector getTatooniePosition(){
            return mApproxPosition;
        }

        atlas::math::Vector getStar1Position(){
            return mStar1Position;
        }






    private:
        void orbitingPlanet(atlas::core::Time<> const& t);
        void binaryStarMovement(atlas::core::Time<> const& t);


        atlas::gl::Buffer mVertexBuffer;
        atlas::gl::Buffer mIndexBuffer;
        atlas::gl::VertexArrayObject mVao;

        atlas::math::Vector mStar1Position;
        atlas::math::Vector mStar2Position;
        atlas::math::Vector mStar1Velocity;
        atlas::math::Vector mStar2Velocity;
        atlas::math::Vector mApproxPosition;
        atlas::math::Vector mApproxVelocity;
        atlas::math::Vector mForce;
        atlas::math::Vector mForce1;
        atlas::math::Vector mForce2;
        atlas::math::Vector mStarForce;
        atlas::math::Vector mStarForceDirection;
        atlas::math::Vector mForceDirection1;
        atlas::math::Vector mForceDirection2;


        float mPlanetMass, mStar1Mass, mStar2Mass, G;
        int mIntegrator;

        GLsizei mIndexCount;
    };
}