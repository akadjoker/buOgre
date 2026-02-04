#include "bindings.hpp"


// ============== PROCEDURAL MESH GENERATION ==============

namespace ProceduralMesh
{
   

    // ========== CREATE CUBE ==========

    int createCube(   Ogre::ManualObject *manual, const char* material, float size, const char* group )
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group!=NULL)
        {
            groupName= group;

        }
    
        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST,groupName);

        float s = size / 2.0f;

        // Define 8 vertices of cube
        Ogre::Vector3 vertices[8] = {
            Ogre::Vector3(-s, -s, -s), // 0
            Ogre::Vector3( s, -s, -s), // 1
            Ogre::Vector3( s,  s, -s), // 2
            Ogre::Vector3(-s,  s, -s), // 3
            Ogre::Vector3(-s, -s,  s), // 4
            Ogre::Vector3( s, -s,  s), // 5
            Ogre::Vector3( s,  s,  s), // 6
            Ogre::Vector3(-s,  s,  s)  // 7
        };

        // Front face
        manual->position(vertices[0]); manual->normal(0, 0, -1); manual->textureCoord(0, 1);
        manual->position(vertices[1]); manual->normal(0, 0, -1); manual->textureCoord(1, 1);
        manual->position(vertices[2]); manual->normal(0, 0, -1); manual->textureCoord(1, 0);
        manual->position(vertices[3]); manual->normal(0, 0, -1); manual->textureCoord(0, 0);

        // Back face
        manual->position(vertices[5]); manual->normal(0, 0, 1); manual->textureCoord(0, 1);
        manual->position(vertices[4]); manual->normal(0, 0, 1); manual->textureCoord(1, 1);
        manual->position(vertices[7]); manual->normal(0, 0, 1); manual->textureCoord(1, 0);
        manual->position(vertices[6]); manual->normal(0, 0, 1); manual->textureCoord(0, 0);

        // Top face
        manual->position(vertices[3]); manual->normal(0, 1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[2]); manual->normal(0, 1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[6]); manual->normal(0, 1, 0); manual->textureCoord(1, 0);
        manual->position(vertices[7]); manual->normal(0, 1, 0); manual->textureCoord(0, 0);

        // Bottom face
        manual->position(vertices[4]); manual->normal(0, -1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[5]); manual->normal(0, -1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[1]); manual->normal(0, -1, 0); manual->textureCoord(1, 0);
        manual->position(vertices[0]); manual->normal(0, -1, 0); manual->textureCoord(0, 0);

        // Right face
        manual->position(vertices[1]); manual->normal(1, 0, 0); manual->textureCoord(0, 1);
        manual->position(vertices[5]); manual->normal(1, 0, 0); manual->textureCoord(1, 1);
        manual->position(vertices[6]); manual->normal(1, 0, 0); manual->textureCoord(1, 0);
        manual->position(vertices[2]); manual->normal(1, 0, 0); manual->textureCoord(0, 0);

        // Left face
        manual->position(vertices[4]); manual->normal(-1, 0, 0); manual->textureCoord(0, 1);
        manual->position(vertices[0]); manual->normal(-1, 0, 0); manual->textureCoord(1, 1);
        manual->position(vertices[3]); manual->normal(-1, 0, 0); manual->textureCoord(1, 0);
        manual->position(vertices[7]); manual->normal(-1, 0, 0); manual->textureCoord(0, 0);

        // Define indices (2 triangles per face)
        for (int i = 0; i < 6; ++i)
        {
            int base = i * 4;
            manual->triangle(base, base + 2, base + 1);
            manual->triangle(base, base + 3, base + 2);
        }

        manual->end();

        // Set bounding box for shadow calculations
        float halfSize = size / 2.0f;
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-halfSize, -halfSize, -halfSize),
                Ogre::Vector3(halfSize, halfSize, halfSize)
            )
        );

        return 1;
    }

    // ========== CREATE SPHERE ==========

    int createSphere(Ogre::ManualObject *manual, const char* material, float radius, int rings, int segments, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        // Generate sphere vertices
        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = Ogre::Math::PI * ring / rings;
            for (int seg = 0; seg <= segments; ++seg)
            {
                float theta = 2.0f * Ogre::Math::PI * seg / segments;

                float x = radius * Ogre::Math::Sin(phi) * Ogre::Math::Cos(theta);
                float y = radius * Ogre::Math::Cos(phi);
                float z = radius * Ogre::Math::Sin(phi) * Ogre::Math::Sin(theta);

                Ogre::Vector3 pos(x, y, z);
                Ogre::Vector3 normal = pos.normalisedCopy();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)seg / segments, (float)ring / rings);
            }
        }

        // Generate indices
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = ring * (segments + 1) + seg;
                int next = current + segments + 1;

                manual->triangle(current, next, current + 1);
                manual->triangle(current + 1, next, next + 1);
            }
        }

        manual->end();

        // Set bounding box for shadow calculations
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-radius, -radius, -radius),
                Ogre::Vector3(radius, radius, radius)
            )
        );

        return 1;
    }

    // ========== CREATE CYLINDER ==========

    int createCylinder(Ogre::ManualObject *manual, const char* material, float radius, float height, int segments, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        float halfHeight = height / 2.0f;

        // Generate cylinder body
        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            Ogre::Vector3 normal(x, 0, z);
            normal.normalise();

            // Top vertex
            manual->position(x, halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 0);

            // Bottom vertex
            manual->position(x, -halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 1);
        }

        // Generate body indices
        for (int i = 0; i < segments; ++i)
        {
            int current = i * 2;
            int next = current + 2;

            manual->triangle(current, next, current + 1);
            manual->triangle(current + 1, next, next + 1);
        }
        // Add top cap
        int topCenter = (segments + 1) * 2;
        manual->position(0, halfHeight, 0);
        manual->normal(0, 1, 0);
        manual->textureCoord(0.5f, 0.5f);

        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            manual->position(x, halfHeight, z);
            manual->normal(0, 1, 0);
            manual->textureCoord(0.5f + 0.5f * Ogre::Math::Cos(theta), 0.5f + 0.5f * Ogre::Math::Sin(theta));
        }

        // Top cap indices
        for (int i = 0; i < segments; ++i)
        {
            manual->triangle(topCenter, topCenter + i + 2, topCenter + i + 1);
        }

        // Add bottom cap
        int bottomCenter = topCenter + segments + 2;
        manual->position(0, -halfHeight, 0);
        manual->normal(0, -1, 0);
        manual->textureCoord(0.5f, 0.5f);

        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            manual->position(x, -halfHeight, z);
            manual->normal(0, -1, 0);
            manual->textureCoord(0.5f + 0.5f * Ogre::Math::Cos(theta), 0.5f + 0.5f * Ogre::Math::Sin(theta));
        }

        // Bottom cap indices
        for (int i = 0; i < segments; ++i)
        {
            manual->triangle(bottomCenter, bottomCenter + i + 1, bottomCenter + i + 2);
        }
        manual->end();

        // Set bounding box for shadow calculations
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-radius, -halfHeight, -radius),
                Ogre::Vector3(radius, halfHeight, radius)
            )
        );

        return 1;
    }

    // ========== CREATE CONE ==========

    int createCone(Ogre::ManualObject *manual, const char* material, float radius, float height, int segments, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        float halfHeight = height / 2.0f;
        Ogre::Vector3 tip(0, halfHeight, 0);

        // Generate cone body
        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            Ogre::Vector3 normal(x, radius, z);
            normal.normalise();

            // Base vertex
            manual->position(x, -halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 1);

            // Tip vertex
            manual->position(tip);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 0);
        }

        // Generate indices
        for (int i = 0; i < segments; ++i)
        {
            int current = i * 2;
            int next = current + 2;

            manual->triangle(current, current + 1, next);
        }

        manual->end();

        // Set bounding box for shadow calculations
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-radius, -halfHeight, -radius),
                Ogre::Vector3(radius, halfHeight, radius)
            )
        );

        return 1;
    }

    // ========== CREATE TORUS ==========

    int createTorus(Ogre::ManualObject *manual, const char* material, float majorRadius, float minorRadius, int majorSegments, int minorSegments, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        // Generate torus vertices
        for (int i = 0; i <= majorSegments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / majorSegments;
            float cosTheta = Ogre::Math::Cos(theta);
            float sinTheta = Ogre::Math::Sin(theta);

            for (int j = 0; j <= minorSegments; ++j)
            {
                float phi = 2.0f * Ogre::Math::PI * j / minorSegments;
                float cosPhi = Ogre::Math::Cos(phi);
                float sinPhi = Ogre::Math::Sin(phi);

                float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
                float y = minorRadius * sinPhi;
                float z = (majorRadius + minorRadius * cosPhi) * sinTheta;

                Ogre::Vector3 center(majorRadius * cosTheta, 0, majorRadius * sinTheta);
                Ogre::Vector3 pos(x, y, z);
                Ogre::Vector3 normal = (pos - center).normalisedCopy();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)i / majorSegments, (float)j / minorSegments);
            }
        }

        // Generate indices
        for (int i = 0; i < majorSegments; ++i)
        {
            for (int j = 0; j < minorSegments; ++j)
            {
                int current = i * (minorSegments + 1) + j;
                int next = current + minorSegments + 1;

                manual->triangle(current, next, current + 1);
                manual->triangle(current + 1, next, next + 1);
            }
        }

        manual->end();

        // Set bounding box for shadow calculations
        float outerRadius = majorRadius + minorRadius;
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-outerRadius, -minorRadius, -outerRadius),
                Ogre::Vector3(outerRadius, minorRadius, outerRadius)
            )
        );

        return 1;
    }

  
    // ========== CREATE QUAD ==========

    int createQuad(Ogre::ManualObject *manual, const char* material, float width, float height, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        float w = width / 2.0f;
        float h = height / 2.0f;

        // Define 4 vertices of quad
        Ogre::Vector3 vertices[4] = {
            Ogre::Vector3(-w, 0, -h),  // 0 - front left
            Ogre::Vector3( w, 0, -h),  // 1 - front right
            Ogre::Vector3( w, 0,  h),  // 2 - back right
            Ogre::Vector3(-w, 0,  h)   // 3 - back left
        };

        // Front face (normal pointing up)
        manual->position(vertices[0]); manual->normal(0, 1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[1]); manual->normal(0, 1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[2]); manual->normal(0, 1, 0); manual->textureCoord(1, 0);
        manual->position(vertices[3]); manual->normal(0, 1, 0); manual->textureCoord(0, 0);

        // Back face (normal pointing down)
        manual->position(vertices[2]); manual->normal(0, -1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[1]); manual->normal(0, -1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[0]); manual->normal(0, -1, 0); manual->textureCoord(0, 0);
        manual->position(vertices[3]); manual->normal(0, -1, 0); manual->textureCoord(1, 0);

        // Front face triangles
        manual->triangle(0, 1, 2);
        manual->triangle(0, 2, 3);

        // Back face triangles
        manual->triangle(4, 5, 6);
        manual->triangle(4, 6, 7);

        manual->end();

        // Set bounding box
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-w, -0.001f, -h),
                Ogre::Vector3(w, 0.001f, h)
            )
        );

        return 1;
    }

    // ========== CREATE PLANE ==========

    int createPlane(Ogre::ManualObject *manual, const char* material, float width, float depth, int widthSegments, int depthSegments, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        float w = width / 2.0f;
        float d = depth / 2.0f;

        // Generate vertices
        for (int z = 0; z <= depthSegments; ++z)
        {
            for (int x = 0; x <= widthSegments; ++x)
            {
                float posX = -w + (x * width / widthSegments);
                float posZ = -d + (z * depth / depthSegments);

                manual->position(posX, 0, posZ);
                manual->normal(0, 1, 0);
                manual->textureCoord((float)x / widthSegments, (float)z / depthSegments);
            }
        }

        // Generate indices
        for (int z = 0; z < depthSegments; ++z)
        {
            for (int x = 0; x < widthSegments; ++x)
            {
                int current = z * (widthSegments + 1) + x;
                int next = current + widthSegments + 1;

                manual->triangle(current, current + 1, next);
                manual->triangle(current + 1, next + 1, next);
            }
        }

        manual->end();

        // Set bounding box
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-w, -0.001f, -d),
                Ogre::Vector3(w, 0.001f, d)
            )
        );

        return 1;
    }

    // ========== CREATE CAPSULE ==========

    int createCapsule(Ogre::ManualObject *manual, const char* material, float radius, float height, int segments, int rings, const char* group)
    {
        Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

        if(group != NULL)
        {
            groupName = group;
        }

        manual->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST, groupName);

        float halfHeight = height / 2.0f;

        // ========== CYLINDER BODY ==========
        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            Ogre::Vector3 normal(x, 0, z);
            normal.normalise();

            // Top vertex
            manual->position(x, halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 0);

            // Bottom vertex
            manual->position(x, -halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 1);
        }

        // Cylinder body indices
        for (int i = 0; i < segments; ++i)
        {
            int current = i * 2;
            int next = current + 2;

            manual->triangle(current, current + 1, next);
            manual->triangle(current + 1, next + 1, next);
        }

        // ========== TOP HEMISPHERE ==========
        int topStart = (segments + 1) * 2;
        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = Ogre::Math::PI * ring / (rings * 2.0f);
            float sinPhi = Ogre::Math::Sin(phi);
            float cosPhi = Ogre::Math::Cos(phi);

            for (int seg = 0; seg <= segments; ++seg)
            {
                float theta = 2.0f * Ogre::Math::PI * seg / segments;
                float x = radius * Ogre::Math::Cos(theta) * sinPhi;
                float y = radius * cosPhi;
                float z = radius * Ogre::Math::Sin(theta) * sinPhi;

                Ogre::Vector3 pos(x, halfHeight + y, z);
                Ogre::Vector3 normal = pos - Ogre::Vector3(0, halfHeight, 0);
                normal.normalise();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)seg / segments, (float)ring / rings);
            }
        }

        // Top hemisphere indices
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = topStart + ring * (segments + 1) + seg;
                int next = current + segments + 1;

                manual->triangle(current, next, current + 1);
                manual->triangle(current + 1, next, next + 1);
            }
        }

        // ========== BOTTOM HEMISPHERE ==========
        int bottomStart = topStart + (rings + 1) * (segments + 1);
        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = Ogre::Math::PI * ring / (rings * 2.0f);
            float sinPhi = Ogre::Math::Sin(phi);
            float cosPhi = Ogre::Math::Cos(phi);

            for (int seg = 0; seg <= segments; ++seg)
            {
                float theta = 2.0f * Ogre::Math::PI * seg / segments;
                float x = radius * Ogre::Math::Cos(theta) * sinPhi;
                float y = radius * cosPhi;
                float z = radius * Ogre::Math::Sin(theta) * sinPhi;

                Ogre::Vector3 pos(x, -halfHeight - y, z);
                Ogre::Vector3 normal = pos - Ogre::Vector3(0, -halfHeight, 0);
                normal.normalise();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)seg / segments, (float)ring / rings);
            }
        }

        // Bottom hemisphere indices
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = bottomStart + ring * (segments + 1) + seg;
                int next = current + segments + 1;

                manual->triangle(current, current + 1, next);
                manual->triangle(current + 1, next + 1, next);
            }
        }

        manual->end();

        // Set bounding box
        float totalHeight = height + radius * 2.0f;
        manual->setBoundingBox(
            Ogre::AxisAlignedBox(
                Ogre::Vector3(-radius, -totalHeight / 2.0f, -radius),
                Ogre::Vector3(radius, totalHeight / 2.0f, radius)
            )
        );

        return 1;
    }

} // namespace ProceduralMeshBindings
