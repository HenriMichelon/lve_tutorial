#pragma once

#include "ze_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace ze {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f };
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class ZeGameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, ZeGameObject>;

        static ZeGameObject createGameObject() {
            static id_t currentId = 0;
            return ZeGameObject{currentId++};
        }

        static ZeGameObject makePointLight(
                float intensity = 10.0f,
                float radius = 0.1f,
                glm::vec3 color = glm::vec3(1.0f)
                );

        id_t getId() { return id; }

        ZeGameObject(const ZeGameObject &) = delete;
        ZeGameObject& operator=(const ZeGameObject &) = delete;
        ZeGameObject(ZeGameObject &&) = default;
        ZeGameObject &operator=(ZeGameObject &&) = delete;

        glm::vec3 color{};
        TransformComponent transform{};

        // optional pointer components
        std::shared_ptr<ZeModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        id_t id;

        explicit ZeGameObject(id_t objId): id{objId} {};

    };
}