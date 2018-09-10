#include <core/core.hpp>
#include <game/enemy.hpp>
#include <game/game_loop.hpp>
#include <geometry/game_position.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <modules/module_io.hpp>
#include <util/util.hpp>

#include <boost/lexical_cast.hpp>

Enemy::Enemy(irr::scene::ISceneNode* _node, const std::function<GamePosition(void)>& _ai_func)
        : node(_node), ai_func(_ai_func), movementSpeed(3)
{
}

void Enemy::hit(double damage)
{
    healthLeft -= damage;
}

double Enemy::getHealthLeft() const
{
    return healthLeft;
}

void Enemy::setHealthLeft(double health)
{
    healthLeft = health;
}

double Enemy::getHealthMax() const
{
    return healthMax;
}
void Enemy::setHealthMax(double health)
{
    healthMax = health;
}

GamePosition Enemy::getPosition() const
{
    position = GamePosition(node->getPosition());
    return position;
}
void Enemy::setPosition(const GamePosition& newPosition)
{
    position = newPosition;
    node->setPosition(newPosition.toIrrVector3df());
}

double Enemy::getMovementSpeed() const
{
    return movementSpeed;
}
void Enemy::setMovementSpeed(double newSpeed)
{
    movementSpeed = newSpeed;
}

irr::scene::ISceneNode* Enemy::sceneNode() const
{
    return node;
}

void Enemy::ai()
{
    auto target = ai_func();
    auto self_position = getPosition();
    auto vec = (target.toIrrVector3df() - self_position.toIrrVector3df());
    vec.Y = 0;

    const double treshold = 5;
    if (vec.getLengthSQ() < treshold) {
        movementSpeed = 0;
    }

    vec = vec.normalize() * movementSpeed;
    auto rotation = getRotationByTarget(getPosition(), ai_func());
    rotation.X = 0;
    rotation.Z = 0;
    node->setRotation(rotation);
    setPosition(GamePosition(self_position.toIrrVector3df() + vec));
}

EnemyId EnemyManager::createEnemy(irr::scene::IMesh* model,
                                  irr::video::ITexture* texture,
                                  const irr::core::vector3df& scale)
{
    ++idCounter;
    enemies.insert({idCounter, Enemy(graphicsCreateMeshSceneNode(model))});

    Enemy& enemy = enemies.at(idCounter);
    enemy.sceneNode()->setMaterialTexture(0, texture);
    enemy.sceneNode()->setScale(scale);
    return idCounter;
}

void EnemyManager::deleteEnemy(EnemyId id)
{
    enemies.erase(id);
}

const Enemy& EnemyManager::accessEnemy(EnemyId id)
{
    return enemies.at(id);
}
Enemy& EnemyManager::mutableAccessEnemy(EnemyId id)
{
    return enemies.at(id);
}

EnemyManager enemyManager;

// TODO: переместить в другое место
FuncResult handlerEachTickWithHandle(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Invalid number of arguments for handlerEnemySyncDrawable");
    }
    FuncResult ret;
    std::string name = getArgument<std::string>(args, 0);
    uint64_t param = getArgument<uint64_t>(args, 1);
    eachTickWithParam(name, param);

    return ret;
}

FuncResult handlerEnemyProcessAi(const std::vector<void*>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Invalid number of arguments for handlerEnemyProcessAi");
    }
    FuncResult ret;

    auto instanceHandle = getArgument<uint64_t>(args, 0);
    auto& instance = getModuleClassInstance(instanceHandle);
    const auto& moduleClass = getModuleClass(instance.className);
    const auto& funcProvider = getFuncProvider(
            getFuncProviderHandle(moduleClass.methods.at("ai").name));
    FuncResult result = funcProvider(args);
    auto action = getArgument<std::string>(result.data, 0);
    dyntypeDelete(result.data.at(0), 's');

    std::vector<std::string> split;
    boost::algorithm::split(split, action, [](char c) { return c == ';'; });

    auto modelHandle = instance.members.at("model").get<uint64_t>();
    for (const std::string& act : split) {
        std::vector<std::string> parts;
        boost::algorithm::split(parts, act, [](char c) { return c == ' '; });
        if (parts.size() == 0) {
            continue;
        }
        if (parts.at(0) == "jump") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'jump' action");
            }
            float jumpSpeed = boost::lexical_cast<float>(parts.at(1));
            graphicsJump(drawablesManager.access(modelHandle), jumpSpeed);
        } else if (parts.at(0) == "lookat") {
            if (parts.size() != 4) {
                throw std::runtime_error("Invalid number of arguments for 'lookat' action");
            }
            float x = boost::lexical_cast<float>(parts.at(1));
            float y = boost::lexical_cast<float>(parts.at(2));
            float z = boost::lexical_cast<float>(parts.at(3));
            graphicsLookAt(drawablesManager.access(modelHandle), x, y, z);
        } else if (parts.at(0) == "step") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'step' action");
            }
            float distance = boost::lexical_cast<float>(parts.at(1));
            graphicsStep(drawablesManager.access(modelHandle), distance);
        } else {
            throw std::runtime_error(std::string("Unknown action: '") + parts.at(0) + "'");
        }
    }
    float x, y, z;
    graphicsGetPosition(drawablesManager.access(modelHandle), x, y, z);
    instance.members.at("x").set<double>(x);
    instance.members.at("y").set<double>(y);
    instance.members.at("z").set<double>(z);

    return ret;
}

void initializeEnemies()
{
    registerFuncProvider(
            FuncProvider("core.eachTickWithHandle", handlerEachTickWithHandle), "sL", "");
    registerFuncProvider(FuncProvider("enemy.processAi", handlerEnemyProcessAi), "L", "");
    addModuleClass("graphics.Drawable",
                   ModuleClass({{"model", {'L'}}}, {}, "graphics.Drawable", ""));
    addModuleClass("game.Enemy",
                   ModuleClass({{"x", {'F'}}, {"y", {'F'}}, {"z", {'F'}}, {"hp", {'F'}}},
                               {{"ai", {"core.class.nop", "Lo", "os"}}},
                               "game.Enemy",
                               "graphics.Drawable"));
}
