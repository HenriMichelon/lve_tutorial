#pragma once

#include "ze_model.hpp"

#include <memory>

namespace ze {
    struct Transform2dComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f };
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{ {c, s}, {-s, c} };
            glm::mat2 scaleMatrix{{scale.x, 0.0f}, { 0.0f, scale.y}};
            return rotMatrix * scaleMatrix;
        }
    };

    class ZeGameObject {
    public:
        using id_t = unsigned int;

        static ZeGameObject createGameObject() {
            static id_t currentId = 0;
            return ZeGameObject{currentId++};
        }

        id_t getId() { return id; }

        ZeGameObject(const ZeGameObject &) = delete;
        ZeGameObject& operator=(const ZeGameObject &) = delete;
        ZeGameObject(ZeGameObject &&) = default;
        ZeGameObject &operator=(ZeGameObject &&) = delete;

        std::shared_ptr<ZeModel> model {};
        glm::vec3 color{};
        Transform2dComponent transform2D {};

    private:
        id_t id;

        explicit ZeGameObject(id_t objId): id{objId} {};

    };
}