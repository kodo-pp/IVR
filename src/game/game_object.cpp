#include <mutex>

#include <modbox/game/game_object.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/handle_storage.hpp>
#include <modbox/util/util.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/core/event_manager.hpp>
#include <modbox/core/core.hpp>

GameObject::GameObject(irr::scene::ISceneNode* _node, const std::string& _kind, GameObjectId _id)
        : node(_node), kind(_kind), id(_id)
{
    selector = graphicsCreateTriangleSelector(node);
    addSubSelector("gameObjects", selector);
    selector->grab();
    node->grab();
}

GameObject::GameObject(const GameObject& other)
        : node(other.node), kind(other.kind), id(other.id), selector(other.selector)
{
    node->grab();
    selector->grab();
}
GameObject::GameObject(GameObject&& other)
        : node(other.node), kind(other.kind), id(other.id), selector(other.selector)
{
    node->grab();
    selector->grab();
}

GameObject::~GameObject()
{
    selector->drop();
    if (selector->getReferenceCount() == 2) {
        removeSubSelector("gameObjects", selector);
    }
    node->drop();
    if (node->getReferenceCount() == 1) {
        node->remove();
    }
}

GameObject& GameObject::operator=(const GameObject& other)
{
    GameObject copy(other);
    std::swap(*this, copy);
    return *this;
}
GameObject& GameObject::operator=(GameObject&& other)
{
    std::swap(*this, other);
    return *this;
}

GamePosition GameObject::getPosition() const
{
    auto position = GamePosition(node->getPosition());
    return position;
}
void GameObject::setPosition(const GamePosition& newPosition)
{
    node->setPosition(newPosition.toIrrVector3df());
}

irr::scene::ISceneNode* GameObject::sceneNode() const
{
    return node;
}

std::string GameObject::getKind() const {
    return kind;
}

GameObjectId GameObjectManager::createGameObject(const std::string& kind, irr::scene::ISceneNode* model)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    ++idCounter;
    gameObjects.emplace(idCounter, GameObject(model, kind, idCounter));
    getEventManager().raiseEvent("gameObject.create", {{"kind", kind}, {"id", std::to_string(idCounter)}});
    return idCounter;
}

void GameObjectManager::deleteGameObject(GameObjectId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    getEventManager().raiseEvent("gameObject.delete", {{"kind", gameObjects.at(id).getKind()}, {"id", std::to_string(id)}});
    gameObjects.erase(id);
}

const GameObject& GameObjectManager::access(GameObjectId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return gameObjects.at(id);
}
GameObject& GameObjectManager::mutableAccess(GameObjectId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return gameObjects.at(id);
}

GameObjectManager& getGameObjectManager()
{
    static GameObjectManager manager;
    return manager;
}

void GameObjectManager::addKind(const std::string& kind) {
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (kinds.count(kind) > 0) {
        throw std::runtime_error("Game object kind '" + kind + "' already registered");
    }
    getEventManager().raiseEvent("gameObject.newKind", {{"kind", kind}});
}

std::optional<GameObjectId> GameObjectManager::reverseLookup(irr::scene::ISceneNode* drawable)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    for (auto [id, object] : gameObjects) {
        if (object.sceneNode() == drawable) {
            return id;
        }
    }
    return {};
}
FuncResult handlerAddGameObjectKind(const std::vector <std::string>& args)
{
    FuncResult ret;
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerAddGameObjectKind()");
    }

    auto kind = getArgument<std::string>(args, 0);
    getGameObjectManager().addKind(kind);
    return ret;
}
FuncResult handlerAddGameObject(const std::vector <std::string>& args)
{
    FuncResult ret;
    ret.data.resize(1);
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerAddGameObject()");
    }

    auto kind = getArgument<std::string>(args, 0);
    auto drawableHandle = getArgument<uint64_t>(args, 1);

    auto gameObjectId = getGameObjectManager().createGameObject(
        kind,
        drawablesManager.access(drawableHandle)
    );
    
    setReturn(ret, 0, gameObjectId);
    return ret;
}
FuncResult handlerRemoveGameObject(const std::vector <std::string>& args)
{
    FuncResult ret;
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerRemoveGameObject()");
    }

    auto gameObjectId = getArgument<uint64_t>(args, 0);
    getGameObjectManager().deleteGameObject(gameObjectId);
    return ret;
}

void initializeGameObjects()
{
    registerFuncProvider(FuncProvider("gameObject.addKind", handlerAddGameObjectKind), "s", "");
    registerFuncProvider(FuncProvider("gameObject.add", handlerAddGameObject), "su", "u");
    registerFuncProvider(FuncProvider("gameObject.remove", handlerRemoveGameObject), "su", "");
}

